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
! File last committed: $Date$
! (File) Revision #: $Rev$
! URL: $HeadURL$
!**********************************************************************************************************************************
module BEMTCoupled
 
   use NWTC_Library
   use BladeElement

   implicit none
   
   
   
   
   private
   
   public :: BEMTC_ElementalErrFn                   ! Computes the coupled method residual value and needs to be public because it is used in the user-defined
                                                 !     function, fcn, which is part of the hybrd1() solver
   public :: BEMTC_Elemental
   
   public :: BEMTC_Wind
   contains
   
! This is the residual calculation for the coupled BEM solve

function BEMTC_ElementalErrFn(axInduction, tanInduction, psi, chi0, airDens, mu, numBlades, rlocal, rtip, chord, theta,  AFInfo, &
                              Vx, Vy, Vinf, useTanInd, useAIDrag, useTIDrag, useHubLoss, useTipLoss, hubLossConst, tipLossConst, SkewWakeMod, &
                              UA_Flag, p_UA, xd_UA, OtherState_UA, &
                              ErrStat, ErrMsg)
      

   real(ReKi),             intent(in   ) :: axInduction
   real(ReKi),             intent(in   ) :: tanInduction
   real(ReKi),             intent(in   ) :: psi
   real(ReKi),             intent(in   ) :: chi0
   real(ReKi),             intent(in   ) :: airDens
   real(ReKi),             intent(in   ) :: mu
   integer,                intent(in   ) :: numBlades
   real(ReKi),             intent(in   ) :: rlocal   
   real(ReKi),             intent(in   ) :: rtip   
   real(ReKi),             intent(in   ) :: chord 
   real(ReKi),             intent(in   ) :: theta         
   type(AFInfoType),       intent(in   ) :: AFInfo
   real(ReKi),             intent(in   ) :: Vx
   real(ReKi),             intent(in   ) :: Vy
   real(ReKi),             intent(in   ) :: Vinf
   logical,                intent(in   ) :: useTanInd 
   logical,                intent(in   ) :: useAIDrag
   logical,                intent(in   ) :: useTIDrag
   logical,                intent(in   ) :: useHubLoss
   logical,                intent(in   ) :: useTipLoss
   real(ReKi),             intent(in   ) :: hubLossConst
   real(ReKi),             intent(in   ) :: tipLossConst
   integer,                intent(in   ) :: SkewWakeMod   ! Skewed wake model
   logical,                intent(in   ) :: UA_Flag
   type(UA_ParameterType),       intent(in   ) :: p_UA           ! Parameters
   type(UA_DiscreteStateType),   intent(in   ) :: xd_UA          ! Discrete states at Time
   type(UA_OtherStateType),      intent(in   ) :: OtherState_UA  ! Other/optimization states
   integer(IntKi),         intent(  out) :: ErrStat       ! Error status of the operation
   character(*),           intent(  out) :: ErrMsg        ! Error message if ErrStat /= ErrID_None
  
   real(ReKi)                            :: BEMTC_ElementalErrFn(2)
   ! Local variables
   
   
   real(ReKi)                            :: fzero, phi, AOA, Re, Cl, Cd, Cx, Cy, Cm, chi
   integer                               :: I
   
   ErrStat = ErrID_None
   ErrMsg  = ""
    
   
      
   BEMTC_ElementalErrFn = BEMTC_Elemental(axInduction, tanInduction, psi, chi0,  airDens, mu, numBlades, rlocal, rtip, chord, theta,  AFInfo, &
                              Vx, Vy, Vinf, useTanInd, useAIDrag, useTIDrag, useHubLoss, useTipLoss, hubLossConst, tipLossConst, SkewWakeMod, &
                              UA_Flag, p_UA, xd_UA, OtherState_UA, &
                              phi, AOA, Re, Cl, Cd, Cx, Cy, Cm, chi, ErrStat, ErrMsg)
      
   
   
end function BEMTC_ElementalErrFn
   
! This is the coupled calculation for the  BEMT solve
function BEMTC_Elemental( axInduction, tanInduction, psi, chi0, airDens, mu, numBlades, rlocal, rtip, chord, theta,  AFInfo, &
                              Vx, Vy, Vinf, useTanInd, useAIDrag, useTIDrag, useHubLoss, useTipLoss, hubLossConst, tipLossConst, SkewWakeMod, &
                              UA_Flag, p_UA, xd_UA, OtherState_UA, &
                              phi, AOA, Re, Cl, Cd, Cx, Cy, Cm, chi, ErrStat, ErrMsg)
      
   !real(ReKi),             intent(in   ) :: phi
   real(ReKi),             intent(in   ) :: axInduction
   real(ReKi),             intent(in   ) :: tanInduction
   real(ReKi),             intent(in   ) :: psi
   real(ReKi),             intent(in   ) :: chi0
   real(ReKi),             intent(in   ) :: airDens
   real(ReKi),             intent(in   ) :: mu
   integer,                intent(in   ) :: numBlades
   real(ReKi),             intent(in   ) :: rlocal   
   real(ReKi),             intent(in   ) :: rtip   
   real(ReKi),             intent(in   ) :: chord 
   real(ReKi),             intent(in   ) :: theta         
   type(AFInfoType),       intent(in   ) :: AFInfo
   real(ReKi),             intent(in   ) :: Vx
   real(ReKi),             intent(in   ) :: Vy
   real(ReKi),             intent(in   ) :: Vinf
   logical,                intent(in   ) :: useTanInd 
   logical,                intent(in   ) :: useAIDrag
   logical,                intent(in   ) :: useTIDrag
   logical,                intent(in   ) :: useHubLoss
   logical,                intent(in   ) :: useTipLoss
   real(ReKi),             intent(in   ) :: hubLossConst
   real(ReKi),             intent(in   ) :: tipLossConst
   integer,                intent(in   ) :: SkewWakeMod   ! Skewed wake model
   logical,                intent(in   ) :: UA_Flag
   type(UA_ParameterType),       intent(in   ) :: p_UA           ! Parameters
   type(UA_DiscreteStateType),   intent(in   ) :: xd_UA          ! Discrete states at Time
   type(UA_OtherStateType),      intent(in   ) :: OtherState_UA  ! Other/optimization states
   real(ReKi),             intent(  out) :: phi, AOA, Re, Cl, Cd, Cx, Cy, Cm, chi
   integer(IntKi),         intent(  out) :: ErrStat       ! Error status of the operation
   character(*),           intent(  out) :: ErrMsg        ! Error message if ErrStat /= ErrID_None
  
   real(ReKi)                            :: BEMTC_Elemental(2)  ! These are the two residuals to return
   
   
   ! Local variables

   real(ReKi)                            :: degAOA, WNorm, W, R1, R2
   integer                               :: I, kk
   
   ErrStat = ErrID_None
   ErrMsg  = ""
    
     
      
         ! Compute phi, AOA, Re, and chi based on current values of axInduction, tanInduction, local wind, etc.
      W = BEMTC_Wind(axInduction, tanInduction, Vx, Vy, chord, theta, airDens, mu, chi0, psi, phi, AOA, Re, chi)
      
         ! Look up Cl and Cd based on AOA and Re and Airfoil information  
      call  BE_CalcOutputs( AFInfo, UA_Flag, AOA*R2D, W, log(Re), p_UA, xd_UA, OtherState_UA, Cl, Cd, Cm,  errStat, errMsg)  
      !call  BE_CalcOutputs( AFInfo, AOA*R2D, log(Re), Cl, Cd, errStat, errMsg) ! AOA is in degrees in this look up table and Re is in log(Re)
      if (errStat >= AbortErrLev) then
         call SetErrStat( errStat, errMsg, errStat, errMsg, 'BEMTC_Elemental' ) 
         return
      end if   
      
         ! Determine Cx, Cy from Cl, Cd and phi  
      call BE_CalcCxCyCoefs(phi, useAIDrag, useTIDrag, Cl, Cd, Cx, Cy) ! Note: Cn is not the same as Cx for cases where theta is non-zero. 
          
         ! The coupled method requires the local total wind normalized with the total local freestream velocity
      Wnorm = W / Vinf
      
      call BEMTC_ElementalInduction( axInduction, tanInduction, rlocal, chord, rtip, Wnorm, phi, Cx, Cy, numBlades, &
                              Vx, Vy, chi0, psi,  useHubLoss, useTipLoss, hubLossConst, tipLossConst, &
                              R1, R2)
      
      if (errStat >= AbortErrLev) then
         call SetErrStat( errStat, errMsg, errStat, errMsg, 'BEMTC_Elemental' ) 
         return
      end if
      
      BEMTC_Elemental = (/R1,R2/)  ! the residual
      
  
   
end function BEMTC_Elemental
                           
                           
real(ReKi) function BEMTC_Wind(a, ap, Vx, Vy, chord, theta, airDens, mu, chi0, psi, phi, AOA, Re, chi)
   ! in
   real(ReKi), intent(in) ::  a, ap, Vx, Vy
   real(ReKi), intent(in) :: chord, theta, airDens, mu, chi0, psi
   
   ! out
    real(ReKi), intent(out) :: phi, AOA,  Re, chi
    
    
    ! local
    real(ReKi) :: W,  Wx, Wy

    ! inflow velocity
    chi = (0.6*a + 1)*chi0
    Wx = Vx*(cos(chi0) - a) + Vy*ap*sin(chi)*cos(psi)*(1 + sin(chi)*sin(psi))
    Wy = Vy*(1 + ap*cos(chi)*(1 + sin(chi)*sin(psi))) + Vx*cos(psi)*(a*tan(chi/2.0) - sin(chi0))
    W = sqrt(Wx**2 + Wy**2)

    ! Alternate computation of inflow velocity using vectoral basis developed by Rick Damiani.
    !Vx, Vy, Vz are in the blade-element coord sys
    !Vmag = sqrt(Vx**2+Vy**2+Vz**2)
    !Wx   = -Vmag*a*cos(precone) + Vx + Vy*ap*sin(chi)*cos(psi)*(1 + sin(chi)*sin(psi))
    ! Wx = Vx
    ! inflow angle and angle of attack
    
    phi = atan2(Wx,Wy)
    AOA = phi - theta 

    Re = airDens * W * chord / mu
    
    BEMTC_Wind = W
   
end function BEMTC_Wind                           
                           
                           
subroutine BEMTC_ElementalInduction(a, ap, r, chord, Rtip, Wnorm, phi, cn, ct, B, &
                              Vx, Vy, chi0, azimuth, hubLoss, tipLoss, hubLossConst, tipLossConst, R1, R2)

    implicit none

   ! integer, parameter :: dp = kind(0.d0)

    ! in
    real(ReKi), intent(in   ) :: a, ap
    real(ReKi), intent(in   ) :: r, chord, Rtip, Wnorm, phi, cn, ct
    integer   , intent(in   ) :: B
    real(ReKi), intent(in   ) :: Vx, Vy
    real(ReKi), intent(in   ) :: chi0, azimuth
    logical   , intent(in   ) :: hubLoss, tipLoss
    real(ReKi), intent(in   ) :: hubLossConst
    real(ReKi), intent(in   ) :: tipLossConst

    
    ! out
    real(ReKi), intent(  out) :: R1, R2
    
    
    
    !f2py logical, optional, intent(in) :: useCd = 1, hubLoss = 1, tipLoss = 1, wakerotation = 1

    

    ! local
    real(ReKi) ::  sigma_p, sphi, cphi, saz, caz, cchi, schi, schi0, cchi0
    real(ReKi) :: factortip, Ftip, factorhub, Fhub
    real(ReKi) :: F
    real(ReKi) :: chi, CT_e, CQ_e, CT_m, CQ_m
    real(ReKi) :: CT_f, CT_fp, k0, k1, k2


    
    ! constants
    !pi      = 3.1415926535897932_ReKi
    sigma_p = B/2.0_ReKi/pi*chord/r
    sphi    = sin(phi)
    cphi    = cos(phi)

    saz = sin(azimuth)
    caz = cos(azimuth)
    !bjj: why aren't we using the "pi" variable?
    if ( EqualRealNos(azimuth, 3.141593_ReKi) ) then
       saz = 0.0_ReKi
    else if ( EqualRealNos(azimuth, 1.570796_ReKi) ) then
       saz = 1.0_ReKi
    else if ( EqualRealNos(azimuth, 3*1.570796_ReKi )) then
       saz = -1.0_ReKi
    else     
       saz = sin(azimuth)
    end if
    
    if ( EqualRealNos(azimuth, 3.141593_ReKi) ) then
       caz = -1.0_ReKi
    else if ( EqualRealNos(azimuth, 1.570796_ReKi) ) then
       caz = 0.0_ReKi
    else if ( EqualRealNos(azimuth, 3*1.570796_ReKi )) then
       caz = 0.0_ReKi
    else     
       caz = cos(azimuth)
    end if
    

    ! Prandtl's tip and hub loss factor
    
    Ftip = 1.0
   if ( tipLoss ) then
      factortip = tipLossConst/abs(sphi)
      Ftip = (2.0/pi)*acos(exp(-factortip))
   end if

   Fhub = 1.0
   if ( hubLoss ) then
      factorhub = hubLossConst/abs(sphi)
      Fhub = (2.0/pi)*acos(exp(-factorhub))
   end if

    F = Ftip * Fhub

    
    ! residuals
    chi  = (0.6*a + 1)*chi0
    
    cchi  = cos(chi)
    schi  = sin(chi)
    cchi0 = cos(chi0)
    schi0 = sin(chi0)
    
    CT_e = Wnorm**2 * sigma_p * cn
    CQ_e = Wnorm**2 * sigma_p * (ct*cchi - cn*schi*caz)

    if (a < 0.4) then
        CT_m = 4*a*F*sqrt(1 - a*(2*cchi0 - a))

!         CT_m = 4*a*F*(cos(chi0) + tan(chi/2.0)*sin(chi0) - a/(cos(chi/2.0)**2))

    else if (a < 1.0) then
        CT_f = 4*0.4*F*sqrt(1 - 0.4*(2*cchi0 - 0.4))
        CT_fp = 2*0.4*F/sqrt(1 - 0.4*(2*cchi0 - 0.4))*(-2*cchi0 + 2*0.4) + &
                sqrt(1 - 0.4*(2*cchi0 - 0.4))*4*F

!         CT_f = 4*0.4*F*(cos(chi0) + tan(chi/2.0)*sin(chi0) - 0.4/(cos(chi/2.0)**2))
!         CT_fp = 4*0.4*F*(-1.0/(cos(chi/2.0)**2)) + (cos(chi0) + tan(chi/2.0)*sin(chi0) - 0.4/(cos(chi/2.0)**2))*4*F

        k2 = (2*cchi0 - CT_f - CT_fp*(1-0.4)) / ((1-0.4)**2)
        k1 = CT_fp - 2*k2*0.4
        k0 = 2*cchi0 - k1 - k2

        CT_m = k0 + k1*a + k2*a**2

    else
        CT_m = 4*a*F*(a - 1)*cchi0

    end if
!     CQ_m = 4*Vy/Vx*ap*F*(cos(chi0) - a*F)*(cos(azimuth)**2 + cos(chi)**2*sin(azimuth)**2)
       CQ_m = 4*Vy/Vx*ap*F*(cchi0 - a    )*( caz**2         + cchi**2    *saz**2)
         !   4*Vy/Vx*ap*F*(cos(chi0) - a)*(cos(azimuth)**2 + cos(chi)**2*sin(azimuth)**2)
    
    !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    
    
!    CT_e = Wnorm**2 * sigma_p * cn
!    CQ_e = Wnorm**2 * sigma_p * (ct*cos(chi) - cn*sin(chi)*cos(azimuth))
!
!    if (a < 0.4) then
!        CT_m = 4*a*F*sqrt(1 - a*(2*cos(chi0) - a))
!
!!         CT_m = 4*a*F*(cos(chi0) + tan(chi/2.0)*sin(chi0) - a/(cos(chi/2.0)**2))
!
!    else if (a < 1.0) then
!        CT_f = 4*0.4*F*sqrt(1 - 0.4*(2*cos(chi0) - 0.4))
!        CT_fp = 2*0.4*F/sqrt(1 - 0.4*(2*cos(chi0) - 0.4))*(-2*cos(chi0) + 2*0.4) + &
!                sqrt(1 - 0.4*(2*cos(chi0) - 0.4))*4*F
!
!!         CT_f = 4*0.4*F*(cos(chi0) + tan(chi/2.0)*sin(chi0) - 0.4/(cos(chi/2.0)**2))
!!         CT_fp = 4*0.4*F*(-1.0/(cos(chi/2.0)**2)) + (cos(chi0) + tan(chi/2.0)*sin(chi0) - 0.4/(cos(chi/2.0)**2))*4*F
!
!        k2 = (2*cos(chi0) - CT_f - CT_fp*(1-0.4)) / ((1-0.4)**2)
!        k1 = CT_fp - 2*k2*0.4
!        k0 = 2*cos(chi0) - k1 - k2
!
!        CT_m = k0 + k1*a + k2*a**2
!
!    else
!        CT_m = 4*a*F*(a - 1)*cos(chi0)
!
!    end if
!     CQ_m = 4*Vy/Vx*ap*F*(cos(chi0) - a*F)*(cos(azimuth)**2 + cos(chi)**2*sin(azimuth)**2)
  !  CQ_m = 4*Vy/Vx*ap*F*(cos(chi0) - a)*(cos(azimuth)**2 + cos(chi)**2*sin(azimuth)**2)
    !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    R1 = CT_e - CT_m
    R2 = CQ_e - CQ_m

end subroutine BEMTC_ElementalInduction
                              
                              
                              
                              
                              
                              
                              
                              
end module BEMTCoupled