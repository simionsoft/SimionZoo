!**********************************************************************************************************************************
! LICENSING
! Copyright (C) 2015  National Renewable Energy Laboratory
!
!    This file is part of AeroDyn.
!
! Licensed under the Apache License, Version 2.0 (the "License");
! you may not use this file except in compliance with the License.
! You may obtain a copy of the License at
!
!     http://www.apache.org/licenses/LICENSE-2.0
!
! Unless required by applicable law or agreed to in writing, software
! distributed under the License is distributed on an "AS IS" BASIS,
! WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
! See the License for the specific language governing permissions and
! limitations under the License.
!
!**********************************************************************************************************************************
! File last committed: $Date: 2015-09-12 14:54:28 -0600 (Sat, 12 Sep 2015) $
! (File) Revision #: $Rev: 163 $
! URL: $HeadURL: https://windsvn.nrel.gov/WT_Perf/branches/v4.x/Source/dependencies/BEMT/BladeElement.f90 $
!**********************************************************************************************************************************
module BladeElement
   
   use AirfoilInfo_Types
   use UnsteadyAero
   use UnsteadyAero_Types
   
   implicit none
   
    integer(IntKi), public, parameter  :: SkewMod_Uncoupled  = 1      ! Uncoupled (no correction) [-]
    integer(IntKi), public, parameter  :: SkewMod_PittPeters = 2      ! Pitt/Peters [-]
    integer(IntKi), public, parameter  :: SkewMod_Coupled    = 3      ! Coupled [-]
   
   
   public :: BE_CalcCxCyCoefs
   public :: BE_CalcOutputs
   
   contains
   
   subroutine BE_CalcCxCyCoefs(phi, useAIDrag, useTIDrag, Cl, Cd, Cx, Cy)
      real(ReKi),             intent(in   ) :: phi
      logical,                intent(in   ) :: useAIDrag
      logical,                intent(in   ) :: useTIDrag
      real(ReKi),             intent(in   ) :: Cl
      real(ReKi),             intent(in   ) :: Cd
      real(ReKi),             intent(  out) :: Cx
      real(ReKi),             intent(  out) :: Cy
   
         !Locals
      real(ReKi)      cphi, sphi
   
      cphi = cos(phi)
      sphi = sin(phi)
   
            ! resolve into normal (x) and tangential (y) forces
       if (  useAIDrag ) then
           Cx = Cl*cphi + Cd*sphi
       else      
           Cx = Cl*cphi
       end if
    
       if (  useTIDrag ) then     
           Cy = Cl*sphi - Cd*cphi
       else     
           Cy = Cl*sphi
       end if
   end subroutine BE_CalcCxCyCoefs
   
   subroutine BE_CalcOutputs(AFInfo, UA_Flag, AOA, W, Re, p_UA, xd_UA, OtherState_UA, Cl, Cd, Cm, ErrStat, ErrMsg)     

      type(AFInfoType),             intent(in   ) :: AFInfo
      logical   ,                   intent(in   ) :: UA_Flag
      real(ReKi),                   intent(in   ) :: AOA            ! Angle of attack in radians
      real(ReKi),                   intent(in   ) :: W
      real(ReKi),                   intent(in   ) :: Re
      type(UA_ParameterType),       intent(in   ) :: p_UA           ! Parameters
      type(UA_DiscreteStateType),   intent(in   ) :: xd_UA          ! Discrete states at Time
      type(UA_OtherStateType),      intent(in   ) :: OtherState_UA  ! Other/optimization states
      real(ReKi),                   intent(  out) :: Cl
      real(ReKi),                   intent(  out) :: Cd
      real(ReKi),                   intent(  out) :: Cm
      integer(IntKi),               intent(  out) :: ErrStat     ! Error status of the operation
      character(*),                 intent(  out) :: ErrMsg      ! Error message if ErrStat /= ErrID_None
   
      real                            :: IntAFCoefs(4)                ! The interpolated airfoil coefficients.
      integer                         :: s1
      type(UA_InputType)              :: u_UA
      type(UA_OutputType)             :: y_UA          !
      
      ErrStat = ErrID_None
      ErrMsg  = ''
      IntAFCoefs = 0.0_ReKi ! initialize in case we only don't have 4 columns in the airfoil data (i.e., so cm is zero if not in the file)
      
      if (UA_Flag) then
         u_UA%alpha = AOA   
         u_UA%Re    = Re
         u_UA%U     = W
         !bjj: TODO: this gets called element-by-element (not all at once). Are OtherState%iBladeNode and OtherState%iBlade set properly?
         call UA_CalcOutput(u_UA, p_UA, xd_UA, OtherState_UA, AFInfo, y_UA, errStat, errMsg )
         Cl         = y_UA%Cl
         Cd         = y_UA%Cd
         Cm         = y_UA%Cm
      else
            ! NOTE: we use Table(1) because the right now we can only interpolate with AOA and not Re or other variables.  If we had multiple tables stored
            ! for changes in other variables (Re, Mach #, etc) then then we would need to interpolate across tables.
            !
         s1 = size(AFInfo%Table(1)%Coefs,2)
   
         IntAFCoefs(1:s1) = CubicSplineInterpM( 1.0_ReKi*real( AOA*R2D, ReKi ) &
                                              , AFInfo%Table(1)%Alpha &
                                              , AFInfo%Table(1)%Coefs &
                                              , AFInfo%Table(1)%SplineCoefs &
                                              , ErrStat, ErrMsg )
   
         Cl = IntAFCoefs(1)
         Cd = IntAFCoefs(2)
         Cm = IntAFCoefs(3)
      end if
      
   end subroutine BE_CalcOutputs
   
end module BladeElement