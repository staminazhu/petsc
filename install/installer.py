#!/usr/bin/env python
import importer
import install.base
import install.build
import install.retrieval

import os
import sys

class Installer(install.base.Base):
  def __init__(self, clArgs = None, localDict = 0, initDict = None):
    install.base.Base.__init__(self, self.setupArgDB(clArgs, localDict, initDict))
    self.retriever = install.retrieval.Retriever(self.argDB)
    self.builder   = install.build.Builder(self.argDB)
    self.force     = self.argDB['forceInstall']
    return

  def setupArgDB(self, clArgs, localDict, initDict):
    import nargs
    argDB = nargs.ArgDict('ArgDict', localDict = localDict)

    argDB.setLocalType('backup',            nargs.ArgBool('Backup makes a tar archive of the generated source rather than installing'))
    argDB.setLocalType('forceInstall',      nargs.ArgBool('Forced installation overwrites any existing project'))
    argDB.setLocalType('retrievalCanExist', nargs.ArgBool('Allow a porject to exist prior to installation'))
    argDB.setLocalType('urlMappingModules', nargs.ArgString('Module name or list of names with a method setupUrlMapping(urlMaps)'))

    argDB['backup']            = 0
    argDB['forceInstall']      = 0
    argDB['retrievalCanExist'] = 0
    argDB['urlMappingModules'] = ''

    argDB.insertArgs(clArgs)
    argDB.insertArgs(initDict)
    return argDB

  def install(self, url):
    self.debugPrint('Installing '+url, 3, 'install')
    root = self.retriever.retrieve(url, force = self.force);
    self.builder.build(root)
    return

  def bootstrapInstall(self, url, argDB):
    self.debugPrint('Installing '+url+' from bootstrap', 3, 'install')
    root = self.retriever.retrieve(url, force = self.force);
    # This is for purging the sidl after the build
    self.argDB['fileset'] = 'sidl'
    self.builder.build(root, target = ['default', 'purge'], setupTarget = 'setupBootstrap')
    # Fixup install arguments
    argDB['installedprojects'] = self.argDB['installedprojects']
    return

  def backup(self, url):
    '''This forces a fresh copy of the project instead of using the one in the database'''
    import shutil

    self.debugPrint('Backing up '+url, 3, 'install')
    root = self.retriever.retrieve(url, self.getInstallRoot(url, isBackup = 1), force = self.force);
    self.builder.build(root, 'sidl', ignoreDependencies = 1)
    output = self.executeShellCommand('tar -czf '+self.getRepositoryName(self.getMappedUrl(url))+'.tgz -C '+os.path.dirname(root)+' '+os.path.basename(root))
    print 'Should remove '+os.path.dirname(root)
    #shutil.rmtree(os.path.dirname(root))
    return

if __name__ == '__main__':
  installer   = Installer(sys.argv[1:])
  compilerUrl = 'bk://sidl.bkbits.net/Compiler'
  # Must copy list since target is reset by each make below
  for url in installer.argDB.target[:]:
    if url == 'default':
      url = compilerUrl
    if installer.argDB['backup']:
      installer.backup(url)
    else:
      if installer.checkBootstrap():
        booter = Installer(localDict = 1, initDict = installer.argDB)
        booter.bootstrapInstall('bk://sidl.bkbits.net/Compiler', installer.argDB)
      if installer.checkBootstrap():
        raise RuntimeError('Should not still be bootstraping')
      installer.install(url)
