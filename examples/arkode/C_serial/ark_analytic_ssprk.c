/*-----------------------------------------------------------------
 * Programmer(s): Mustafa Aggul @ SMU
 *---------------------------------------------------------------
 * SUNDIALS Copyright Start
 * Copyright (c) 2002-2024, Lawrence Livermore National Security
 * and Southern Methodist University.
 * All rights reserved.
 *
 * See the top-level LICENSE and NOTICE files for details.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 * SUNDIALS Copyright End
 *---------------------------------------------------------------
 * Example problem:
 *
 * The following is a simple example problem with analytical
 * solution,
 *    dy/dt = lambda*y + 1/(1+t^2) - lambda*atan(t)
 * for t in the interval [0.0, 10.0], with initial condition: y=0.
 *
 * The stiffness of the problem is directly proportional to the
 * value of "lambda".  The value of lambda should be negative to
 * result in a well-posed ODE; for values with magnitude larger
 * than 100 the problem becomes quite stiff.
 *
 * This program solves the problem with the SSP(s,3) method from LSRK.
 * Output is printed every 1.0 units of time (10 total).
 * Run statistics (optional outputs) are printed at the end.
 *-----------------------------------------------------------------*/

/* Header files */
#include <arkode/arkode_lsrkstep.h> /* prototypes for LSRKStep fcts., consts */
#include <math.h>
#include <nvector/nvector_serial.h> /* serial N_Vector types, fcts., macros */
#include <stdio.h>
#include <sundials/sundials_math.h> /* def. of SUNRsqrt, etc. */
#include <sundials/sundials_types.h> /* definition of type sunrealtype          */

#if defined(SUNDIALS_EXTENDED_PRECISION)
#define GSYM "Lg"
#define ESYM "Le"
#define FSYM "Lf"
#else
#define GSYM "g"
#define ESYM "e"
#define FSYM "f"
#endif

#if defined(SUNDIALS_DOUBLE_PRECISION)
#define ATAN(x) (atan((x)))
#elif defined(SUNDIALS_SINGLE_PRECISION)
#define ATAN(x) (atanf((x)))
#elif defined(SUNDIALS_EXTENDED_PRECISION)
#define ATAN(x) (atanl((x)))
#endif

/* User-supplied Functions Called by the Solver */
static int f(sunrealtype t, N_Vector y, N_Vector ydot, void* user_data);

/* Private function to check function return values */
static int check_flag(void* flagvalue, const char* funcname, int opt);

/* Private function to compute error */
static int compute_error(N_Vector y, sunrealtype t);

/* Main Program */
int main(void)
{
  /* general problem parameters */
  sunrealtype T0    = SUN_RCONST(0.0);  /* initial time */
  sunrealtype Tf    = SUN_RCONST(10.0); /* final time */
  sunrealtype dTout = SUN_RCONST(1.0);  /* time between outputs */
  sunindextype NEQ  = 1;                /* number of dependent vars. */

#if defined(SUNDIALS_DOUBLE_PRECISION)
  sunrealtype reltol = SUN_RCONST(1.0e-8); /* tolerances */
  sunrealtype abstol = SUN_RCONST(1.0e-8);
  sunrealtype lambda = SUN_RCONST(-10.0); /* stiffness parameter */
#elif defined(SUNDIALS_SINGLE_PRECISION)
  sunrealtype reltol = SUN_RCONST(1.0e-4); /* tolerances */
  sunrealtype abstol = SUN_RCONST(1.0e-8);
  sunrealtype lambda = SUN_RCONST(-10.0); /* stiffness parameter */
#elif defined(SUNDIALS_EXTENDED_PRECISION)
  sunrealtype reltol = SUN_RCONST(1.0e-8); /* tolerances */
  sunrealtype abstol = SUN_RCONST(1.0e-8);
  sunrealtype lambda = SUN_RCONST(-10.0); /* stiffness parameter */
#endif

  /* general problem variables */
  int flag;                /* reusable error-checking flag */
  N_Vector y       = NULL; /* empty vector for storing solution */
  void* arkode_mem = NULL; /* empty ARKode memory structure */
  FILE *UFID, *FID;
  sunrealtype t, tout;

  /* Create the SUNDIALS context object for this simulation */
  SUNContext ctx;
  flag = SUNContext_Create(SUN_COMM_NULL, &ctx);
  if (check_flag(&flag, "SUNContext_Create", 1)) { return 1; }

  /* Initial diagnostics output */
  printf("\nAnalytical ODE test problem:\n");
  printf("    lambda = %" GSYM "\n", lambda);
  printf("   reltol = %.1" ESYM "\n", reltol);
  printf("   abstol = %.1" ESYM "\n\n", abstol);

  /* Initialize data structures */
  y = N_VNew_Serial(NEQ, ctx); /* Create serial vector for solution */
  if (check_flag((void*)y, "N_VNew_Serial", 0)) { return 1; }
  N_VConst(SUN_RCONST(0.0), y); /* Specify initial condition */

  /* Call LSRKStepCreateSSP to initialize the ARK timestepper module and
     specify the right-hand side function in y'=f(t,y), the initial time
     T0, and the initial dependent variable vector y. */
  arkode_mem = LSRKStepCreateSSP(f, T0, y, ctx);
  if (check_flag((void*)arkode_mem, "ARKStepCreate", 0)) { return 1; }

  /* Set routines */
  flag = ARKodeSetUserData(arkode_mem,
                           (void*)&lambda); /* Pass lambda to user functions */
  if (check_flag(&flag, "ARKodeSetUserData", 1)) { return 1; }

  /* Specify tolerances */
  flag = ARKodeSStolerances(arkode_mem, reltol, abstol);
  if (check_flag(&flag, "ARKStepSStolerances", 1)) { return 1; }

  /* Specify max number of steps allowed */
  flag = ARKodeSetMaxNumSteps(arkode_mem, 1000);
  if (check_flag(&flag, "ARKodeSetMaxNumSteps", 1)) { return 1; }

  /* Specify the SSP(s,3) LSRK method */
  flag = LSRKStepSetSSPMethod(arkode_mem, ARKODE_LSRK_SSP_S_3);
  if (check_flag(&flag, "LSRKStepSetSSPMethod", 1)) { return 1; }

  /* Specify the SSP number of stages */
  flag = LSRKStepSetSSPStageNum(arkode_mem, 9);
  if (check_flag(&flag, "LSRKStepSetSSPStageNum", 1)) { return 1; }

  /* Open output stream for results, output comment line */
  UFID = fopen("solution.txt", "w");
  fprintf(UFID, "# t u\n");

  /* output initial condition to disk */
  fprintf(UFID, " %.16" ESYM " %.16" ESYM "\n", T0, N_VGetArrayPointer(y)[0]);

  /* Main time-stepping loop: calls ARKodeEvolve to perform the integration, then
     prints results.  Stops when the final time has been reached */
  t    = T0;
  tout = T0 + dTout;
  printf("        t           u\n");
  printf("   ---------------------\n");
  while (Tf - t > SUN_RCONST(1.0e-15))
  {
    flag = ARKodeEvolve(arkode_mem, tout, y, &t, ARK_NORMAL); /* call integrator */
    if (check_flag(&flag, "ARKodeEvolve", 1)) { break; }
    printf("  %10.6" FSYM "  %10.6" FSYM "\n", t,
           N_VGetArrayPointer(y)[0]); /* access/print solution */
    fprintf(UFID, " %.16" ESYM " %.16" ESYM "\n", t, N_VGetArrayPointer(y)[0]);
    if (flag < 0)
    { /* unsuccessful solve: break */
      fprintf(stderr, "Solver failure, stopping integration\n");
      break;
    }
    else
    { /* successful solve: update time */
      tout += dTout;
      tout = (tout > Tf) ? Tf : tout;
    }
  }
  printf("   ---------------------\n");
  fclose(UFID);

  /* Print final statistics */
  printf("\nFinal Statistics:\n");
  flag = ARKodePrintAllStats(arkode_mem, stdout, SUN_OUTPUTFORMAT_TABLE);

  /* Print final statistics to a file in CSV format */
  FID  = fopen("ark_analytic_nonlin_stats.csv", "w");
  flag = ARKodePrintAllStats(arkode_mem, FID, SUN_OUTPUTFORMAT_CSV);
  fclose(FID);

  /* check the solution error */
  flag = compute_error(y, t);

  /* Clean up and return */
  N_VDestroy(y);           /* Free y vector */
  ARKodeFree(&arkode_mem); /* Free integrator memory */
  SUNContext_Free(&ctx);   /* Free context */

  return flag;
}

/*-------------------------------
 * Functions called by the solver
 *-------------------------------*/

/* f routine to compute the ODE RHS function f(t,y). */
static int f(sunrealtype t, N_Vector y, N_Vector ydot, void* user_data)
{
  sunrealtype* rdata = (sunrealtype*)user_data; /* cast user_data to sunrealtype */
  sunrealtype lambda = rdata[0]; /* set shortcut for stiffness parameter */
  sunrealtype u = N_VGetArrayPointer(y)[0]; /* access current solution value */

  /* fill in the RHS function: "N_VGetArrayPointer" accesses the 0th entry of ydot */
  N_VGetArrayPointer(ydot)[0] =
    lambda * u + SUN_RCONST(1.0) / (SUN_RCONST(1.0) + t * t) - lambda * ATAN(t);

  return 0; /* return with success */
}

/*-------------------------------
 * Private helper functions
 *-------------------------------*/

/* Check function return value...
    opt == 0 means SUNDIALS function allocates memory so check if
             returned NULL pointer
    opt == 1 means SUNDIALS function returns a flag so check if
             flag >= 0
    opt == 2 means function allocates memory so check if returned
             NULL pointer
*/
static int check_flag(void* flagvalue, const char* funcname, int opt)
{
  int* errflag;

  /* Check if SUNDIALS function returned NULL pointer - no memory allocated */
  if (opt == 0 && flagvalue == NULL)
  {
    fprintf(stderr, "\nSUNDIALS_ERROR: %s() failed - returned NULL pointer\n\n",
            funcname);
    return 1;
  }

  /* Check if flag < 0 */
  else if (opt == 1)
  {
    errflag = (int*)flagvalue;
    if (*errflag < 0)
    {
      fprintf(stderr, "\nSUNDIALS_ERROR: %s() failed with flag = %d\n\n",
              funcname, *errflag);
      return 1;
    }
  }

  /* Check if function returned NULL pointer - no memory allocated */
  else if (opt == 2 && flagvalue == NULL)
  {
    fprintf(stderr, "\nMEMORY_ERROR: %s() failed - returned NULL pointer\n\n",
            funcname);
    return 1;
  }

  return 0;
}

/* check the error */
static int compute_error(N_Vector y, sunrealtype t)
{
  sunrealtype ans, err; /* answer data, error */

  /* compute solution error */
  ans = ATAN(t);
  err = SUNRabs(N_VGetArrayPointer(y)[0] - ans);

  fprintf(stdout, "\nACCURACY at the final time = %" GSYM "\n", err);
  return 0;
}

/*---- end of file ----*/
