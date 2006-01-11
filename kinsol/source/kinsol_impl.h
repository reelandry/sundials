/*
 * -----------------------------------------------------------------
 * $Revision: 1.14 $
 * $Date: 2006-01-11 21:14:00 $
 * -----------------------------------------------------------------
 * Programmer(s): Allan Taylor, Alan Hindmarsh, Radu Serban, and
 *                Aaron Collier @ LLNL
 * -----------------------------------------------------------------
 * Copyright (c) 2002, The Regents of the University of California.
 * Produced at the Lawrence Livermore National Laboratory.
 * All rights reserved.
 * For details, see sundials/kinsol/LICENSE.
 * -----------------------------------------------------------------
 * KINSOL solver module header file (private version)
 * -----------------------------------------------------------------
 */

#ifndef _KINSOL_IMPL_H
#define _KINSOL_IMPL_H

#ifdef __cplusplus  /* wrapper to enable C++ usage */
extern "C" {
#endif

#include "kinsol.h"

/*
 * -----------------------------------------------------------------
 * default constants
 * -----------------------------------------------------------------
 */
 
#define PRINTFL_DEFAULT    0
#define MXITER_DEFAULT     200
#define MXNBCF_DEFAULT     10
#define MSBSET_DEFAULT     10
#define MSBSET_SUB_DEFAULT 5

#define OMEGA_MIN RCONST(0.00001)
#define OMEGA_MAX RCONST(0.9)

/*
 * -----------------------------------------------------------------
 * Types : struct KINMemRec and struct *KINMem
 * -----------------------------------------------------------------
 * A variable declaration of type struct *KINMem denotes a
 * pointer to a data structure of type struct KINMemRec. The
 * KINMemRec structure contains numerous fields that must be
 * accessible by KINSOL solver module routines.
 * -----------------------------------------------------------------
 */

typedef struct KINMemRec {

  realtype kin_uround; /* machine epsilon (or unit roundoff error) 
			  (defined in shared/include/sundialstypes.h)          */

  /* problem specification data */

  KINSysFn kin_func;           /* nonlinear system function implementation     */
  void *kin_f_data;            /* work space available to func routine         */
  realtype kin_fnormtol;       /* stopping tolerance on L2-norm of function
				  value                                        */
  realtype kin_scsteptol;      /* scaled step length tolerance                 */
  int kin_globalstrategy;      /* choices are KIN_NONE and KIN_LINESEARCH      */
  int kin_printfl;             /* level of verbosity of output                 */
  long int kin_mxiter;         /* maximum number of nonlinear iterations       */
  long int kin_msbset;         /* maximum number of nonlinear iterations that
				  may be performed between calls to the
				  linear solver setup routine (lsetup)         */
  long int kin_msbset_sub;     /* subinterval length for residual monitoring   */
  long int kin_mxnbcf;         /* maximum number of beta condition failures    */
  int kin_etaflag;             /* choices are KIN_ETACONSTANT, KIN_ETACHOICE1
				  and KIN_ETACHOICE2                           */
  booleantype kin_noMinEps;    /* flag controlling whether or not the value
				  of eps is bounded below                      */
  booleantype kin_setupNonNull;   /* flag indicating if linear solver setup
				     routine is non-null and if setup is used  */
  booleantype kin_constraintsSet; /* flag indicating if constraints are being
				     used                                      */
  booleantype kin_jacCurrent;     /* flag indicating if the Jacobian info. 
				     used by the linear solver is current      */
  booleantype kin_callForcingTerm; /* flag set if using either KIN_ETACHOICE1
				      or KIN_ETACHOICE2                        */
  booleantype kin_noResMon;         /* flag indicating if the nonlinear
				       residual monitoring scheme should be
				       used                                    */
  booleantype kin_retry_nni;        /* flag indicating if nonlinear iteration
				       should be retried (set by residual
				       monitoring algorithm)                   */
  booleantype kin_update_fnorm_sub; /* flag indicating if the fnorm associated
				       with the subinterval needs to be
				       updated (set by residual monitoring
				       algorithm)                              */

  realtype kin_mxnewtstep;     /* maximum allowable scaled step length         */
  realtype kin_sqrt_relfunc;   /* relative error bound for func(u)             */
  realtype kin_stepl;          /* scaled length of current step                */
  realtype kin_stepmul;        /* step scaling factor                          */
  realtype kin_eps;            /* current value of eps                         */
  realtype kin_eta;            /* current value of eta                         */
  realtype kin_eta_gamma;      /* gamma value used in eta calculation
				  (choice #2)                                  */
  realtype kin_eta_alpha;      /* alpha value used in eta calculation
			          (choice #2)                                  */
  booleantype kin_noInitSetup; /* flag controlling whether or not the KINSol
				  routine makes an initial call to the
				  linear solver setup routine (lsetup)         */
  realtype kin_sthrsh;         /* threshold value for calling the linear   
                                  solver setup routine                         */

  /* counters */

  long int kin_nni;            /* number of nonlinear iterations               */
  long int kin_nfe;            /* number of calls made to func routine         */
  long int kin_nnilset;        /* value of nni counter when the linear solver
				  setup was last called                        */
  long int kin_nnilset_sub;    /* value of nni counter when the linear solver
				  setup was last called (subinterval)          */
  long int kin_nbcf;           /* number of times the beta-condition could not 
                                  be met in KINLineSearch                      */
  long int kin_nbktrk;         /* number of backtracks performed by
		         	  KINLineSearch                                */
  long int kin_ncscmx;         /* number of consecutive steps of size
                                  mxnewtstep taken                             */

  /* vectors */

  N_Vector kin_uu;          /* solution vector/current iterate (initially
			       contains initial guess, but holds approximate
			       solution upon completion if no errors occurred) */
  N_Vector kin_unew;        /* next iterate (unew = uu+pp)                     */
  N_Vector kin_fval;        /* vector containing result of nonlinear system
			       function evaluated at a given iterate
			       (fval = func(uu))                               */
  N_Vector kin_uscale;      /* iterate scaling vector                          */
  N_Vector kin_fscale;      /* fval scaling vector                             */
  N_Vector kin_pp;          /* incremental change vector (pp = unew-uu)        */
  N_Vector kin_constraints; /* constraints vector                              */ 
  N_Vector kin_vtemp1;      /* scratch vector #1                               */
  N_Vector kin_vtemp2;      /* scratch vector #2                               */

  /* space requirements for vector storage */ 

  long int kin_lrw1;        /* number of realtype-sized memory blocks needed
			       for a single N_Vector                           */ 
  long int kin_liw1;        /* number of int-sized memory blocks needed for
			       a single N_Vecotr                               */ 
  long int kin_lrw;         /* total number of realtype-sized memory blocks
			       needed for all KINSOL work vectors              */
  long int kin_liw;         /* total number of int-sized memory blocks needed
			       for all KINSOL work vectors                     */

  /* linear solver data */
 
  /* function prototypes (pointers) */

  int (*kin_linit)(struct KINMemRec *kin_mem);

  int (*kin_lsetup)(struct KINMemRec *kin_mem);

  int (*kin_lsolve)(struct KINMemRec *kin_mem, N_Vector xx, N_Vector bb, 
                    realtype *res_norm );

  int (*kin_lfree)(struct KINMemRec *kin_mem);

  booleantype kin_inexact_ls; /* flag set by the linear solver module
				 (in linit) indicating whether this is an
				 iterative linear solver (TRUE), or a direct
				 linear solver (FALSE)                         */

  void *kin_lmem;         /* pointer to linear solver memory block             */

  realtype kin_fnorm;     /* value of L2-norm of fscale*fval                   */
  realtype kin_f1norm;    /* f1norm = 0.5*(fnorm)^2                            */
  realtype kin_res_norm;  /* value of L2-norm of residual (set by linear
			    solver)                                            */
  realtype kin_sfdotJp;   /* value of scaled func(u) vector (fscale*fval)
			    dotted with scaled J(u)*pp vector                  */
  realtype kin_sJpnorm;   /* value of L2-norm of fscale*(J(u)*pp)              */
  realtype kin_fnorm_sub; /* value of L2-norm of fscale*fval (subinterval)     */
  realtype kin_omega_min; /* lower bound on real scalar used in test to
			     determine if reduction of norm of nonlinear
			     residual is sufficient                            */
  realtype kin_omega_max; /* upper bound on real scalar used in test to
			     determine if reduction of norm of nonlinear
			     residual is sufficient                            */
  
/*
 * -----------------------------------------------------------------
 * Note: The KINLineSearch subroutine scales the values of the
 * variables sfdotJp and sJpnorm by a factor rl (lambda) that is
 * chosen by the line search algorithm such that the sclaed Newton
 * step satisfies the following conditions:
 *
 *  F(u_k+1) <= F(u_k) + alpha*(F(u_k)^T * J(u_k))*p*rl
 *
 *  F(u_k+1) >= F(u_k) + beta*(F(u_k)^T * J(u_k))*p*rl
 *
 * where alpha = 1.0e-4, beta = 0.9, u_k+1 = u_k + rl*p,
 * 0 < rl <= 1, J denotes the system Jacobian, and F represents
 * the nonliner system function.
 * -----------------------------------------------------------------
 */

  booleantype kin_MallocDone; /* flag indicating if KINMalloc has been
				 called yet                                    */

  /* message files */
  
  FILE *kin_errfp;  /* where KINSol error/warning messages are sent            */
  FILE *kin_infofp; /* where KINSol info messages are sent                     */

} *KINMem;

/*
 * -----------------------------------------------------------------
 * KINSOL error messages
 * -----------------------------------------------------------------
 */

/* KINCreate error messages */

#define MSG_KINMEM_FAIL "KINCreate-- allocation of kin_mem failed.\n\n "

/* KINSet* error messages */

#define MSG_KINS_NO_MEM     "KINSet*-- kin_mem = NULL illegal.\n\n"
#define MSG_BAD_PRINTFL     "KINSetPrintLevel-- illegal value for printfl.\n\n"
#define MSG_BAD_MXITER      "KINSetNumMaxIters-- illegal value for mxiter.\n\n"
#define MSG_BAD_MSBSET      "KINSetMaxSetupCalls-- illegal msbset < 0.\n\n"
#define MSG_BAD_MSBSETSUB   "KINSetMaxSubSetupCalls-- illegal msbsetsub < 0.\n\n"
#define MSG_BAD_ETACHOICE   "KINSetEtaForm-- illegal value for etachoice.\n\n"
#define MSG_BAD_ETACONST    "KINSetEtaConstValue-- eta out of range.\n\n"
#define MSG_BAD_GAMMA       "KINSetEtaParams-- gamma out of range.\n\n"
#define MSG_BAD_ALPHA       "KINSetEtaParams-- alpha out of range.\n\n"
#define MSG_BAD_MXNEWTSTEP  "KINSetMaxNewtonStep-- mxnewtstep < 0 illegal.\n\n"
#define MSG_BAD_RELFUNC     "KINSetRelErrFunc-- relfunc < 0 illegal.\n\n"
#define MSG_BAD_FNORMTOL    "KINSetFuncNormTol-- fnormtol < 0 illegal.\n\n"
#define MSG_BAD_SCSTEPTOL   "KINSetScaledStepTol-- scsteptol < 0 illegal.\n\n"
#define MSG_BAD_MXNBCF      "KINSetMaxBetaFails-- mxbcf < 0 illegal.\n\n"
#define MSG_BAD_CONSTRAINTS "KINSetConstraints-- illegal values in constraints vector.\n\n"
#define MSG_BAD_OMEGA       "KINSetResMonParams-- scalars < 0 illegal.\n\n"

/* KINMalloc error messages */

#define MSG_KINM_NO_MEM    "KINMalloc-- kin_mem = NULL illegal.\n\n"
#define MSG_MEM_FAIL       "KINMalloc-- a memory request failed.\n\n"
#define MSG_FUNC_NULL      "KINMalloc-- func = NULL illegal.\n\n"
#define MSG_BAD_NVECTOR    "KINMalloc-- a required vector operation is not implemented.\n\n"
#define MSG_KINS_FUNC_NULL "KINSetSysFunc-- func = NULL illegal.\n\n"

/* KINSol error messages */

#define MSG_KINSOL_NO_MEM    "KINSol-- kinsol_mem = NULL illegal.\n\n"
#define MSG_KINSOL_NO_MALLOC "KINSol-- attempt to call before KINMalloc illegal.\n\n"

/* KINSolInit error messages */

#define KINSI                  "KINSolInit--"
#define MSG_LSOLV_NO_MEM       KINSI "the linear solver memory pointer is NULL.\n\n"
#define MSG_UU_NULL            KINSI "uu = NULL illegal.\n\n"
#define MSG_BAD_GLSTRAT        KINSI "illegal value for globalstrategy.\n"
#define MSG_BAD_USCALE         KINSI "uscale = NULL illegal.\n\n"
#define MSG_USCALE_NONPOSITIVE KINSI "uscale has nonpositive elements.\n\n"
#define MSG_BAD_FSCALE         KINSI "fscale = NULL illegal.\n\n"
#define MSG_FSCALE_NONPOSITIVE KINSI "fscale has nonpositive elements.\n\n"
#define MSG_INITIAL_CNSTRNT    KINSI "initial guess does NOT meet constraints.\n\n"
#define MSG_LINIT_FAIL         KINSI "the linear solver's init routine failed.\n\n"

/* KINGet* error messages */

#define MSG_KING_NO_MEM "KINGet*-- kin_mem = NULL illegal.\n\n"

#ifdef __cplusplus
}
#endif

#endif
