
#ifndef lint
static char vcid[] = "$Id: snesmfj.c,v 1.12 1995/06/29 23:54:14 bsmith Exp bsmith $";
#endif

#include "draw.h"   /*I  "draw.h"   I*/
#include "snes.h"   /*I  "snes.h"   I*/
#include "ptscimpl.h"
#include "plog.h"

typedef struct {
  SNES snes;
  Vec  w;
} MFCtx_Private;

int SNESMatrixFreeDestroy_Private(void *ptr)
{
  int           ierr;
  MFCtx_Private *ctx = (MFCtx_Private* ) ptr;
  ierr = VecDestroy(ctx->w); CHKERRQ(ierr);
  PETSCFREE(ptr);
  return 0;
}  
/*
    SNESMatrixFreeMult_Private - Default matrix free form of A*u.

*/
int SNESMatrixFreeMult_Private(void *ptr,Vec dx,Vec y)
{
  MFCtx_Private *ctx = (MFCtx_Private* ) ptr;
  SNES          snes = ctx->snes;
  double        norm,epsilon = 1.e-8; /* assumes double precision */
  Scalar        h,dot;
  double        sum;
  Scalar        mone = -1.0;
  Vec           w = ctx->w,U,F;
  int           ierr;

  ierr = SNESGetSolution(snes,&U); CHKERRQ(ierr);
  ierr = SNESGetFunction(snes,&F); CHKERRQ(ierr);
  /* determine a "good" step size */
  VecDot(U,dx,&dot); VecASum(dx,&sum); VecNorm(dx,&norm);
  if (sum == 0.0) {dot = 1.0; norm = 1.0;}
#if defined(PETSC_COMPLEX)
  else if (abs(dot) < 1.e-16*sum && real(dot) >= 0.0) dot = 1.e-16*sum;
  else if (abs(dot) < 0.0 && real(dot) > 1.e-16*sum) dot = -1.e-16*sum;
#else
  else if (dot < 1.e-16*sum && dot >= 0.0) dot = 1.e-16*sum;
  else if (dot < 0.0 && dot > 1.e-16*sum) dot = -1.e-16*sum;
#endif
  h = epsilon*dot/(norm*norm);
  
  /* evaluate function at F(x + dx) */
  VecWAXPY(&h,dx,U,w); 
  ierr = SNESComputeFunction(snes,w,y); CHKERRQ(ierr);
  VecAXPY(&mone,F,y);
  h = -1.0/h;
  VecScale(&h,y);
  return 0;
}
/*@
     SNESDefaultMatrixFreeMatCreate - Creates a matrix-free matrix
         for use with SNES solver. You may use this matrix as
         Jacobian argument for the routine SNESSetJacobian. This is 
         most useful when you are using finite differences for a
         matrix free Newton method but explictly are forming a 
         preconditioner matrix.

  Input Parameters:
.  x - vector where SNES solution is to be stored.

  Output Parameters:
.  J - the matrix-free matrix

@*/
int SNESDefaultMatrixFreeMatCreate(SNES snes,Vec x, Mat *J)
{
  MPI_Comm      comm;
  MFCtx_Private *mfctx;
  int           n,ierr;

  mfctx = (MFCtx_Private *) PETSCMALLOC(sizeof(MFCtx_Private));CHKPTRQ(mfctx);
  mfctx->snes = snes;
  ierr = VecDuplicate(x,&mfctx->w); CHKERRQ(ierr);
  PetscObjectGetComm((PetscObject)x,&comm);
  VecGetSize(x,&n);
  ierr = MatShellCreate(comm,n,n,(void*)mfctx,J); CHKERRQ(ierr);
  MatShellSetMult(*J,SNESMatrixFreeMult_Private);
  MatShellSetDestroy(*J,SNESMatrixFreeDestroy_Private);
  PLogObjectParent(*J,mfctx->w);
  return 0;
}

