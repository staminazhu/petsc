#ifndef lint
static char vcid[] = "$Id: gcreatev.c,v 1.20 1995/08/22 16:28:38 bsmith Exp bsmith $";
#endif


#include "sys.h"
#include "petsc.h"
#include "sysio.h"
#include "is.h"
#include "vec.h"    /*I "vec.h" I*/

/*@C
    VecCreate - Creates a vector, where the vector type is determined 
    from the options database.  Generates a parallel MPI vector if the 
    communicator has more than one processor.

    Input Parameters:
.   comm - MPI communicator
.   n - global vector length
 
    Output Parameter:
.   V - location to stash resulting vector

    Options Database Key:
$   -vec_mpi : use MPI vectors, even for the uniprocessor case

    Notes:
    Use VecDuplicate() or VecGetVecs() to form additional vectors
    of the same type as an existing vector.

.keywords: vector, create, initial

.seealso: VecCreateSequential(), VecCreateMPI(), VecDuplicate(), VecGetVecs()
@*/
int VecCreate(MPI_Comm comm,int n,Vec *V)
{
  int numtid;
  MPI_Comm_size(comm,&numtid);
  if (OptionsHasName(0,"-help")) {
    MPIU_printf(comm,"VecCreate() option: -vec_mpi\n");
  }
  if (numtid > 1 || OptionsHasName(0,"-vec_mpi")) {
    return VecCreateMPI(comm,PETSC_DECIDE,n,V);
  }
  return VecCreateSequential(comm,n,V);
}
 
