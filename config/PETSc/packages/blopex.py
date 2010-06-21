import PETSc.package

class Configure(PETSc.package.NewPackage):
  def __init__(self, framework):
    PETSc.package.NewPackage.__init__(self, framework)
    self.download   = ['http://ftp.mcs.anl.gov/pub/petsc/externalpackages/blopex-1.1.tar.gz']
    self.functions  = ['lobpcg_solve']
    self.includes   = ['interpreter.h']
    self.liblist    = [['libBLOPEX.a']]
    self.complex    = 1
    self.requires32bitint = 0
    return

  def setupDependencies(self, framework):
    PETSc.package.NewPackage.setupDependencies(self, framework)
    self.blasLapack = framework.require('config.packages.BlasLapack',self)
    self.deps       = [self.blasLapack]
    return

  def Install(self):
    import os

    g = open(os.path.join(self.packageDir,'Makefile.inc'),'w')
    self.setCompilers.pushLanguage('C')
    g.write('CC          = '+self.setCompilers.getCompiler()+'\n') 
    g.write('CFLAGS      = ' + self.setCompilers.getCompilerFlags().replace('-Wall','').replace('-Wshadow','') +'\n')
    self.setCompilers.popLanguage()
    g.write('AR          = '+self.setCompilers.AR+' '+self.setCompilers.AR_FLAGS+'\n')
    g.write('RANLIB      = '+self.setCompilers.RANLIB+'\n')
    g.close()

    if self.installNeeded('Makefile.inc'):
      try:
        self.logPrintBox('Compiling blopex; this may take several minutes')
        output,err,ret  = PETSc.package.NewPackage.executeShellCommand('cd '+self.packageDir+';BLOPEX_INSTALL_DIR='+self.installDir+';export BLOPEX_INSTALL_DIR; make clean; make; mv -f lib/* '+os.path.join(self.installDir,self.libdir)+'; cp -fp multivector/temp_multivector.h include/.; mv -f include/* '+os.path.join(self.installDir,self.includedir)+'', timeout=2500, log = self.framework.log)
      except RuntimeError, e:
        raise RuntimeError('Error running make on BLOPEX: '+str(e))
      self.postInstall(output+err,'Makefile.inc')
    return self.installDir
