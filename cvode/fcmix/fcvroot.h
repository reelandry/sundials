/*
 * -----------------------------------------------------------------
 * $Revision: 1.3 $
 * $Date: 2004-07-26 17:26:30 $
 * ----------------------------------------------------------------- 
 * Programmer(s): Aaron Collier @ LLNL
 * -----------------------------------------------------------------
 * This is the Fortran interface include file for the rootfinding
 * feature of CVODE.
 * -----------------------------------------------------------------
 */

#ifndef _fcvroot_h
#define _fcvroot_h

/*
 * ==============================================================================
 *
 *                   FCVROOT Interface Package
 *
 * The FCVROOT interface package allows programs written in FORTRAN to
 * use the rootfinding feature of the CVODE solver module.
 *
 * The user-callable functions constituting the FCVROOT package are the
 * following: FCVROOTINIT and FCVROOTFREE. The corresponding
 * CVODE subroutine called by each interface function is given below.
 *
 *   -----------------      -----------------------
 *  | FCVROOT routine |    | CVODE function called |
 *   -----------------      -----------------------
 *      FCVROOTINIT     ->     CVodeRootInit
 *      FCVROOTFREE     ->     CVodeRootInit
 *
 * FCVROOTFN is a user-supplied subroutine defining the functions whose
 * roots are sought.
 *
 * ==============================================================================
 *
 *                     Usage of the FCVROOT Interface Package
 *
 * In order to use the rootfinding feature of the CVODE package the user must
 * define the following subroutine:
 *
 *   SUBROUTINE FCVROOTFN (T, Y, G)
 *   DIMENSION Y(*), G(*)
 *
 * The arguments are:
 *   T = independent variable value t  [input]
 *   Y = dependent variable vector y  [input]
 *   G = function values g(t,y)  [output]
 *
 * Also, after calling FCVMALLOC but prior to calling FCVODE, the user must
 * allocate and initialize memory for the FCVROOT module by making the
 * following call:
 *
 *   CALL FCVROOTINIT (NRTFN, IER)
 *
 * The arguments are:
 *   NRTFN = total number of root functions  [input]
 *   IER   = return completion flag (0 = success, -1 = CVODE memory NULL and
 *           -2 = memory allocation error)  [output]
 *
 * The total number of calls made to the root function (FCVROOTFN)
 * can be obtained from IOPT[]
 *
 * If the FCVODE/CVODE memory block is reinitialized to solve a different
 * problem via a call to FCVREINIT, then the counter variable NGE is cleared
 * (reset to zero).
 *
 * To free the memory resources allocated by a prior call to FCVROOTINIT make
 * the following call:
 *
 *   CALL FCVROOTFREE
 *
 * See the CVODE documentation for additional information.
 *
 * ==============================================================================
 */

/* Definitions of interface function names */

#if defined(SUNDIALS_UNDERSCORE_NONE)

#if defined(SUNDIALS_CASE_LOWER)

#define FCV_ROOTINIT   fcvrootinit
#define FCV_ROOTFREE   fcvrootfree
#define FCV_ROOTFN     fcvrootfn

#elif defined(SUNDIALS_CASE_UPPER)

#define FCV_ROOTINIT   FCVROOTINIT
#define FCV_ROOTFREE   FCVROOTFREE
#define FCV_ROOTFN     FCVROOTFN

#endif

#elif defined(SUNDIALS_UNDERSCORE_TWO)

#if defined(SUNDIALS_CASE_LOWER)

#define FCV_ROOTINIT   fcvrootinit__
#define FCV_ROOTFREE   fcvrootfree__
#define FCV_ROOTFN     fcvrootfn__

#elif defined(SUNDIALS_CASE_UPPER)

#define FCV_ROOTINIT   FCVROOTINIT__
#define FCV_ROOTFREE   FCVROOTFREE__
#define FCV_ROOTFN     FCVROOTFN__

#endif

#else

#if defined(SUNDIALS_CASE_LOWER)

#define FCV_ROOTINIT   fcvrootinit_
#define FCV_ROOTFREE   fcvrootfree_
#define FCV_ROOTFN     fcvrootfn_

#elif defined(SUNDIALS_CASE_UPPER)

#define FCV_ROOTINIT   FCVROOTINIT_
#define FCV_ROOTFREE   FCVROOTFREE_
#define FCV_ROOTFN     FCVROOTFN_

#endif

#endif

/* SUNDIALS header files */

#include "sundialstypes.h"  /* definition of SUNDIALS type realtype */
#include "nvector.h"        /* definition of type N_Vector */

/* Prototype of function called by CVROOT module */

void FCVrootfunc(realtype t, N_Vector y, realtype *gout, void *g_data);

#endif
