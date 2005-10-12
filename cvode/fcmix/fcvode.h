/*
 * -----------------------------------------------------------------
 * $Revision: 1.54 $
 * $Date: 2005-10-12 21:12:12 $
 * ----------------------------------------------------------------- 
 * Programmer(s): Alan C. Hindmarsh, Radu Serban and
 *                Aaron Collier @ LLNL
 * -----------------------------------------------------------------
 * Copyright (c) 2002, The Regents of the University of California.
 * Produced at the Lawrence Livermore National Laboratory.
 * All rights reserved.
 * For details, see sundials/cvode/LICENSE.
 * -----------------------------------------------------------------
 * This is the header file for FCVODE, the Fortran interface to
 * the CVODE package.                                            
 * -----------------------------------------------------------------
 */

/*
 * =============================================================================
 *
 *                FCVODE Interface Package
 *
 * The FCVODE Interface Package is a package of C functions which support
 * the use of the CVODE solver, for the solution of ODE systems 
 * dy/dt = f(t,y), in a mixed Fortran/C setting.  While CVODE is written
 * in C, it is assumed here that the user's calling program and
 * user-supplied problem-defining routines are written in Fortran. 
 * This package provides the necessary interface to CVODE for both the
 * serial and the parallel NVECTOR implementations.
 * 
 * The user-callable functions, with the corresponding CVODE functions,
 * are as follows:
 * 
 *   FNVINITS and FNVINITP interface to N_VNew_Serial and
 *               N_VNew_Parallel, respectively
 * 
 *   FCVMALLOC  interfaces to CVodeCreate, CVodeSetFdata, and CVodeMalloc
 * 
 *   FCVREINIT  interfaces to CVReInit
 * 
 *   FCVSETIIN and FCVSETRIN interface to CVodeSet*
 *
 *   FCVEWTSET  interfaces to CVodeSetEwtFn
 * 
 *   FCVDIAG    interfaces to CVDiag
 * 
 *   FCVDENSE   interfaces to CVDense
 *   FCVDENSESETJAC   interfaces to CVDenseSetJacFn
 * 
 *   FCVBAND    interfaces to CVBand
 *   FCVBANDSETJAC    interfaces to CVBandSetJacFn
 *
 *   FCVSPGMR and FCVSPGMRREINIT interface to CVSpgmr and CVSpgmrSet*
 *   FCVSPGMRSETJAC   interfaces to CVSpgmrSetJacTimesVecFn
 *   FCVSPGMRSETPREC  interfaces to CVSpgmrSetPreconditioner
 * 
 *   FCVSPBCG, FCVSPBCGREINIT interface to CVSpbcg and CVSpbcgSet*
 *   FCVSPBCGSETJAC   interfaces to CVSpbcgSetJacTimesVecFn
 *   FCVSPBCGSETPREC  interfaces to CVSpbcgSetPreconditioner
 *
 *   FCVSPTFQMR, FCVSPTFQMRREINIT interface to CVSptfqmr and CVSptfqmrSet*
 *   FCVSPTFQMRSETJAC   interfaces to CVSptfqmrSetJacTimesVecFn
 *   FCVSPTFQMRSETPREC  interfaces to CVSptfqmrSetPreconditioner
 *
 *   FCVODE     interfaces to CVode, CVodeGet*, and CV*Get*
 * 
 *   FCVDKY     interfaces to CVodeGetDky
 * 
 *   FCVGETERRWEIGHTS  interfaces to CVodeGetErrWeights
 *
 *   FCVGETESTLOCALERR  interfaces to CVodeGetEstLocalErrors
 *
 *   FCVFREE    interfaces to CVodeFree
 * 
 * The user-supplied functions, each listed with the corresponding interface
 * function which calls it (and its type within CVODE), are as follows:
 *   FCVFUN    is called by the interface function FCVf of type CVRhsFn
 *   FCVDJAC   is called by the interface fn. FCVDenseJac of type CVDenseJacFn
 *   FCVBJAC   is called by the interface fn. FCVBandJac of type CVBandJacFn
 *   FCVPSOL   is called by the interface fn. FCVPSol of type CVSpilsPrecSolveFn
 *   FCVPSET   is called by the interface fn. FCVPSet of type CVSpilsPrecSetupFn
 *   FCVJTIMES is called by interface fn. FCVJtimes of type CVSpilsJacTimesVecFn
 *   FCVEWT    is called by interface fn. FCVEwtSet of type CVEwtFn
 * In contrast to the case of direct use of CVODE, and of most Fortran ODE
 * solvers, the names of all user-supplied routines here are fixed, in
 * order to maximize portability for the resulting mixed-language program.
 * 
 * Important note on portability.
 * In this package, the names of the interface functions, and the names of
 * the Fortran user routines called by them, appear as dummy names
 * which are mapped to actual values by a series of definitions, in this
 * and other header files.
 * 
 * =============================================================================
 * 
 *                  Usage of the FCVODE Interface Package
 * 
 * The usage of FCVODE requires calls to five or more interface
 * functions, depending on the method options selected, and one or more
 * user-supplied routines which define the problem to be solved.  These
 * function calls and user routines are summarized separately below.
 * 
 * Some details are omitted, and the user is referred to the user documents
 * on CVODE for more complete documentation.  Information on the
 * arguments of any given user-callable interface routine, or of a given
 * user-supplied function called by an interface function, can be found in
 * the documentation on the corresponding function in the CVODE package.
 * 
 * The number labels on the instructions below end with s for instructions
 * that apply to the serial version of CVODE only, and end with p for
 * those that apply to the parallel version only.
 *
 * (1) User-supplied right-hand side routine: FCVFUN
 * The user must in all cases supply the following Fortran routine
 *       SUBROUTINE FCVFUN (T, Y, YDOT, IPAR, RPAR)
 *       DIMENSION Y(*), YDOT(*), IPAR(*), RPAR(*)
 * It must set the YDOT array to f(t,y), the right-hand side of the ODE 
 * system, as function of T = t and the array Y = y.  Here Y and YDOT
 * are distributed vectors. IPAR and RPAR are arrays of integer and real user 
 * data, respectively as passed to FCVMALLOC.
 * 
 * (2s) Optional user-supplied dense Jacobian approximation routine: FCVDJAC
 * As an option when using the DENSE linear solver, the user may supply a
 * routine that computes a dense approximation of the system Jacobian 
 * J = df/dy. If supplied, it must have the following form:
 *       SUBROUTINE FCVDJAC (NEQ, T, Y, FY, DJAC, H, IPAR, RPAR, WK1, WK2, WK3)
 *       DIMENSION Y(*), FY(*), DJAC(NEQ,*), IPAR(*), RPAR(*), WK1(*), WK2(*), WK3(*)
 * Typically this routine will use only NEQ, T, Y, and DJAC. It must compute
 * the Jacobian and store it columnwise in DJAC.
 * IPAR and RPAR are user (integer and real) arrays passed to FCVMALLOC.
 * 
 * (3s) Optional user-supplied band Jacobian approximation routine: FCVBJAC
 * As an option when using the BAND linear solver, the user may supply a
 * routine that computes a band approximation of the system Jacobian 
 * J = df/dy. If supplied, it must have the following form:
 *       SUBROUTINE FCVBJAC (NEQ, MU, ML, MDIM, T, Y, FY, BJAC, H,
 *      1                    IPAR, RPAR, WK1, WK2, WK3)
 *       DIMENSION Y(*), FY(*), BJAC(MDIM,*), IPAR(*), RPAR(*), WK1(*), WK2(*), WK3(*)
 * Typically this routine will use only NEQ, MU, ML, T, Y, and BJAC. 
 * It must load the MDIM by N array BJAC with the Jacobian matrix at the
 * current (t,y) in band form.  Store in BJAC(k,j) the Jacobian element J(i,j)
 * with k = i - j + MU + 1 (k = 1 ... ML+MU+1) and j = 1 ... N.
 * IPAR and RPAR are user (integer and real) arrays passed to FCVMALLOC.
 * 
 * (4) Optional user-supplied Jacobian-vector product routine: FCVJTIMES
 * As an option when using the SP* linear solver, the user may supply
 * a routine that computes the product of the system Jacobian J = df/dy and 
 * a given vector v.  If supplied, it must have the following form:
 *       SUBROUTINE FCVJTIMES (V, FJV, T, Y, FY, H, IPAR, RPAR, WORK, IER)
 *       DIMENSION V(*), FJV(*), Y(*), FY(*), IPAR(*), RPAR(*), WORK(*)
 * Typically this routine will use only NEQ, T, Y, V, and FJV.  It must
 * compute the product vector Jv where the vector v is stored in V, and store
 * the product in FJV.  On return, set IER = 0 if FCVJTIMES was successful,
 * and nonzero otherwise.
 * IPAR and RPAR are user (integer and real) arrays passed to FCVMALLOC.
 * 
 * (5) Optional user-supplied error weight vector routine: FCVEWT
 * As an option to providing the relative and absolute tolerances, the user
 * may supply a routine that computes the weights used in the WRMS norms.
 * If supplied, it must have the following form:
 *       SUBROUTINE FCVEWT (Y, EWT, IPAR, RPAR, IER)
 *       DIMENSION Y(*), EWT(*), IPAR(*), RPAR(*)
 * It must store the error weights in EWT, given the current solution vector Y.
 * On return, set IER = 0 if successful, and nonzero otherwise.
 * IPAR and RPAR are user (integer and real) arrays passed to FCVMALLOC.
 *
 * (6) Initialization:  FNVINITS / FNVINITP , FCVMALLOC, FCVREINIT
 * 
 * (6.1s) To initialize the serial machine environment, the user must make
 * the following call:
 *        CALL FNVINITS (1, NEQ, IER)
 * where the first argument is the CVODE solver ID. The other arguments are:
 * NEQ     = size of vectors
 * IER     = return completion flag. Values are 0 = success, -1 = failure.
 * 
 * (6.1p) To initialize the parallel machine environment, the user must make 
 * the following call:
 *        CALL FNVINITP (1, NLOCAL, NGLOBAL, IER)
 * The arguments are:
 * NLOCAL  = local size of vectors on this processor
 * NGLOBAL = the system size, and the global size of vectors (the sum 
 *           of all values of NLOCAL)
 * IER     = return completion flag. Values are 0 = success, -1 = failure.
 * Note: If MPI was initialized by the user, the communicator must be
 * set to MPI_COMM_WORLD.  If not, this routine initializes MPI and sets
 * the communicator equal to MPI_COMM_WORLD.
 * 
 * (6.2) To set various problem and solution parameters and allocate
 * internal memory, make the following call:
 *       CALL FCVMALLOC(T0, Y0, METH, ITMETH, IATOL, RTOL, ATOL,
 *      1               IOUT, ROUT, IPAR, RPAR, IER)
 * The arguments are:
 * T0     = initial value of t
 * Y0     = array of initial conditions
 * METH   = basic integration method: 1 = Adams (nonstiff), 2 = BDF (stiff)
 * ITMETH = nonlinear iteration method: 1=functional iteration, 2=Newton iter.
 * IATOL  = type for absolute tolerance ATOL: 1 = scalar, 2 = array.
 *          If IATOL = 3, then the user must supply a routine FCVEWT to compute
 *          the error weight vector.
 * RTOL   = relative tolerance (scalar)
 * ATOL   = absolute tolerance (scalar or array)
 * IOUT   = array of length 21 for integer optional outputs
 *          (declare as INTEGER*4 or INTEGER*8 according to C type long int)
 * ROUT   = array of length 6 for real optional outputs
 * IPAR   = array with user integer data
 *          (declare as INTEGER*4 or INTEGER*8 according to C type long int)
 * RPAR   = array with user real data
 * IER    = return completion flag.  Values are 0 = SUCCESS, and -1 = failure.
 *          See printed message for details in case of failure.
 *
 * The user data arrays IPAR and RPAR are passed unmodified to all subsequent
 * calls to user-provided routines. Modifications to either array inside a
 * user-provided routine will be propagated. Using these two arrays, the user
 * can dispense with Common blocks to pass data betwen user-provided routines.
 * 
 * The optional outputs are:
 *           LENRW   = IOUT( 1) from CVodeGetWorkSpace
 *           LENIW   = IOUT( 2) from CVodeGetWorkSpace
 *           NST     = IOUT( 3) from CVodeGetNumSteps
 *           NFE     = IOUT( 4) from CVodeGetNumRhsEvals
 *           NETF    = IOUT( 5) from CVodeGetNumErrTestFails
 *           NCFN    = IOUT( 6) from CVodeGetNumNonlinSolvConvFails
 *           NNI     = IOUT( 7) from CVodeGetNumNonlinSolvIters
 *           NSETUPS = IOUT( 8) from CVodeGetNumLinSolvSetups
 *           QU      = IOUT( 9) from CVodeGetLastOrder
 *           QCUR    = IOUT(10) from CVodeGetCurrentOrder
 *           NOR     = IOUT(11) from CVodeGetNumStabLimOrderReds
 *           NGE     = IOUT(12) from CVodeGetNumGEvals
 *
 *           H0U     = ROUT( 1) from CVodeGetActualInitStep
 *           HU      = ROUT( 2) from CVodeGetLastStep
 *           HCUR    = ROUT( 3) from CVodeGetCurrentStep
 *           TCUR    = ROUT( 4) from CVodeGetCurrentTime
 *           TOLSF   = ROUT( 5) from CVodeGetTolScaleFactor
 *           UROUND  = ROUT( 6) from UNIT_ROUNDOFF
 * See the CVODE manual for details. 
 *
 * If the user program includes the FCVEWT routine for the evaluation of the 
 * error weights, the following call must be made
 *       CALL FCVEWTSET(FLAG, IER)
 * with FLAG = 1 to specify that FCVEWT is provided.
 * The return flag IER is 0 if successful, and nonzero otherwise.
 *
 * (6.3) To re-initialize the CVODE solver for the solution of a new problem
 * of the same size as one already solved, make the following call:
 *       CALL FCVREINIT(T0, Y0, IATOL, RTOL, ATOL, IER)
 * The arguments have the same names and meanings as those of FCVMALLOC,
 * except that METH and ITMETH  have been omitted from the argument list 
 * (being unchanged for the new problem).  
 * FCVREINIT performs the same initializations as FCVMALLOC, but does no memory 
 * allocation, using instead the existing internal memory created by the
 * previous  FCVMALLOC call.  The call to specify the linear system solution
 * method may or may not be needed; see paragraph (7) below.
 * 
 * (6.4) To set various integer optional inputs, make the folowing call:
 *       CALL FCVSETIIN(KEY, VALUE, IER)
 * to set the integer value VAL to the optional input specified by the
 * quoted character string KEY.
 * KEY is one of the following: MAX_ORD, MAX_NSTEPS, MAX_ERRFAIL, MAX_NITERS, 
 * MAX_CONVFAIL, HNIL_WARNS, STAB_LIM.
 *
 * To set various real optional inputs, make the folowing call:
 *       CALL FCVSETRIN(KEY, VALUE, IER)
 * to set the real value VAL to the optional input specified by the
 * quoted character string KEY.
 * KEY is one of the following: INIT_STEP, MAX_STEP, MIN_STEP, STOP_TIME,
 * NLCONV_COEF.
 *
 * FCVSETIIN and FCVSETRIN return IER = 0 if successful and IER < 0 if an 
 * error occured.
 *
 * (7) Specification of linear system solution method.
 * In the case of a stiff system, the implicit BDF method involves the solution
 * of linear systems related to the Jacobian J = df/dy of the ODE system.
 * CVODE presently includes four choices for the treatment of these systems,
 * and the user of FCVODE must call a routine with a specific name to make the
 * desired choice.
 * 
 * (7.1) Diagonal approximate Jacobian.
 * This choice is appropriate when the Jacobian can be well approximated by
 * a diagonal matrix.  The user must make the call:
 *       CALL FCVDIAG(IER)
 * IER is an error return flag: 0 = success, negative value = error.
 * There is no additional user-supplied routine.  
 *
 * Optional outputs specific to the DIAG case are:
 *        LENRWD = IOUT(13) from CVDiagGetWorkSpace
 *        LENIWD = IOUT(14) from CVDiagGetWorkSpace
 *        LSTF   = IOUT(15) from CVDiagGetLastFlag
 *        NFEDQ  = IOUT(16) from CVDiagGetNumRhsEvals
 * See the CVODE manual for descriptions.
 * 
 * (7.2s) DENSE treatment of the linear system.
 * The user must make the call
 *       CALL FCVDENSE(NEQ, IER)
 * The argument is:
 * IER = error return flag: 0 = success , negative value = an error occured
 * 
 * If the user program includes the FCVDJAC routine for the evaluation of the 
 * dense approximation to the Jacobian, the following call must be made
 *       CALL FCVDENSESETJAC(FLAG, IER)
 * with FLAG = 1 to specify that FCVDJAC is provided.  (FLAG = 0 specifies
 * using the internal finite differences approximation to the Jacobian.)
 * The return flag IER is 0 if successful, and nonzero otherwise.
 * 
 * Optional outputs specific to the DENSE case are:
 *        LENRWD = IOUT(13) from CVDenseGetWorkSpace
 *        LENIWD = IOUT(14) from CVDenseGetWorkSpace
 *        LSTF   = IOUT(15) from CVDenseGetLastFlag
 *        NFEDQ  = IOUT(16) from CVDenseGetNumRhsEvals
 *        NJED   = IOUT(17) from CVDenseGetNumJacEvals
 * See the CVODE manual for descriptions.
 * 
 * (7.3s) BAND treatment of the linear system
 * The user must make the call
 *       CALL FCVBAND(NEQ, MU, ML, IER)
 * The arguments are:
 * MU  = upper bandwidth
 * ML  = lower bandwidth
 * IER = error return flag: 0 = success , negative value = an error occured
 * 
 * If the user program includes the FCVBJAC routine for the evaluation of the 
 * band approximation to the Jacobian, the following call must be made
 *       CALL FCVBANDSETJAC(FLAG, IER)
 * with FLAG = 1 to specify that FCVBJAC is provided.  (FLAG = 0 specifies
 * using the internal finite differences approximation to the Jacobian.)
 * The return flag IER is 0 if successful, and nonzero otherwise.
 * 
 * Optional outputs specific to the BAND case are:
 *        LENRWB = IOUT(13) from CVBandGetWorkSpace
 *        LENIWB = IOUT(14) from CVBandGetWorkSpace
 *        LSTF   = IOUT(15) from CVBandGetLastFlag
 *        NFEDQ  = IOUT(16) from CVBandGetNumRhsEvals
 *        NJEB   = IOUT(17) from CVBandGetNumJacEvals
  * See the CVODE manual for descriptions.
 *
 * (7.4) SPGMR treatment of the linear systems.
 * For the Scaled Preconditioned GMRES solution of the linear systems,
 * the user must make the following call:
 *       CALL FCVSPGMR(IPRETYPE, IGSTYPE, MAXL, DELT, IER)              
 * The arguments are:
 * IPRETYPE = preconditioner type: 
 *              0 = none 
 *              1 = left only
 *              2 = right only
 *              3 = both sides
 * IGSTYPE  = Gram-schmidt process type: 
 *              1 = modified G-S
 *              2 = classical G-S.
 * MAXL     = maximum Krylov subspace dimension; 0 indicates default.
 * DELT     = linear convergence tolerance factor; 0.0 indicates default.
 * IER      = error return flag: 0 = success; negative value = an error occured
 * 
 * If the user program includes the FCVJTIMES routine for the evaluation of the 
 * Jacobian vector product, the following call must be made
 *       CALL FCVSPGMRSETJAC(FLAG, IER)
 * with FLAG = 1 to specify that FCVJTIMES is provided.  (FLAG = 0 specifies
 * using and internal finite difference approximation to this product.)
 * The return flag IER is 0 if successful, and nonzero otherwise.
 * 
 * Usage of the user-supplied routines FCVPSOL and FCVPSET for solution of the 
 * preconditioner linear system requires the following call:
 *       CALL FCVSPGMRSETPREC(FLAG, IER)
 * with FLAG = 1. The return flag IER is 0 if successful, nonzero otherwise.
 * The user-supplied routine FCVPSOL must have the form:
 *       SUBROUTINE FCVPSOL (T,Y,FY,R,Z,GAMMA,DELTA,LR,IPAR,RPAR,VT,IER)
 *       DIMENSION Y(*), FY(*), VT(*), R(*), Z(*), IPAR(*), RPAR(*)
 * Typically this routine will use only NEQ, T, Y, GAMMA, R, LR, and Z.  It
 * must solve the preconditioner linear system Pz = r, where r = R is input, 
 * and store the solution z in Z.  Here P is the left preconditioner if LR = 1
 * and the right preconditioner if LR = 2.  The preconditioner (or the product
 * of the left and right preconditioners if both are nontrivial) should be an 
 * approximation to the matrix I - GAMMA*J (I = identity, J = Jacobian).
 * IPAR and RPAR are user (integer and real) arrays passed to FCVMALLOC.
 *
 * The user-supplied routine FCVPSET must be of the form:
 *       SUBROUTINE FCVPSET(T,Y,FY,JOK,JCUR,GAMMA,H,IPAR,RPAR,V1,V2,V3,IER)
 *       DIMENSION Y(*), FY(*), V1(*), V2(*), V3(*), IPAR(*), RPAR(*)
 * Typically this routine will use only NEQ, T, Y, JOK, and GAMMA. It must
 * perform any evaluation of Jacobian-related data and preprocessing needed
 * for the solution of the preconditioner linear systems by FCVPSOL.
 * The JOK argument allows for Jacobian data to be saved and reused:  If 
 * JOK = 0, this data should be recomputed from scratch.  If JOK = 1, a saved
 * copy of it may be reused, and the preconditioner constructed from it.
 * On return, set JCUR = 1 if Jacobian data was computed, and 0 otherwise.
 * Also on return, set IER = 0 if FCVPSET was successful, set IER positive if a 
 * recoverable error occurred, and set IER negative if a non-recoverable error
 * occurred.
 * IPAR and RPAR are user (integer and real) arrays passed to FCVMALLOC.
 * 
 * Optional outputs specific to the SPGMR case are:
 *        LENRWG = IOUT(13) from CVSpgmrGetWorkSpace
 *        LENIWG = IOUT(14) from CVSpgmrGetWorkSpace
 *        LSTF   = IOUT(15) from CVSpgmrGetLastFlag
 *        NFEDQ  = IOUT(16) from CVSpgmrGetRhsEvals
 *        NJTV   = IOUT(17) from CVSpgmrGetJtimesEvals
 *        NPE    = IOUT(18) from CVSpgmrGetPrecEvals
 *        NPS    = IOUT(19) from CVSpgmrGetPrecSolves
 *        NLI    = IOUT(20) from CVSpgmrGetLinIters
 *        NCFL   = IOUT(21) from CVSpgmrGetConvFails
 * See the CVODE manual for descriptions.
 * 
 * If a sequence of problems of the same size is being solved using the
 * SPGMR linear solver, then following the call to FCVREINIT, a call to the
 * FCVSPGMRREINIT routine is needed if any of IPRETYPE, IGSTYPE, DELT is
 * being changed.  In that case, call FCVSPGMRREINIT as follows:
 *       CALL FCVSPGMRREINIT(IPRETYPE, IGSTYPE, DELT, IER)              
 * The arguments have the same meanings as for FCVSPGMR.  If MAXL is being
 * changed, then call FCVSPGMR instead.
 * 
 * (7.5) SPBCG treatment of the linear systems.
 * For the Scaled Preconditioned Bi-CGSTAB solution of the linear systems,
 * the user must make the following call:
 *       CALL FCVSPBCG(IPRETYPE, MAXL, DELT, IER)              
 * The arguments are:
 * IPRETYPE = preconditioner type: 
 *              0 = none 
 *              1 = left only
 *              2 = right only
 *              3 = both sides
 * MAXL     = maximum Krylov subspace dimension; 0 indicates default.
 * DELT     = linear convergence tolerance factor; 0.0 indicates default.
 * IER      = error return flag: 0 = success; negative value = an error occured
 * 
 * If the user program includes the FCVJTIMES routine for the evaluation of the 
 * Jacobian vector product, the following call must be made
 *       CALL FCVSPBCGSETJAC(FLAG, IER)
 * with FLAG = 1 to specify that FCVJTIMES is provided.  (FLAG = 0 specifies
 * using and internal finite difference approximation to this product.)
 * The return flag IER is 0 if successful, and nonzero otherwise.
 * 
 * Usage of the user-supplied routines FCVPSOL and FCVPSET for solution of the 
 * preconditioner linear system requires the following call:
 *       CALL FCVSPBCGSETPREC(FLAG, IER)
 * with FLAG = 1. The return flag IER is 0 if successful, nonzero otherwise.
 * The user-supplied routine FCVPSOL must have the form:
 *       SUBROUTINE FCVPSOL (T,Y,FY,R,Z,GAMMA,DELTA,LR,IPAR,RPAR,VT,IER)
 *       DIMENSION Y(*), FY(*), VT(*), R(*), Z(*), IPAR(*), RPAR(*)
 * Typically this routine will use only NEQ, T, Y, GAMMA, R, LR, and Z.  It
 * must solve the preconditioner linear system Pz = r, where r = R is input, 
 * and store the solution z in Z.  Here P is the left preconditioner if LR = 1
 * and the right preconditioner if LR = 2.  The preconditioner (or the product
 * of the left and right preconditioners if both are nontrivial) should be an 
 * approximation to the matrix I - GAMMA*J (I = identity, J = Jacobian).
 * IPAR and RPAR are user (integer and real) arrays passed to FCVMALLOC.
 * 
 * The user-supplied routine FCVPSET must be of the form:
 *       SUBROUTINE FCVPSET(T,Y,FY,JOK,JCUR,GAMMA,H,IPAR,RPAR,V1,V2,V3,IER)
 *       DIMENSION Y(*), FY(*), V1(*), V2(*), V3(*), IPAR(*), RPAR(*) 
 * Typically this routine will use only NEQ, T, Y, JOK, and GAMMA. It must
 * perform any evaluation of Jacobian-related data and preprocessing needed
 * for the solution of the preconditioner linear systems by FCVPSOL.
 * The JOK argument allows for Jacobian data to be saved and reused:  If 
 * JOK = 0, this data should be recomputed from scratch.  If JOK = 1, a saved
 * copy of it may be reused, and the preconditioner constructed from it.
 * On return, set JCUR = 1 if Jacobian data was computed, and 0 otherwise.
 * Also on return, set IER = 0 if FCVPSET was successful, set IER positive if a 
 * recoverable error occurred, and set IER negative if a non-recoverable error
 * occurred.
 * IPAR and RPAR are user (integer and real) arrays passed to FCVMALLOC.
 * 
 * Optional outputs specific to the SPBCG case are:
 *        LENRWB = IOUT(13) from CVSpbcgGetWorkSpace
 *        LENIWB = IOUT(14) from CVSpbcgGetWorkSpace
 *        LSTF   = IOUT(15) from CVSpbcgGetLastFlag
 *        NFEDQ  = IOUT(16) from CVSpbcgGetRhsEvals
 *        NJTV   = IOUT(17) from CVSpbcgGetJtimesEvals
 *        NPE    = IOUT(18) from CVSpbcgGetPrecEvals
 *        NPS    = IOUT(19) from CVSpbcgGetPrecSolves
 *        NLI    = IOUT(20) from CVSpbcgGetLinIters
 *        NCFL   = IOUT(21) from CVSpbcgGetConvFails
  * See the CVODE manual for descriptions.
 * 
 * If a sequence of problems of the same size is being solved using the
 * SPBCG linear solver, then following the call to FCVREINIT, a call to the
 * FCVSPBCGREINIT routine is needed if any of its arguments is
 * being changed.  The call is:
 *       CALL FCVSPBCGREINIT(IPRETYPE, MAXL, DELT, IER)              
 * The arguments have the same meanings as for FCVSPBCG.
 *
 * (7.6) SPTFQMR treatment of the linear systems.
 * For the Scaled Preconditioned TFQMR solution of the linear systems,
 * the user must make the following call:
 *       CALL FCVSPTFQMR(IPRETYPE, MAXL, DELT, IER)              
 * The arguments are:
 * IPRETYPE = preconditioner type: 
 *              0 = none 
 *              1 = left only
 *              2 = right only
 *              3 = both sides
 * MAXL     = maximum Krylov subspace dimension; 0 indicates default.
 * DELT     = linear convergence tolerance factor; 0.0 indicates default.
 * IER      = error return flag: 0 = success; negative value = an error occured
 * 
 * If the user program includes the FCVJTIMES routine for the evaluation of the 
 * Jacobian vector product, the following call must be made
 *       CALL FCVSPTFQMRSETJAC(FLAG, IER)
 * with FLAG = 1 to specify that FCVJTIMES is provided.  (FLAG = 0 specifies
 * using and internal finite difference approximation to this product.)
 * The return flag IER is 0 if successful, and nonzero otherwise.
 * 
 * Usage of the user-supplied routines FCVPSOL and FCVPSET for solution of the 
 * preconditioner linear system requires the following call:
 *       CALL FCVSPTFQMRSETPREC(FLAG, IER)
 * with FLAG = 1. The return flag IER is 0 if successful, nonzero otherwise.
 * The user-supplied routine FCVPSOL must have the form:
 *       SUBROUTINE FCVPSOL (T,Y,FY,R,Z,GAMMA,DELTA,LR,IPAR,RPAR,VT,IER)
 *       DIMENSION Y(*), FY(*), VT(*), R(*), Z(*), IPAR(*), RPAR(*)
 * Typically this routine will use only NEQ, T, Y, GAMMA, R, LR, and Z.  It
 * must solve the preconditioner linear system Pz = r, where r = R is input, 
 * and store the solution z in Z.  Here P is the left preconditioner if LR = 1
 * and the right preconditioner if LR = 2.  The preconditioner (or the product
 * of the left and right preconditioners if both are nontrivial) should be an 
 * approximation to the matrix I - GAMMA*J (I = identity, J = Jacobian).
 * IPAR and RPAR are user (integer and real) arrays passed to FCVMALLOC.
 * 
 * The user-supplied routine FCVPSET must be of the form:
 *       SUBROUTINE FCVPSET(T,Y,FY,JOK,JCUR,GAMMA,H,IPAR,RPAR,V1,V2,V3,IER)
 *       DIMENSION Y(*), FY(*), V1(*), V2(*), V3(*), IPAR(*), RPAR(*)
 * Typically this routine will use only NEQ, T, Y, JOK, and GAMMA. It must
 * perform any evaluation of Jacobian-related data and preprocessing needed
 * for the solution of the preconditioner linear systems by FCVPSOL.
 * The JOK argument allows for Jacobian data to be saved and reused:  If 
 * JOK = 0, this data should be recomputed from scratch.  If JOK = 1, a saved
 * copy of it may be reused, and the preconditioner constructed from it.
 * On return, set JCUR = 1 if Jacobian data was computed, and 0 otherwise.
 * Also on return, set IER = 0 if FCVPSET was successful, set IER positive if a 
 * recoverable error occurred, and set IER negative if a non-recoverable error
 * occurred.
 * IPAR and RPAR are user (integer and real) arrays passed to FCVMALLOC.
 * 
 * Optional outputs specific to the SPTFQMR case are:
 *        LENRWQ = IOUT(13) from CVSptfqmrGetWorkSpace
 *        LENIWQ = IOUT(14) from CVSptfqmrGetWorkSpace
 *        LSTF   = IOUT(15) from CVSptfqmrGetLastFlag
 *        NFEDQ  = IOUT(16) from CVSptfqmrGetRhsEvals
 *        NJTV   = IOUT(17) from CVSptfqmrGetJtimesEvals
 *        NPE    = IOUT(18) from CVSptfqmrGetPrecEvals
 *        NPS    = IOUT(19) from CVSptfqmrGetPrecSolves
 *        NLI    = IOUT(20) from CVSptfqmrGetLinIters
 *        NCFL   = IOUT(21) from CVSptfqmrGetConvFails
 * See the CVODE manual for descriptions.
 *
 * If a sequence of problems of the same size is being solved using the
 * SPTFQMR linear solver, then following the call to FCVREINIT, a call to the
 * FCVSPTFQMRREINIT routine is needed if any of its arguments is
 * being changed.  The call is:
 *       CALL FCVSPTFQMRREINIT(IPRETYPE, MAXL, DELT, IER)              
 * The arguments have the same meanings as for FCVSPTFQMR.
 *
 * (8) The integrator: FCVODE
 * Carrying out the integration is accomplished by making calls as follows:
 *       CALL FCVODE (TOUT, T, Y, ITASK, IER)
 * The arguments are:
 * TOUT  = next value of t at which a solution is desired (input)
 * T     = value of t reached by the solver on output
 * Y     = array containing the computed solution on output
 * ITASK = task indicator: 1 = normal mode (overshoot TOUT and interpolate)
 *         2 = one-step mode (return after each internal step taken)
 *         3 = normal tstop mode (like 1, but integration never proceeds past 
 *             TSTOP, which must be specified through a call to FCVSETRIN
 *             using the key 'STOP_TIME')
 *         4 = one step tstop (like 2, but integration never goes past TSTOP)
 * IER   = completion flag: 0 = success, 1 = tstop return, 2 = root return, 
 *         values -1 ... -10 are various failure modes (see CVODE manual).
 * The current values of the optional outputs are available in IOUT and ROUT.
 * 
 * (9) Computing solution derivatives: FCVDKY
 * To obtain a derivative of the solution, of order up to the current method
 * order, make the following call:
 *       CALL FCVDKY (T, K, DKY, IER)
 * The arguments are:
 * T   = value of t at which solution derivative is desired, in [TCUR-HU,TCUR].
 * K   = derivative order (0 .le. K .le. QU)
 * DKY = array containing computed K-th derivative of y on return
 * IER = return flag: = 0 for success, < 0 for illegal argument.
 * 
 * (10) Memory freeing: FCVFREE 
 * To free the internal memory created by the calls to FCVMALLOC and
 * FNVINITS or FNVINITP, make the call
 *       CALL FCVFREE
 * 
 * =============================================================================
 */

#ifndef _FCVODE_H
#define _FCVODE_H

#ifdef __cplusplus  /* wrapper to enable C++ usage */
extern "C" {
#endif

/* header files  */

#include "band.h"           /* definition of BandMat       */
#include "cvode.h"          /* definition of type CVRhsFn  */
#include "dense.h"          /* definition of DenseMat      */
#include "nvector.h"        /* definition of type N_Vector */
#include "sundialstypes.h"  /* definition of type realtype */

/* Definitions of interface function names */

#if defined(F77_FUNC)

#define FCV_MALLOC         F77_FUNC(fcvmalloc, FCVMALLOC)
#define FCV_REINIT         F77_FUNC(fcvreinit, FCVREINIT)
#define FCV_SETIIN         F77_FUNC(fcvsetiin, FCVSETIIN)
#define FCV_SETRIN         F77_FUNC(fcvsetrin, FCVSETRIN)
#define FCV_EWTSET         F77_FUNC(fcvewtset, FCVEWTSET)
#define FCV_DIAG           F77_FUNC(fcvdiag, FCVDIAG)
#define FCV_DENSE          F77_FUNC(fcvdense, FCVDENSE)
#define FCV_DENSESETJAC    F77_FUNC(fcvdensesetjac, FCVDENSESETJAC)
#define FCV_BAND           F77_FUNC(fcvband, FCVBAND)
#define FCV_BANDSETJAC     F77_FUNC(fcvbandsetjac, FCVBANDSETJAC)
#define FCV_SPTFQMR        F77_FUNC(fcvsptfqmr, FCVSPTFQMR)
#define FCV_SPTFQMRREINIT  F77_FUNC(fcvsptfqmrreinit, FCVSPTFQMRREINIT)
#define FCV_SPTFQMRSETJAC  F77_FUNC(fcvsptfqmrsetjac, FCVSPTFQMRSETJAC)
#define FCV_SPTFQMRSETPREC F77_FUNC(fcvsptfqmrsetprec, FCVSPTFQMRSETPREC)
#define FCV_SPBCG          F77_FUNC(fcvspbcg, FCVSPBCG)
#define FCV_SPBCGREINIT    F77_FUNC(fcvspbcgreinit, FCVSPBCGREINIT)
#define FCV_SPBCGSETJAC    F77_FUNC(fcvspbcgsetjac, FCVSPBCGSETJAC)
#define FCV_SPBCGSETPREC   F77_FUNC(fcvspbcgsetprec, FCVSPBCGSETPREC)
#define FCV_SPGMR          F77_FUNC(fcvspgmr, FCVSPGMR)
#define FCV_SPGMRREINIT    F77_FUNC(fcvspgmrreinit, FCVSPGMRREINIT)
#define FCV_SPGMRSETJAC    F77_FUNC(fcvspgmrsetjac, FCVSPGMRSETJAC)
#define FCV_SPGMRSETPREC   F77_FUNC(fcvspgmrsetprec, FCVSPGMRSETPREC)
#define FCV_CVODE          F77_FUNC(fcvode, FCVODE)
#define FCV_DKY            F77_FUNC(fcvdky, FCVDKY)
#define FCV_FREE           F77_FUNC(fcvfree, FCVFREE)
#define FCV_FUN            F77_FUNC(fcvfun, FCVFUN)
#define FCV_DJAC           F77_FUNC(fcvdjac, FCVDJAC)
#define FCV_BJAC           F77_FUNC(fcvbjac, FCVBJAC)
#define FCV_PSOL           F77_FUNC(fcvpsol, FCVPSOL)
#define FCV_PSET           F77_FUNC(fcvpset, FCVPSET)
#define FCV_JTIMES         F77_FUNC(fcvjtimes, FCVJTIMES)
#define FCV_EWT            F77_FUNC(fcvewt, FCVEWT)
#define FCV_GETERRWEIGHTS  F77_FUNC(fcvgeterrweights, FCVGETERRWEIGHTS)
#define FCV_GETESTLOCALERR F77_FUNC(fcvgetestlocalerr, FCVGETESTLOCALERR)

#elif defined(SUNDIALS_UNDERSCORE_NONE) && defined(SUNDIALS_CASE_LOWER)

#define FCV_MALLOC         fcvmalloc
#define FCV_REINIT         fcvreinit
#define FCV_SETIIN         fcvsetiin
#define FCV_SETRIN         fcvsetrin
#define FCV_EWTSET         fcvewtset
#define FCV_DIAG           fcvdiag
#define FCV_DENSE          fcvdense
#define FCV_DENSESETJAC    fcvdensesetjac
#define FCV_BAND           fcvband
#define FCV_BANDSETJAC     fcvbandsetjac
#define FCV_SPTFQMR        fcvsptfqmr
#define FCV_SPTFQMRREINIT  fcvsptfqmrreinit
#define FCV_SPTFQMRSETJAC  fcvsptfqmrsetjac
#define FCV_SPTFQMRSETPREC fcvsptfqmrsetprec
#define FCV_SPBCG          fcvspbcg
#define FCV_SPBCGREINIT    fcvspbcgreinit
#define FCV_SPBCGSETJAC    fcvspbcgsetjac
#define FCV_SPBCGSETPREC   fcvspbcgsetprec
#define FCV_SPGMR          fcvspgmr
#define FCV_SPGMRREINIT    fcvspgmrreinit
#define FCV_SPGMRSETJAC    fcvspgmrsetjac
#define FCV_SPGMRSETPREC   fcvspgmrsetprec
#define FCV_CVODE          fcvode
#define FCV_DKY            fcvdky
#define FCV_FREE           fcvfree
#define FCV_FUN            fcvfun
#define FCV_DJAC           fcvdjac
#define FCV_BJAC           fcvbjac
#define FCV_PSOL           fcvpsol
#define FCV_PSET           fcvpset
#define FCV_JTIMES         fcvjtimes
#define FCV_EWT            fcvewt
#define FCV_GETERRWEIGHTS  fcvgeterrweights
#define FCV_GETESTLOCALERR fcvgetestlocalerr

#elif defined(SUNDIALS_UNDERSCORE_NONE) && defined(SUNDIALS_CASE_UPPER)

#define FCV_MALLOC         FCVMALLOC
#define FCV_REINIT         FCVREINIT
#define FCV_SETIIN         FCVSETIIN
#define FCV_SETRIN         FCVSETRIN
#define FCV_EWTSET         FCVEWTSET
#define FCV_DIAG           FCVDIAG
#define FCV_DENSE          FCVDENSE
#define FCV_DENSESETJAC    FCVDENSESETJAC
#define FCV_BAND           FCVBAND
#define FCV_BANDSETJAC     FCVBANDSETJAC
#define FCV_SPTFQMR        FCVSPTFQMR
#define FCV_SPTFQMRREINIT  FCVSPTFQMRREINIT
#define FCV_SPTFQMRSETJAC  FCVSPTFQMRSETJAC
#define FCV_SPTFQMRSETPREC FCVSPTFQMRSETPREC
#define FCV_SPBCG          FCVSPBCG
#define FCV_SPBCGREINIT    FCVSPBCGREINIT
#define FCV_SPBCGSETJAC    FCVSPBCGSETJAC
#define FCV_SPBCGSETPREC   FCVSPBCGSETPREC
#define FCV_SPGMR          FCVSPGMR
#define FCV_SPGMRREINIT    FCVSPGMRREINIT
#define FCV_SPGMRSETJAC    FCVSPGMRSETJAC
#define FCV_SPGMRSETPREC   FCVSPGMRSETPREC
#define FCV_CVODE          FCVODE
#define FCV_DKY            FCVDKY
#define FCV_FREE           FCVFREE
#define FCV_FUN            FCVFUN
#define FCV_DJAC           FCVDJAC
#define FCV_BJAC           FCVBJAC
#define FCV_PSOL           FCVPSOL
#define FCV_PSET           FCVPSET
#define FCV_JTIMES         FCVJTIMES
#define FCV_EWT            FCVEWT
#define FCV_GETERRWEIGHTS  FCVGETERRWEIGHTS
#define FCV_GETESTLOCALERR FCVGETESTLOCALERR

#elif defined(SUNDIALS_UNDERSCORE_ONE) && defined(SUNDIALS_CASE_LOWER)

#define FCV_MALLOC         fcvmalloc_
#define FCV_REINIT         fcvreinit_
#define FCV_SETIIN         fcvsetiin_
#define FCV_SETRIN         fcvsetrin_
#define FCV_EWTSET         fcvewtset_
#define FCV_DIAG           fcvdiag_
#define FCV_DENSE          fcvdense_
#define FCV_DENSESETJAC    fcvdensesetjac_
#define FCV_BAND           fcvband_
#define FCV_BANDSETJAC     fcvbandsetjac_
#define FCV_SPTFQMR        fcvsptfqmr_
#define FCV_SPTFQMRREINIT  fcvsptfqmrreinit_
#define FCV_SPTFQMRSETJAC  fcvsptfqmrsetjac_
#define FCV_SPTFQMRSETPREC fcvsptfqmrsetprec_
#define FCV_SPBCG          fcvspbcg_
#define FCV_SPBCGREINIT    fcvspbcgreinit_
#define FCV_SPBCGSETJAC    fcvspbcgsetjac_
#define FCV_SPBCGSETPREC   fcvspbcgsetprec_
#define FCV_SPGMR          fcvspgmr_
#define FCV_SPGMRREINIT    fcvspgmrreinit_
#define FCV_SPGMRSETJAC    fcvspgmrsetjac_
#define FCV_SPGMRSETPREC   fcvspgmrsetprec_
#define FCV_CVODE          fcvode_
#define FCV_DKY            fcvdky_
#define FCV_FREE           fcvfree_
#define FCV_FUN            fcvfun_
#define FCV_DJAC           fcvdjac_
#define FCV_BJAC           fcvbjac_
#define FCV_PSOL           fcvpsol_
#define FCV_PSET           fcvpset_
#define FCV_JTIMES         fcvjtimes_
#define FCV_EWT            fcvewt_
#define FCV_GETERRWEIGHTS  fcvgeterrweights_
#define FCV_GETESTLOCALERR fcvgetestlocalerr_

#elif defined(SUNDIALS_UNDERSCORE_ONE) && defined(SUNDIALS_CASE_UPPER)

#define FCV_MALLOC         FCVMALLOC_
#define FCV_REINIT         FCVREINIT_
#define FCV_SETIIN         FCVSETIIN_
#define FCV_SETRIN         FCVSETRIN_
#define FCV_EWTSET         FCVEWTSET_
#define FCV_DIAG           FCVDIAG_
#define FCV_DENSE          FCVDENSE_
#define FCV_DENSESETJAC    FCVDENSESETJAC_
#define FCV_BAND           FCVBAND_
#define FCV_BANDSETJAC     FCVBANDSETJAC_
#define FCV_SPTFQMR        FCVSPTFQMR_
#define FCV_SPTFQMRREINIT  FCVSPTFQMRREINIT_
#define FCV_SPTFQMRSETJAC  FCVSPTFQMRSETJAC_
#define FCV_SPTFQMRSETPREC FCVSPTFQMRSETPREC_
#define FCV_SPBCG          FCVSPBCG_
#define FCV_SPBCGREINIT    FCVSPBCGREINIT_
#define FCV_SPBCGSETJAC    FCVSPBCGSETJAC_
#define FCV_SPBCGSETPREC   FCVSPBCGSETPREC_
#define FCV_SPGMR          FCVSPGMR_
#define FCV_SPGMRREINIT    FCVSPGMRREINIT_
#define FCV_SPGMRSETJAC    FCVSPGMRSETJAC_
#define FCV_SPGMRSETPREC   FCVSPGMRSETPREC_
#define FCV_CVODE          FCVODE_
#define FCV_DKY            FCVDKY_
#define FCV_FREE           FCVFREE_
#define FCV_FUN            FCVFUN_
#define FCV_DJAC           FCVDJAC_
#define FCV_BJAC           FCVBJAC_
#define FCV_PSOL           FCVPSOL_
#define FCV_PSET           FCVPSET_
#define FCV_JTIMES         FCVJTIMES_
#define FCV_EWT            FCVEWT_
#define FCV_GETERRWEIGHTS  FCVGETERRWEIGHTS_
#define FCV_GETESTLOCALERR FCVGETESTLOCALERR_

#elif defined(SUNDIALS_UNDERSCORE_TWO) && defined(SUNDIALS_CASE_LOWER)

#define FCV_MALLOC         fcvmalloc__
#define FCV_REINIT         fcvreinit__
#define FCV_SETIIN         fcvsetiin__
#define FCV_SETRIN         fcvsetrin__
#define FCV_EWTSET         fcvewtset__
#define FCV_DIAG           fcvdiag__
#define FCV_DENSE          fcvdense__
#define FCV_DENSESETJAC    fcvdensesetjac__
#define FCV_BAND           fcvband__
#define FCV_BANDSETJAC     fcvbandsetjac__
#define FCV_SPTFQMR        fcvsptfqmr__
#define FCV_SPTFQMRREINIT  fcvsptfqmrreinit__
#define FCV_SPTFQMRSETJAC  fcvsptfqmrsetjac__
#define FCV_SPTFQMRSETPREC fcvsptfqmrsetprec__
#define FCV_SPBCG          fcvspbcg__
#define FCV_SPBCGREINIT    fcvspbcgreinit__
#define FCV_SPBCGSETJAC    fcvspbcgsetjac__
#define FCV_SPBCGSETPREC   fcvspbcgsetprec__
#define FCV_SPGMR          fcvspgmr__
#define FCV_SPGMRREINIT    fcvspgmrreinit__
#define FCV_SPGMRSETJAC    fcvspgmrsetjac__
#define FCV_SPGMRSETPREC   fcvspgmrsetprec__
#define FCV_CVODE          fcvode__
#define FCV_DKY            fcvdky__
#define FCV_FREE           fcvfree__
#define FCV_FUN            fcvfun__
#define FCV_DJAC           fcvdjac__
#define FCV_BJAC           fcvbjac__
#define FCV_PSOL           fcvpsol__
#define FCV_PSET           fcvpset__
#define FCV_JTIMES         fcvjtimes__
#define FCV_EWT            fcvewt__
#define FCV_GETERRWEIGHTS  fcvgeterrweights__
#define FCV_GETESTLOCALERR fcvgetestlocalerr__

#elif defined(SUNDIALS_UNDERSCORE_TWO) && defined(SUNDIALS_CASE_UPPER)

#define FCV_MALLOC         FCVMALLOC__
#define FCV_REINIT         FCVREINIT__
#define FCV_SETIIN         FCVSETIIN__
#define FCV_SETRIN         FCVSETRIN__
#define FCV_EWTSET         FCVEWTSET__
#define FCV_DIAG           FCVDIAG__
#define FCV_DENSE          FCVDENSE__
#define FCV_DENSESETJAC    FCVDENSESETJAC__
#define FCV_BAND           FCVBAND__
#define FCV_BANDSETJAC     FCVBANDSETJAC__
#define FCV_SPTFQMR        FCVSPTFQMR__
#define FCV_SPTFQMRREINIT  FCVSPTFQMRREINIT__
#define FCV_SPTFQMRSETJAC  FCVSPTFQMRSETJAC__
#define FCV_SPTFQMRSETPREC FCVSPTFQMRSETPREC__
#define FCV_SPBCG          FCVSPBCG__
#define FCV_SPBCGREINIT    FCVSPBCGREINIT__
#define FCV_SPBCGSETJAC    FCVSPBCGSETJAC__
#define FCV_SPBCGSETPREC   FCVSPBCGSETPREC__
#define FCV_SPGMR          FCVSPGMR__
#define FCV_SPGMRREINIT    FCVSPGMRREINIT__
#define FCV_SPGMRSETJAC    FCVSPGMRSETJAC__
#define FCV_SPGMRSETPREC   FCVSPGMRSETPREC__
#define FCV_CVODE          FCVODE__
#define FCV_DKY            FCVDKY__
#define FCV_FREE           FCVFREE__
#define FCV_FUN            FCVFUN__
#define FCV_DJAC           FCVDJAC__
#define FCV_BJAC           FCVBJAC__
#define FCV_PSOL           FCVPSOL__
#define FCV_PSET           FCVPSET__
#define FCV_JTIMES         FCVJTIMES__
#define FCV_EWT            FCVEWT__
#define FCV_GETERRWEIGHTS  FCVGETERRWEIGHTS__
#define FCV_GETESTLOCALERR FCVGETESTLOCALERR__

#endif

  /* Type for user data */
  typedef struct {
    realtype *rpar;
    long int *ipar;
  } *FCVUserData;

  /* Prototypes of exported functions */

  void FCV_MALLOC(realtype *t0, realtype *y0,
                  int *meth, int *itmeth, int *iatol,
                  realtype *rtol, realtype *atol,
                  long int *iout, realtype *rout,
                  long int *ipar, realtype *rpar,
                  int *ier);

  void FCV_REINIT(realtype *t0, realtype *y0,
                  int *iatol, realtype *rtol, realtype *atol,
                  int *ier);

  void FCV_SETIIN(char key_name[], long int *ival, int *ier, int key_len);

  void FCV_SETRIN(char key_name[], realtype *rval, int *ier, int key_len);

  void FCV_EWTSET(int *flag, int *ier);

  void FCV_DIAG(int *ier);

  void FCV_DENSE(long int *neq, int *ier);
  void FCV_DENSESETJAC(int *flag, int *ier);

  void FCV_BAND(long int *neq, long int *mupper, long int *mlower, int *ier);
  void FCV_BANDSETJAC(int *flag, int *ier);

  void FCV_SPGMR(int *pretype, int *gstype, int *maxl, realtype *delt, int *ier);
  void FCV_SPGMRREINIT(int *pretype, int *gstype, realtype *delt, int *ier);
  void FCV_SPGMRSETJAC(int *flag, int *ier);
  void FCV_SPGMRSETPREC(int *flag, int *ier);
  
  void FCV_SPBCG(int *pretype, int *maxl, realtype *delt, int *ier);
  void FCV_SPBCGREINIT(int *pretype, int *maxl, realtype *delt, int *ier);
  void FCV_SPBCGSETJAC(int *flag, int *ier);
  void FCV_SPBCGSETPREC(int *flag, int *ier);
  
  void FCV_SPTFQMR(int *pretype, int *maxl, realtype *delt, int *ier);
  void FCV_SPTFQMRREINIT(int *pretype, int *maxl, realtype *delt, int *ier);
  void FCV_SPTFQMRSETJAC(int *flag, int *ier);
  void FCV_SPTFQMRSETPREC(int *flag, int *ier);

  void FCV_CVODE(realtype *tout, realtype *t, realtype *y, int *itask, int *ier);

  void FCV_DKY(realtype *t, int *k, realtype *dky, int *ier);

  void FCV_GETERRWEIGHTS(realtype *eweight, int *ier);
  void FCV_GETESTLOCALERR(realtype *ele, int *ier);

  void FCV_FREE(void);


  /* Prototypes: Functions Called by the CVODE Solver */
  
  void FCVf(realtype t, N_Vector y, N_Vector ydot, void *f_data);
  
  void FCVDenseJac(long int N, DenseMat J, realtype t, 
                   N_Vector y, N_Vector fy, void *jac_data,
                   N_Vector vtemp1, N_Vector vtemp2, N_Vector vtemp3);
  
  void FCVBandJac(long int N, long int mupper, long int mlower,
                  BandMat J, realtype t, N_Vector y, N_Vector fy,
                  void *jac_data,
                  N_Vector vtemp1, N_Vector vtemp2, N_Vector vtemp3);
  
  int FCVPSet(realtype tn, N_Vector y,N_Vector fy, booleantype jok,
              booleantype *jcurPtr, realtype gamma, void *P_data,
              N_Vector vtemp1, N_Vector vtemp2, N_Vector vtemp3);
  
  int FCVPSol(realtype tn, N_Vector y, N_Vector fy, 
              N_Vector r, N_Vector z,
              realtype gamma, realtype delta,
              int lr, void *P_data, N_Vector vtemp);
  
  int FCVJtimes(N_Vector v, N_Vector Jv, realtype t, 
                N_Vector y, N_Vector fy,
                void *jac_data, N_Vector work);
  
  int FCVEwtSet(N_Vector y, N_Vector ewt, void *e_data);

  /* Declarations for global variables shared amongst various routines */

  extern N_Vector F2C_CVODE_vec;   /* defined in FNVECTOR module */

  extern void *CV_cvodemem;        /* defined in fcvode.c */
  extern long int *CV_iout;        /* defined in fcvode.c */
  extern realtype *CV_rout;        /* defined in fcvode.c */
  extern int CV_nrtfn;             /* defined in fcvode.c */
  extern int CV_ls;                /* defined in fcvode.c */

  /* Linear solver IDs */

  enum { CV_LS_DENSE = 1, CV_LS_BAND = 2, CV_LS_DIAG = 3,
	 CV_LS_SPGMR = 4, CV_LS_SPBCG = 5, CV_LS_SPTFQMR = 6 };

#ifdef __cplusplus
}
#endif

#endif
