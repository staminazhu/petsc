static char help[] = "Sifter Basic Ordering Tests.\n\n";

#include <petsc.h>
#include "xsifterTest.hh"

typedef ALE::Test::XSifter::arrow_type  arrow_type;
typedef ALE::Test::XSifter::xsifter_type xsifter_type;


#undef __FUNCT__
#define __FUNCT__ "BasicTest"
PetscErrorCode BasicTest(const ALE::Obj<xsifter_type>& xsifter, ALE::Test::XSifter::Options options, const char* xsifterName = NULL)
{

  PetscFunctionBegin;
  ALE::LogStage stage = ALE::LogStageRegister("Basic Test");
  ALE::LogStagePush(stage);
  xsifter->view(std::cout, xsifterName);
  ALE::LogStagePop(stage);
  PetscFunctionReturn(0);
}


#undef __FUNCT__
#define __FUNCT__ "main"
int main(int argc, char *argv[])
{
  PetscErrorCode ierr;

  PetscFunctionBegin;
  ierr = PetscInitialize(&argc, &argv, (char *) 0, help); CHKERRQ(ierr);
  {
    ALE::Test::XSifter::Options        options;
    ALE::Obj<xsifter_type> xsifterFork = ALE::Test::XSifter::createForkXSifter(PETSC_COMM_SELF, options);
    ierr = BasicTest(xsifterFork, options); CHKERRQ(ierr);
    ALE::Obj<xsifter_type> xsifterHat = ALE::Test::XSifter::createHatXSifter(PETSC_COMM_SELF, options);
    ierr = BasicTest(xsifterHat, options); CHKERRQ(ierr);
  }
  ierr = PetscFinalize();CHKERRQ(ierr);
  PetscFunctionReturn(0);
}
