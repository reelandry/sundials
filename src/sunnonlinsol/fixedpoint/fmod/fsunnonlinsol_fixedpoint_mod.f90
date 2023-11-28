! This file was automatically generated by SWIG (http://www.swig.org).
! Version 4.0.0
!
! Do not make changes to this file unless you know what you are doing--modify
! the SWIG interface file instead.

! ---------------------------------------------------------------
! Programmer(s): Auto-generated by swig.
! ---------------------------------------------------------------
! SUNDIALS Copyright Start
! Copyright (c) 2002-2023, Lawrence Livermore National Security
! and Southern Methodist University.
! All rights reserved.
!
! See the top-level LICENSE and NOTICE files for details.
!
! SPDX-License-Identifier: BSD-3-Clause
! SUNDIALS Copyright End
! ---------------------------------------------------------------

module fsunnonlinsol_fixedpoint_mod
 use, intrinsic :: ISO_C_BINDING
 use fsundials_types_mod
 use fsundials_nvector_mod
 use fsundials_context_mod
 use fsundials_types_mod
 use fsundials_nonlinearsolver_mod
 use fsundials_nvector_mod
 use fsundials_context_mod
 use fsundials_types_mod
 implicit none
 private

 ! DECLARATION CONSTRUCTS
 public :: FSUNNonlinSol_FixedPoint
 public :: FSUNNonlinSol_FixedPointSens
 public :: FSUNNonlinSolGetType_FixedPoint
 public :: FSUNNonlinSolInitialize_FixedPoint
 public :: FSUNNonlinSolSolve_FixedPoint
 public :: FSUNNonlinSolFree_FixedPoint
 public :: FSUNNonlinSolSetSysFn_FixedPoint
 public :: FSUNNonlinSolSetConvTestFn_FixedPoint
 public :: FSUNNonlinSolSetMaxIters_FixedPoint
 public :: FSUNNonlinSolSetDamping_FixedPoint
 public :: FSUNNonlinSolGetNumIters_FixedPoint
 public :: FSUNNonlinSolGetCurIter_FixedPoint
 public :: FSUNNonlinSolGetNumConvFails_FixedPoint
 public :: FSUNNonlinSolGetSysFn_FixedPoint

! WRAPPER DECLARATIONS
interface
function swigc_FSUNNonlinSol_FixedPoint(farg1, farg2, farg3) &
bind(C, name="_wrap_FSUNNonlinSol_FixedPoint") &
result(fresult)
use, intrinsic :: ISO_C_BINDING
type(C_PTR), value :: farg1
integer(C_INT), intent(in) :: farg2
type(C_PTR), value :: farg3
type(C_PTR) :: fresult
end function

function swigc_FSUNNonlinSol_FixedPointSens(farg1, farg2, farg3, farg4) &
bind(C, name="_wrap_FSUNNonlinSol_FixedPointSens") &
result(fresult)
use, intrinsic :: ISO_C_BINDING
integer(C_INT), intent(in) :: farg1
type(C_PTR), value :: farg2
integer(C_INT), intent(in) :: farg3
type(C_PTR), value :: farg4
type(C_PTR) :: fresult
end function

function swigc_FSUNNonlinSolGetType_FixedPoint(farg1) &
bind(C, name="_wrap_FSUNNonlinSolGetType_FixedPoint") &
result(fresult)
use, intrinsic :: ISO_C_BINDING
type(C_PTR), value :: farg1
integer(C_INT) :: fresult
end function

function swigc_FSUNNonlinSolInitialize_FixedPoint(farg1) &
bind(C, name="_wrap_FSUNNonlinSolInitialize_FixedPoint") &
result(fresult)
use, intrinsic :: ISO_C_BINDING
type(C_PTR), value :: farg1
integer(C_INT) :: fresult
end function

function swigc_FSUNNonlinSolSolve_FixedPoint(farg1, farg2, farg3, farg4, farg5, farg6, farg7) &
bind(C, name="_wrap_FSUNNonlinSolSolve_FixedPoint") &
result(fresult)
use, intrinsic :: ISO_C_BINDING
type(C_PTR), value :: farg1
type(C_PTR), value :: farg2
type(C_PTR), value :: farg3
type(C_PTR), value :: farg4
real(C_DOUBLE), intent(in) :: farg5
integer(C_INT), intent(in) :: farg6
type(C_PTR), value :: farg7
integer(C_INT) :: fresult
end function

function swigc_FSUNNonlinSolFree_FixedPoint(farg1) &
bind(C, name="_wrap_FSUNNonlinSolFree_FixedPoint") &
result(fresult)
use, intrinsic :: ISO_C_BINDING
type(C_PTR), value :: farg1
integer(C_INT) :: fresult
end function

function swigc_FSUNNonlinSolSetSysFn_FixedPoint(farg1, farg2) &
bind(C, name="_wrap_FSUNNonlinSolSetSysFn_FixedPoint") &
result(fresult)
use, intrinsic :: ISO_C_BINDING
type(C_PTR), value :: farg1
type(C_FUNPTR), value :: farg2
integer(C_INT) :: fresult
end function

function swigc_FSUNNonlinSolSetConvTestFn_FixedPoint(farg1, farg2, farg3) &
bind(C, name="_wrap_FSUNNonlinSolSetConvTestFn_FixedPoint") &
result(fresult)
use, intrinsic :: ISO_C_BINDING
type(C_PTR), value :: farg1
type(C_FUNPTR), value :: farg2
type(C_PTR), value :: farg3
integer(C_INT) :: fresult
end function

function swigc_FSUNNonlinSolSetMaxIters_FixedPoint(farg1, farg2) &
bind(C, name="_wrap_FSUNNonlinSolSetMaxIters_FixedPoint") &
result(fresult)
use, intrinsic :: ISO_C_BINDING
type(C_PTR), value :: farg1
integer(C_INT), intent(in) :: farg2
integer(C_INT) :: fresult
end function

function swigc_FSUNNonlinSolSetDamping_FixedPoint(farg1, farg2) &
bind(C, name="_wrap_FSUNNonlinSolSetDamping_FixedPoint") &
result(fresult)
use, intrinsic :: ISO_C_BINDING
type(C_PTR), value :: farg1
real(C_DOUBLE), intent(in) :: farg2
integer(C_INT) :: fresult
end function

function swigc_FSUNNonlinSolGetNumIters_FixedPoint(farg1, farg2) &
bind(C, name="_wrap_FSUNNonlinSolGetNumIters_FixedPoint") &
result(fresult)
use, intrinsic :: ISO_C_BINDING
type(C_PTR), value :: farg1
type(C_PTR), value :: farg2
integer(C_INT) :: fresult
end function

function swigc_FSUNNonlinSolGetCurIter_FixedPoint(farg1, farg2) &
bind(C, name="_wrap_FSUNNonlinSolGetCurIter_FixedPoint") &
result(fresult)
use, intrinsic :: ISO_C_BINDING
type(C_PTR), value :: farg1
type(C_PTR), value :: farg2
integer(C_INT) :: fresult
end function

function swigc_FSUNNonlinSolGetNumConvFails_FixedPoint(farg1, farg2) &
bind(C, name="_wrap_FSUNNonlinSolGetNumConvFails_FixedPoint") &
result(fresult)
use, intrinsic :: ISO_C_BINDING
type(C_PTR), value :: farg1
type(C_PTR), value :: farg2
integer(C_INT) :: fresult
end function

function swigc_FSUNNonlinSolGetSysFn_FixedPoint(farg1, farg2) &
bind(C, name="_wrap_FSUNNonlinSolGetSysFn_FixedPoint") &
result(fresult)
use, intrinsic :: ISO_C_BINDING
type(C_PTR), value :: farg1
type(C_PTR), value :: farg2
integer(C_INT) :: fresult
end function

end interface


contains
 ! MODULE SUBPROGRAMS
function FSUNNonlinSol_FixedPoint(y, m, sunctx) &
result(swig_result)
use, intrinsic :: ISO_C_BINDING
type(SUNNonlinearSolver), pointer :: swig_result
type(N_Vector), target, intent(inout) :: y
integer(C_INT), intent(in) :: m
type(C_PTR) :: sunctx
type(C_PTR) :: fresult 
type(C_PTR) :: farg1 
integer(C_INT) :: farg2 
type(C_PTR) :: farg3 

farg1 = c_loc(y)
farg2 = m
farg3 = sunctx
fresult = swigc_FSUNNonlinSol_FixedPoint(farg1, farg2, farg3)
call c_f_pointer(fresult, swig_result)
end function

function FSUNNonlinSol_FixedPointSens(count, y, m, sunctx) &
result(swig_result)
use, intrinsic :: ISO_C_BINDING
type(SUNNonlinearSolver), pointer :: swig_result
integer(C_INT), intent(in) :: count
type(N_Vector), target, intent(inout) :: y
integer(C_INT), intent(in) :: m
type(C_PTR) :: sunctx
type(C_PTR) :: fresult 
integer(C_INT) :: farg1 
type(C_PTR) :: farg2 
integer(C_INT) :: farg3 
type(C_PTR) :: farg4 

farg1 = count
farg2 = c_loc(y)
farg3 = m
farg4 = sunctx
fresult = swigc_FSUNNonlinSol_FixedPointSens(farg1, farg2, farg3, farg4)
call c_f_pointer(fresult, swig_result)
end function

function FSUNNonlinSolGetType_FixedPoint(nls) &
result(swig_result)
use, intrinsic :: ISO_C_BINDING
integer(SUNNonlinearSolver_Type) :: swig_result
type(SUNNonlinearSolver), target, intent(inout) :: nls
integer(C_INT) :: fresult 
type(C_PTR) :: farg1 

farg1 = c_loc(nls)
fresult = swigc_FSUNNonlinSolGetType_FixedPoint(farg1)
swig_result = fresult
end function

function FSUNNonlinSolInitialize_FixedPoint(nls) &
result(swig_result)
use, intrinsic :: ISO_C_BINDING
integer(C_INT) :: swig_result
type(SUNNonlinearSolver), target, intent(inout) :: nls
integer(C_INT) :: fresult 
type(C_PTR) :: farg1 

farg1 = c_loc(nls)
fresult = swigc_FSUNNonlinSolInitialize_FixedPoint(farg1)
swig_result = fresult
end function

function FSUNNonlinSolSolve_FixedPoint(nls, y0, y, w, tol, callsetup, mem) &
result(swig_result)
use, intrinsic :: ISO_C_BINDING
integer(C_INT) :: swig_result
type(SUNNonlinearSolver), target, intent(inout) :: nls
type(N_Vector), target, intent(inout) :: y0
type(N_Vector), target, intent(inout) :: y
type(N_Vector), target, intent(inout) :: w
real(C_DOUBLE), intent(in) :: tol
integer(C_INT), intent(in) :: callsetup
type(C_PTR) :: mem
integer(C_INT) :: fresult 
type(C_PTR) :: farg1 
type(C_PTR) :: farg2 
type(C_PTR) :: farg3 
type(C_PTR) :: farg4 
real(C_DOUBLE) :: farg5 
integer(C_INT) :: farg6 
type(C_PTR) :: farg7 

farg1 = c_loc(nls)
farg2 = c_loc(y0)
farg3 = c_loc(y)
farg4 = c_loc(w)
farg5 = tol
farg6 = callsetup
farg7 = mem
fresult = swigc_FSUNNonlinSolSolve_FixedPoint(farg1, farg2, farg3, farg4, farg5, farg6, farg7)
swig_result = fresult
end function

function FSUNNonlinSolFree_FixedPoint(nls) &
result(swig_result)
use, intrinsic :: ISO_C_BINDING
integer(C_INT) :: swig_result
type(SUNNonlinearSolver), target, intent(inout) :: nls
integer(C_INT) :: fresult 
type(C_PTR) :: farg1 

farg1 = c_loc(nls)
fresult = swigc_FSUNNonlinSolFree_FixedPoint(farg1)
swig_result = fresult
end function

function FSUNNonlinSolSetSysFn_FixedPoint(nls, sysfn) &
result(swig_result)
use, intrinsic :: ISO_C_BINDING
integer(C_INT) :: swig_result
type(SUNNonlinearSolver), target, intent(inout) :: nls
type(C_FUNPTR), intent(in), value :: sysfn
integer(C_INT) :: fresult 
type(C_PTR) :: farg1 
type(C_FUNPTR) :: farg2 

farg1 = c_loc(nls)
farg2 = sysfn
fresult = swigc_FSUNNonlinSolSetSysFn_FixedPoint(farg1, farg2)
swig_result = fresult
end function

function FSUNNonlinSolSetConvTestFn_FixedPoint(nls, ctestfn, ctest_data) &
result(swig_result)
use, intrinsic :: ISO_C_BINDING
integer(C_INT) :: swig_result
type(SUNNonlinearSolver), target, intent(inout) :: nls
type(C_FUNPTR), intent(in), value :: ctestfn
type(C_PTR) :: ctest_data
integer(C_INT) :: fresult 
type(C_PTR) :: farg1 
type(C_FUNPTR) :: farg2 
type(C_PTR) :: farg3 

farg1 = c_loc(nls)
farg2 = ctestfn
farg3 = ctest_data
fresult = swigc_FSUNNonlinSolSetConvTestFn_FixedPoint(farg1, farg2, farg3)
swig_result = fresult
end function

function FSUNNonlinSolSetMaxIters_FixedPoint(nls, maxiters) &
result(swig_result)
use, intrinsic :: ISO_C_BINDING
integer(C_INT) :: swig_result
type(SUNNonlinearSolver), target, intent(inout) :: nls
integer(C_INT), intent(in) :: maxiters
integer(C_INT) :: fresult 
type(C_PTR) :: farg1 
integer(C_INT) :: farg2 

farg1 = c_loc(nls)
farg2 = maxiters
fresult = swigc_FSUNNonlinSolSetMaxIters_FixedPoint(farg1, farg2)
swig_result = fresult
end function

function FSUNNonlinSolSetDamping_FixedPoint(nls, beta) &
result(swig_result)
use, intrinsic :: ISO_C_BINDING
integer(C_INT) :: swig_result
type(SUNNonlinearSolver), target, intent(inout) :: nls
real(C_DOUBLE), intent(in) :: beta
integer(C_INT) :: fresult 
type(C_PTR) :: farg1 
real(C_DOUBLE) :: farg2 

farg1 = c_loc(nls)
farg2 = beta
fresult = swigc_FSUNNonlinSolSetDamping_FixedPoint(farg1, farg2)
swig_result = fresult
end function

function FSUNNonlinSolGetNumIters_FixedPoint(nls, niters) &
result(swig_result)
use, intrinsic :: ISO_C_BINDING
integer(C_INT) :: swig_result
type(SUNNonlinearSolver), target, intent(inout) :: nls
integer(C_LONG), dimension(*), target, intent(inout) :: niters
integer(C_INT) :: fresult 
type(C_PTR) :: farg1 
type(C_PTR) :: farg2 

farg1 = c_loc(nls)
farg2 = c_loc(niters(1))
fresult = swigc_FSUNNonlinSolGetNumIters_FixedPoint(farg1, farg2)
swig_result = fresult
end function

function FSUNNonlinSolGetCurIter_FixedPoint(nls, iter) &
result(swig_result)
use, intrinsic :: ISO_C_BINDING
integer(C_INT) :: swig_result
type(SUNNonlinearSolver), target, intent(inout) :: nls
integer(C_INT), dimension(*), target, intent(inout) :: iter
integer(C_INT) :: fresult 
type(C_PTR) :: farg1 
type(C_PTR) :: farg2 

farg1 = c_loc(nls)
farg2 = c_loc(iter(1))
fresult = swigc_FSUNNonlinSolGetCurIter_FixedPoint(farg1, farg2)
swig_result = fresult
end function

function FSUNNonlinSolGetNumConvFails_FixedPoint(nls, nconvfails) &
result(swig_result)
use, intrinsic :: ISO_C_BINDING
integer(C_INT) :: swig_result
type(SUNNonlinearSolver), target, intent(inout) :: nls
integer(C_LONG), dimension(*), target, intent(inout) :: nconvfails
integer(C_INT) :: fresult 
type(C_PTR) :: farg1 
type(C_PTR) :: farg2 

farg1 = c_loc(nls)
farg2 = c_loc(nconvfails(1))
fresult = swigc_FSUNNonlinSolGetNumConvFails_FixedPoint(farg1, farg2)
swig_result = fresult
end function

function FSUNNonlinSolGetSysFn_FixedPoint(nls, sysfn) &
result(swig_result)
use, intrinsic :: ISO_C_BINDING
integer(C_INT) :: swig_result
type(SUNNonlinearSolver), target, intent(inout) :: nls
type(C_FUNPTR), target, intent(inout) :: sysfn
integer(C_INT) :: fresult 
type(C_PTR) :: farg1 
type(C_PTR) :: farg2 

farg1 = c_loc(nls)
farg2 = c_loc(sysfn)
fresult = swigc_FSUNNonlinSolGetSysFn_FixedPoint(farg1, farg2)
swig_result = fresult
end function


end module
