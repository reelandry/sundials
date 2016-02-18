/*
 * -----------------------------------------------------------------
 * $Revision:  $
 * $Date:  $
 * -----------------------------------------------------------------
 * Programmer(s): Slaven Peles @ LLNL
 * 
 * Based on N_Vector_Parallel by Scott D. Cohen, Alan C. Hindmarsh, 
 * Radu Serban, and Aaron Collier @ LLNL
 * -----------------------------------------------------------------
 * LLNS Copyright Start
 * Copyright (c) 2014, Lawrence Livermore National Security
 * This work was performed under the auspices of the U.S. Department 
 * of Energy by Lawrence Livermore National Laboratory in part under 
 * Contract W-7405-Eng-48 and in part under Contract DE-AC52-07NA27344.
 * Produced at the Lawrence Livermore National Laboratory.
 * All rights reserved.
 * For details, see the LICENSE file.
 * LLNS Copyright End
 * -----------------------------------------------------------------
 * This is the implementation file for a PETSc implementation
 * of the NVECTOR package.
 * -----------------------------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>

#include <nvector/nvector_petsc.h>
#include <sundials/sundials_math.h>

#define ZERO   RCONST(0.0)
#define HALF   RCONST(0.5)
#define ONE    RCONST(1.0)
#define ONEPT5 RCONST(1.5)



/* Error Message */


#define BAD_N1 "N_VNew_petsc -- Sum of local vector lengths differs from "
#define BAD_N2 "input global length. \n\n"
#define BAD_N   BAD_N1 BAD_N2

/* Private function prototypes */

/* Reduction operations add/max/min over the processor group */
static realtype VAllReduce_petsc(realtype d, int op, MPI_Comm comm);

/*
 * -----------------------------------------------------------------
 * exported functions
 * -----------------------------------------------------------------
 */

/* ----------------------------------------------------------------
 * Function to create a new parallel vector with empty data array
 */

N_Vector N_VNewEmpty_petsc(MPI_Comm comm, 
                           long int local_length,
                           long int global_length)
{
  N_Vector v;
  N_Vector_Ops ops;
  N_VectorContent_petsc content;
  long int n, Nsum;

  /* Compute global length as sum of local lengths */
  n = local_length;
  MPI_Allreduce(&n, &Nsum, 1, PVEC_INTEGER_MPI_TYPE, MPI_SUM, comm);
  if (Nsum != global_length) {
    printf(BAD_N);
    return(NULL);
  } 

  /* Create vector */
  v = NULL;
  v = (N_Vector) malloc(sizeof *v);
  if (v == NULL) return(NULL);
  
  /* Create vector operation structure */
  ops = NULL;
  ops = (N_Vector_Ops) malloc(sizeof(struct _generic_N_Vector_Ops));
  if (ops == NULL) { free(v); return(NULL); }

  ops->nvclone           = N_VClone_petsc;
  ops->nvcloneempty      = N_VCloneEmpty_petsc;
  ops->nvdestroy         = N_VDestroy_petsc;
  ops->nvspace           = N_VSpace_petsc;
  ops->nvgetarraypointer = N_VGetArrayPointer_petsc;
  ops->nvsetarraypointer = N_VSetArrayPointer_petsc;
  ops->nvlinearsum       = N_VLinearSum_petsc;
  ops->nvconst           = N_VConst_petsc;
  ops->nvprod            = N_VProd_petsc;
  ops->nvdiv             = N_VDiv_petsc;
  ops->nvscale           = N_VScale_petsc;
  ops->nvabs             = N_VAbs_petsc;
  ops->nvinv             = N_VInv_petsc;
  ops->nvaddconst        = N_VAddConst_petsc;
  ops->nvdotprod         = N_VDotProd_petsc;
  ops->nvmaxnorm         = N_VMaxNorm_petsc;
  ops->nvwrmsnormmask    = N_VWrmsNormMask_petsc;
  ops->nvwrmsnorm        = N_VWrmsNorm_petsc;
  ops->nvmin             = N_VMin_petsc;
  ops->nvwl2norm         = N_VWL2Norm_petsc;
  ops->nvl1norm          = N_VL1Norm_petsc;
  ops->nvcompare         = N_VCompare_petsc;
  ops->nvinvtest         = N_VInvTest_petsc;
  ops->nvconstrmask      = N_VConstrMask_petsc;
  ops->nvminquotient     = N_VMinQuotient_petsc;

  /* Create content */
  content = NULL;
  content = (N_VectorContent_petsc) malloc(sizeof(struct _N_VectorContent_petsc));
  if (content == NULL) { 
    free(ops); 
    free(v); 
    return(NULL); 
  }

  /* Attach lengths and communicator */
  content->local_length  = local_length;
  content->global_length = global_length;
  content->comm          = comm;
  content->own_data      = FALSE;
  content->pvec          = NULL;

  /* Attach content and ops */
  v->content = content;
  v->ops     = ops;

  return(v);
}

/* ---------------------------------------------------------------- 
 * Function to create a new parallel vector
 */

N_Vector N_VNew_petsc(MPI_Comm comm, 
                      long int local_length,
                      long int global_length)
{
  N_Vector v;
  Vec *pvec = NULL;
  PetscErrorCode ierr;
  PetscBool ok;

  /* Check if PETSc is initialized and exit if it is not */
  ierr = PetscInitialized(&ok);
  if(!ok) {
    fprintf(stderr, "PETSc not initialized!\n");
    return NULL;
  }
  
  v = NULL;
  v = N_VNewEmpty_petsc(comm, local_length, global_length);
  if (v == NULL) return(NULL);

  /* Create data */
  if(local_length > 0) {

    /* Allocate empty PETSc vector */
    pvec = (Vec*) malloc(sizeof(Vec));
    if(pvec == NULL) { 
      N_VDestroy_petsc(v); 
      return(NULL);
    }
    
    ierr = VecCreate(comm, pvec);
    //CHKERRQ(ierr);
    ierr = VecSetSizes(*pvec, local_length, global_length);
    //CHKERRQ(ierr);
    ierr = VecSetFromOptions(*pvec);
    //CHKERRQ(ierr);

    /* Attach data */
    NV_OWN_DATA_PTC(v) = TRUE;
    NV_PVEC_PTC(v)     = pvec; 
  }

  return(v);
}




/* ---------------------------------------------------------------- 
 * Function to create a parallel N_Vector with user data component
 * This function is NOT implemented for PETSc wrapper!
 */

N_Vector N_VMake_petsc(Vec *pvec)
{
  N_Vector v = NULL;
  MPI_Comm comm;
  PetscInt local_length;
  PetscInt global_length;

  VecGetLocalSize(*pvec, &local_length);
  VecGetSize(*pvec, &global_length);
  PetscObjectGetComm((PetscObject) (*pvec), &comm);
  
  v = N_VNewEmpty_petsc(comm, local_length, global_length);
  if (v == NULL) return(NULL);

  if (local_length > 0) {
    /* Attach data */
    NV_OWN_DATA_PTC(v) = FALSE;
    NV_PVEC_PTC(v)     = pvec;
  }

  return(v);
}

/* ---------------------------------------------------------------- 
 * Function to create an array of new parallel vectors. 
 */

N_Vector *N_VCloneVectorArray_petsc(int count, N_Vector w)
{
  N_Vector *vs;
  int j;

  if (count <= 0) return(NULL);

  vs = NULL;
  vs = (N_Vector *) malloc(count * sizeof(N_Vector));
  if(vs == NULL) return(NULL);

  for (j = 0; j < count; j++) {
    vs[j] = NULL;
    vs[j] = N_VClone_petsc(w);
    if (vs[j] == NULL) {
      N_VDestroyVectorArray_petsc(vs, j-1);
      return(NULL);
    }
  }

  return(vs);
}

/* ---------------------------------------------------------------- 
 * Function to create an array of new parallel vectors with empty
 * (NULL) data array.
 */

N_Vector *N_VCloneVectorArrayEmpty_petsc(int count, N_Vector w)
{
  N_Vector *vs;
  int j;

  if (count <= 0) return(NULL);

  vs = NULL;
  vs = (N_Vector *) malloc(count * sizeof(N_Vector));
  if(vs == NULL) return(NULL);

  for (j = 0; j < count; j++) {
    vs[j] = NULL;
    vs[j] = N_VCloneEmpty_petsc(w);
    if (vs[j] == NULL) {
      N_VDestroyVectorArray_petsc(vs, j-1);
      return(NULL);
    }
  }

  return(vs);
}

/* ----------------------------------------------------------------
 * Function to free an array created with N_VCloneVectorArray_petsc
 */

void N_VDestroyVectorArray_petsc(N_Vector *vs, int count)
{
  int j;

  for (j = 0; j < count; j++) N_VDestroy_petsc(vs[j]);

  free(vs); 
  vs = NULL;

  return;
}

/* ---------------------------------------------------------------- 
 * Function to print a parallel vector 
 */

void N_VPrint_petsc(N_Vector x)
{
  Vec *xv = NV_PVEC_PTC(x);
  MPI_Comm comm = NV_COMM_PTC(x);
  
  VecView(*xv, PETSC_VIEWER_STDOUT_(comm));

  return;
}

/*
 * -----------------------------------------------------------------
 * implementation of vector operations
 * -----------------------------------------------------------------
 */

N_Vector N_VCloneEmpty_petsc(N_Vector w)
{
  N_Vector v;
  N_Vector_Ops ops;
  N_VectorContent_petsc content;

  if (w == NULL) return(NULL);

  /* Create vector */
  v = NULL;
  v = (N_Vector) malloc(sizeof *v);
  if (v == NULL) return(NULL);
  
  /* Create vector operation structure */
  ops = NULL;
  ops = (N_Vector_Ops) malloc(sizeof(struct _generic_N_Vector_Ops));
  if (ops == NULL) { 
    free(v); 
    return(NULL); 
  }
  
  ops->nvclone           = w->ops->nvclone;
  ops->nvcloneempty      = w->ops->nvcloneempty;
  ops->nvdestroy         = w->ops->nvdestroy;
  ops->nvspace           = w->ops->nvspace;
  ops->nvgetarraypointer = w->ops->nvgetarraypointer;
  ops->nvsetarraypointer = w->ops->nvsetarraypointer;
  ops->nvlinearsum       = w->ops->nvlinearsum;
  ops->nvconst           = w->ops->nvconst;  
  ops->nvprod            = w->ops->nvprod;   
  ops->nvdiv             = w->ops->nvdiv;
  ops->nvscale           = w->ops->nvscale; 
  ops->nvabs             = w->ops->nvabs;
  ops->nvinv             = w->ops->nvinv;
  ops->nvaddconst        = w->ops->nvaddconst;
  ops->nvdotprod         = w->ops->nvdotprod;
  ops->nvmaxnorm         = w->ops->nvmaxnorm;
  ops->nvwrmsnormmask    = w->ops->nvwrmsnormmask;
  ops->nvwrmsnorm        = w->ops->nvwrmsnorm;
  ops->nvmin             = w->ops->nvmin;
  ops->nvwl2norm         = w->ops->nvwl2norm;
  ops->nvl1norm          = w->ops->nvl1norm;
  ops->nvcompare         = w->ops->nvcompare;    
  ops->nvinvtest         = w->ops->nvinvtest;
  ops->nvconstrmask      = w->ops->nvconstrmask;
  ops->nvminquotient     = w->ops->nvminquotient;

  /* Create content */  
  content = NULL;
  content = (N_VectorContent_petsc) malloc(sizeof(struct _N_VectorContent_petsc));
  if (content == NULL) { 
    free(ops); 
    free(v); 
    return(NULL); 
  }

  /* Attach lengths and communicator */
  content->local_length  = NV_LOCLENGTH_PTC(w);
  content->global_length = NV_GLOBLENGTH_PTC(w);
  content->comm          = NV_COMM_PTC(w);
  content->own_data      = FALSE;
  content->pvec          = NULL;

  /* Attach content and ops */
  v->content = content;
  v->ops     = ops;

  return(v);
}

N_Vector N_VClone_petsc(N_Vector w)
{
  N_Vector v     = NULL;
  Vec *pvec      = NULL;
  Vec *wvec      = NV_PVEC_PTC(w);
  
  PetscInt local_length  = NV_LOCLENGTH_PTC(w);
  PetscInt global_length = NV_GLOBLENGTH_PTC(w);
  MPI_Comm comm          = NV_COMM_PTC(w);
  
  PetscErrorCode ierr;
  
//   ierr = VecGetSize(*wvec, &global_length);
//   ierr = VecGetLocalSize(*wvec, &local_length);
//   ierr = PetscObjectGetComm((PetscObject) (*wvec), &comm);

  v = N_VCloneEmpty_petsc(w);
  if (v == NULL) return(NULL);

  /* Create data */
  if(local_length > 0) {

    /* Allocate empty PETSc vector */
    pvec = (Vec*) malloc(sizeof(Vec));
    if(pvec == NULL) {
      N_VDestroy_petsc(v); 
      return(NULL);
    }
    
    ierr = VecDuplicate(*wvec, pvec);
    //CHKERRQ(ierr);
    if(pvec == NULL) {
      N_VDestroy_petsc(v); 
      return(NULL);
    }
    

    
//     ierr = VecCreate(comm, pvec);
//     //CHKERRQ(ierr);
//     ierr = VecSetSizes(*pvec, local_length, global_length);
//     //CHKERRQ(ierr);
//     ierr = VecSetFromOptions(*pvec);
//     //CHKERRQ(ierr);

    /* Attach data */
    NV_OWN_DATA_PTC(v) = TRUE;
    NV_PVEC_PTC(v)     = pvec;
  }

  return(v);
}

void N_VDestroy_petsc(N_Vector v)
{
  PetscErrorCode ierr;
  
  if ((NV_OWN_DATA_PTC(v) == TRUE) && (NV_PVEC_PTC(v) != NULL)) {
    ierr = VecDestroy((NV_PVEC_PTC(v)));
    //CHKERRQ(ierr);
    NV_PVEC_PTC(v) = NULL;
  }
  
  free(v->content); 
  v->content = NULL;
  free(v->ops); 
  v->ops = NULL;
  free(v); 
  v = NULL;

  return;
}

void N_VSpace_petsc(N_Vector v, long int *lrw, long int *liw)
{
  MPI_Comm comm;
  int npes;

  comm = NV_COMM_PTC(v);
  MPI_Comm_size(comm, &npes);
  
  *lrw = NV_GLOBLENGTH_PTC(v);
  *liw = 2*npes;

  return;
}

/*
 * Not implemented for PETSc wrapper.
 */
realtype *N_VGetArrayPointer_petsc(N_Vector v)
{
  return NULL;
}

/*
 * Not implemented for PETSc wrapper.
 */
void N_VSetArrayPointer_petsc(realtype *v_data, N_Vector v)
{
  return;
}

void N_VLinearSum_petsc(realtype a, N_Vector x, realtype b, N_Vector y, N_Vector z)
{
  long int i;
  realtype c;
  N_Vector v1, v2;
  booleantype test;
  Vec *xv = NV_PVEC_PTC(x);
  Vec *yv = NV_PVEC_PTC(y);
  Vec *zv = NV_PVEC_PTC(z);
  
  if (x == y) {
    N_VScale_petsc(a + b, x, z); /* z <~ ax+bx */
    return;
  }

  if (z == y) {
    if (b == ONE) { 
      VecAXPY(*yv, a, *xv);   /* BLAS usage: axpy  y <- ax+y */
      return;
    }
    VecAXPBY(*yv, a, b, *xv); /* BLAS usage: axpby y <- ax+by */
    return;
  }

  if (z == x) {
    if (a == ONE) { 
      VecAXPY(*xv, b, *yv);   /* BLAS usage: axpy  x <- by+x */
      return;
    }
    VecAXPBY(*xv, b, a, *yv); /* BLAS usage: axpby x <- by+ax */
    return;
  }

//   /* Case: a == b == 1.0 */
// 
//   if ((a == ONE) && (b == ONE)) {
//     VecAXPBYPCZ(*zv, a, b, 0.0, *xv, *yv); // PETSc, probably not optimal 
//     return;
//   }
// 
//   /* Cases: (1) a == 1.0, b = -1.0, (2) a == -1.0, b == 1.0 */
// 
//   if ((test = ((a == ONE) && (b == -ONE))) || ((a == -ONE) && (b == ONE))) {
//     VecAXPBYPCZ(*zv, a, b, 0.0, *xv, *yv); // PETSc, probably not optimal 
//     return;
//   }
// 
//   /* Cases: (1) a == 1.0, b == other or 0.0, (2) a == other or 0.0, b == 1.0 */
//   /* if a or b is 0.0, then user should have called N_VScale */
// 
//   if ((test = (a == ONE)) || (b == ONE)) {
//     VecAXPBYPCZ(*zv, a, b, 0.0, *xv, *yv); // PETSc, probably not optimal 
//     return;
//   }
// 
//   /* Cases: (1) a == -1.0, b != 1.0, (2) a != 1.0, b == -1.0 */
// 
//   if ((test = (a == -ONE)) || (b == -ONE)) {
//     VecAXPBYPCZ(*zv, a, b, 0.0, *xv, *yv); // PETSc, probably not optimal 
//     return;
//   }
// 
//   /* Case: a == b */
//   /* catches case both a and b are 0.0 - user should have called N_VConst */
// 
//   if (a == b) {
//     VecAXPBYPCZ(*zv, a, b, 0.0, *xv, *yv); // PETSc, probably not optimal 
//     return;
//   }
// 
//   /* Case: a == -b */
// 
//   if (a == -b) {
//     VecAXPBYPCZ(*zv, a, b, 0.0, *xv, *yv); // PETSc, probably not optimal 
//     return;
//   }

  /* Do all cases not handled above:
     (1) a == other, b == 0.0 - user should have called N_VScale
     (2) a == 0.0, b == other - user should have called N_VScale
     (3) a,b == other, a !=b, a != -b */
  
  VecAXPBYPCZ(*zv, a, b, 0.0, *xv, *yv); // PETSc, probably not optimal 

  return;
}

void N_VConst_petsc(realtype c, N_Vector z)
{
  Vec *zv      = NV_PVEC_PTC(z);

  VecSet(*zv, c);
  
  return;
}

void N_VProd_petsc(N_Vector x, N_Vector y, N_Vector z)
{
  Vec *xv = NV_PVEC_PTC(x);
  Vec *yv = NV_PVEC_PTC(y);
  Vec *zv = NV_PVEC_PTC(z);
  
  VecPointwiseMult(*zv, *xv, *yv);
  
  return;
}

void N_VDiv_petsc(N_Vector x, N_Vector y, N_Vector z)
{
  Vec *xv = NV_PVEC_PTC(x);
  Vec *yv = NV_PVEC_PTC(y);
  Vec *zv = NV_PVEC_PTC(z);

  VecPointwiseDivide(*zv, *xv, *yv); /* z = x/y */

  return;
}

void N_VScale_petsc(realtype c, N_Vector x, N_Vector z)
{
  Vec *xv = NV_PVEC_PTC(x);
  Vec *zv = NV_PVEC_PTC(z);

  if (z == x) {       /* BLAS usage: scale x <- cx */
    VecScale(*xv, c);
    return;
  }
  
  VecAXPBY(*zv, c, 0.0, *xv); // PETSc; is it optimal?

  return;
}

void N_VAbs_petsc(N_Vector x, N_Vector z)
{
  Vec *xv = NV_PVEC_PTC(x);
  Vec *zv = NV_PVEC_PTC(z);

  if(z != x)
    VecCopy(*xv, *zv); /* copy x~>z */
  VecAbs(*zv); // PETSc
  
  return;
}

void N_VInv_petsc(N_Vector x, N_Vector z)
{
  Vec *xv = NV_PVEC_PTC(x);
  Vec *zv = NV_PVEC_PTC(z);

  if(z != x)
    VecCopy(*xv, *zv); /* copy x~>z */
  VecReciprocal(*zv); // PETSc

  return;
}

void N_VAddConst_petsc(N_Vector x, realtype b, N_Vector z)
{
  Vec *xv = NV_PVEC_PTC(x);
  Vec *zv = NV_PVEC_PTC(z);
  PetscErrorCode ierr;

  if(z != x)
    VecCopy(*xv, *zv); /* copy x~>z */
  VecShift(*zv, b); // PETSc
  
  return;
}

realtype N_VDotProd_petsc(N_Vector x, N_Vector y)
{
  Vec *xv = NV_PVEC_PTC(x);
  Vec *yv = NV_PVEC_PTC(y);
  PetscScalar dotprod;
  PetscErrorCode ierr;
  
  ierr = VecDot(*xv, *yv, &dotprod);
  return dotprod;
}

realtype N_VMaxNorm_petsc(N_Vector x)
{
  Vec *xv = NV_PVEC_PTC(x);
  PetscReal norm;
  PetscErrorCode ierr;
  
  ierr = VecNorm(*xv, NORM_INFINITY, &norm);
  
  return norm;
}

realtype N_VWrmsNorm_petsc(N_Vector x, N_Vector w)
{
  long int i;
  long int N        = NV_LOCLENGTH_PTC(x);
  long int N_global = NV_GLOBLENGTH_PTC(x);
  MPI_Comm comm     = NV_COMM_PTC(x);
  Vec *xv = NV_PVEC_PTC(x);
  Vec *wv = NV_PVEC_PTC(w);
  PetscScalar *xd;
  PetscScalar *wd;
  PetscReal sum = ZERO;
  realtype global_sum;
  
  VecGetArray(*xv, &xd);
  VecGetArray(*wv, &wd);
  for (i = 0; i < N; i++) {
    sum += PetscSqr(PetscAbsScalar(xd[i] * wd[i]));
  }
  VecRestoreArray(*xv, &xd);
  VecRestoreArray(*wv, &wd);
  
  global_sum = VAllReduce_petsc(sum, 1, comm);
  return (SUNRsqrt(global_sum/N_global)); 
}

realtype N_VWrmsNormMask_petsc(N_Vector x, N_Vector w, N_Vector id)
{
  long int i;
  long int N        = NV_LOCLENGTH_PTC(x);
  long int N_global = NV_GLOBLENGTH_PTC(x);
  MPI_Comm comm     = NV_COMM_PTC(x);
  
  Vec *xv = NV_PVEC_PTC(x);
  Vec *wv = NV_PVEC_PTC(w);
  Vec *idv = NV_PVEC_PTC(id);
  PetscScalar *xd;
  PetscScalar *wd;
  PetscScalar *idd;
  PetscReal sum = ZERO;
  realtype global_sum;
  
  VecGetArray(*xv, &xd);
  VecGetArray(*wv, &wd);
  VecGetArray(*idv, &idd);
  for (i = 0; i < N; i++) {
    PetscReal tag = (PetscReal) idd[i];
    if (tag > ZERO) {
      sum += PetscSqr(PetscAbsScalar(xd[i] * wd[i]));
    }
  }
  VecRestoreArray(*xv, &xd);
  VecRestoreArray(*wv, &wd);
  VecRestoreArray(*idv, &idd);

  global_sum = VAllReduce_petsc(sum, 1, comm);
  return (SUNRsqrt(global_sum/N_global)); 
}

realtype N_VMin_petsc(N_Vector x)
{
  Vec *xv = NV_PVEC_PTC(x);
  PetscReal minval;
  PetscErrorCode ierr;
  PetscInt i;
  
  ierr = VecMin(*xv, &i, &minval);
  
  return minval;
}

realtype N_VWL2Norm_petsc(N_Vector x, N_Vector w)
{
  long int i;
  long int N        = NV_LOCLENGTH_PTC(x);
  MPI_Comm comm     = NV_COMM_PTC(x);

  Vec *xv = NV_PVEC_PTC(x);
  Vec *wv = NV_PVEC_PTC(w);
  PetscScalar *xd;
  PetscScalar *wd;
  PetscReal sum = ZERO;
  realtype global_sum;
  
  VecGetArray(*xv, &xd);
  VecGetArray(*wv, &wd);
  for (i = 0; i < N; i++) {
    sum += PetscSqr(PetscAbsScalar(xd[i] * wd[i]));
  }
  VecRestoreArray(*xv, &xd);
  VecRestoreArray(*wv, &wd);

  global_sum = VAllReduce_petsc(sum, 1, comm);
  return (SUNRsqrt(global_sum)); 
}

realtype N_VL1Norm_petsc(N_Vector x)
{
  Vec *xv = NV_PVEC_PTC(x);
  PetscReal norm;
  PetscErrorCode ierr;
  
  ierr = VecNorm(*xv, NORM_1, &norm);
  
  return norm;
}

void N_VCompare_petsc(realtype c, N_Vector x, N_Vector z)
{
  long int i;
  long int N = NV_LOCLENGTH_PTC(x);
  Vec *xv = NV_PVEC_PTC(x);
  Vec *zv = NV_PVEC_PTC(z);
  PetscReal cpet = c; // <~ realtype should typedef to PETScReal
  PetscScalar *xdata;
  PetscScalar *zdata;

  VecGetArray(*xv, &xdata);
  VecGetArray(*zv, &zdata);
  for (i = 0; i < N; i++) {
    zdata[i] = PetscAbsScalar(xdata[i]) >= cpet ? ONE : ZERO;
  }
  VecRestoreArray(*xv, &xdata);
  VecRestoreArray(*zv, &zdata);

  return;
}

booleantype N_VInvTest_petsc(N_Vector x, N_Vector z)
{
  Vec *xv = NV_PVEC_PTC(x);
  Vec *zv = NV_PVEC_PTC(z);
  PetscErrorCode ierr;
  PetscInt p;
  PetscReal val;
  
  if(xv != zv)
    ierr = VecCopy(*xv, *zv);
  ierr = VecReciprocal(*zv);
  ierr = VecMin(*xv, &p, &val);

  if (val == ZERO)
    return(FALSE);
  else
    return(TRUE);
}

booleantype N_VConstrMask_petsc(N_Vector c, N_Vector x, N_Vector m)
{
  long int i;
  long int N = NV_LOCLENGTH_PTC(x);
  MPI_Comm comm = NV_COMM_PTC(x);
  realtype minval = ONE;
  Vec *xv = NV_PVEC_PTC(x);
  Vec *cv = NV_PVEC_PTC(c);
  Vec *mv = NV_PVEC_PTC(m);
  PetscScalar *xd;
  PetscScalar *cd;
  PetscScalar *md;

  VecGetArray(*xv, &xd);
  VecGetArray(*cv, &cd);
  VecGetArray(*mv, &md);
  for (i = 0; i < N; i++) {
    PetscReal cc = (PetscReal) cd[i]; /* <~ Drop imaginary parts if any. */
    PetscReal xx = (PetscReal) xd[i]; /* <~ This is quick and dirty temporary fix */
    md[i] = ZERO;
    if (cc == ZERO) continue;
    if (cc > ONEPT5 || cc < -ONEPT5) {
      if (xx*cc <= ZERO) { minval = ZERO; md[i] = ONE; }
      continue;
    }
    if (cc > HALF || cc < -HALF) {
      if (xx*cc < ZERO ) { minval = ZERO; md[i] = ONE; }
    }
  }
  VecRestoreArray(*xv, &xd);
  VecRestoreArray(*cv, &cd);
  VecRestoreArray(*mv, &md);

  minval = VAllReduce_petsc(minval, 3, comm);

  if (minval == ONE) 
    return(TRUE);
  else
    return(FALSE);
}

realtype N_VMinQuotient_petsc(N_Vector num, N_Vector denom)
{
  booleantype notEvenOnce = TRUE;
  long int i; 
  long int N    = NV_LOCLENGTH_PTC(num);
  MPI_Comm comm = NV_COMM_PTC(num);

  Vec *nv = NV_PVEC_PTC(num);
  Vec *dv = NV_PVEC_PTC(denom);
  PetscScalar *nd;
  PetscScalar *dd;
  PetscReal minval = BIG_REAL;

  VecGetArray(*nv, &nd);
  VecGetArray(*dv, &dd);
  for (i = 0; i < N; i++) {
    PetscReal nr = (PetscReal) nd[i];
    PetscReal dr = (PetscReal) dd[i];
    if (dr == ZERO) 
      continue;
    else {
      if (!notEvenOnce) 
        minval = SUNMIN(minval, nr/dr);
      else {
        minval = nr/dr;
        notEvenOnce = FALSE;
      }
    }
  }
  VecRestoreArray(*nv, &nd);
  VecRestoreArray(*dv, &dd);

  return(VAllReduce_petsc(minval, 3, comm));
}

/*
 * -----------------------------------------------------------------
 * private functions
 * -----------------------------------------------------------------
 */

static realtype VAllReduce_petsc(realtype d, int op, MPI_Comm comm)
{
  /* 
   * This function does a global reduction.  The operation is
   *   sum if op = 1,
   *   max if op = 2,
   *   min if op = 3.
   * The operation is over all processors in the communicator 
   */

  realtype out;

  switch (op) {
   case 1: MPI_Allreduce(&d, &out, 1, PVEC_REAL_MPI_TYPE, MPI_SUM, comm);
           break;

   case 2: MPI_Allreduce(&d, &out, 1, PVEC_REAL_MPI_TYPE, MPI_MAX, comm);
           break;

   case 3: MPI_Allreduce(&d, &out, 1, PVEC_REAL_MPI_TYPE, MPI_MIN, comm);
           break;

   default: break;
  }

  return(out);
}
