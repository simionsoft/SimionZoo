module Brent
   


   use NWTC_Library
   use BEMTUncoupled
   use BEMT_Types
   implicit none
   private
   public :: BrentRoots
   
   contains

!
!                              
! http://jean-pierre.moreau.pagesperso-orange.fr/
!      
                              
                              
                              
! TRUE if x1*x2 negative
integer function RootBracketed(x1,x2)
  real(ReKi), intent(in) :: x1,x2 
  if ((x1 > 0.and.x2 > 0).or.(x1 < 0.and.x2 < 0)) then 
    RootBracketed = 0
  else
    RootBracketed = 1
  endif
END FUNCTION RootBracketed


! returns the minimum of two real numbers
real(ReKi) function Minimum(x1,x2) 
  REAL(ReKi), INTENT(IN) :: x1,x2
  
  if (x1 < x2) then
    Minimum = x1
  else 
    Minimum = x2
  end if
end Function Minimum

!*****************************************************
!*              Brent Method Function                *
!* ------------------------------------------------- *
!* The purpose is to find a real root of a real      *
!* function f(x) using Brent method.                 *
!*                                                   *
!* INPUTS:  x1,x2     : interval of root             *
!*          Tolerance : desired accuracy for root    *
!*          maxIter   : maximum number of iterations *
!*                                                   *
!* OUTPUTS: The function returns the root value      *
!*          ValueAtRoot : value of f(root)           *
!*          niter    : number of done iterations     *
!*          error    : =0, all OK                    *
!*                   : =1, no root found in interval *
!*                   : =2, no more iterations !      *
!*****************************************************
real(ReKi) function BrentRoots( x1, x2, Tolerance, maxIndIterations, valueAtRoot, niter, psi, chi0, numReIterations, airDens, mu, numBlades, rlocal, rtip, chord, theta,  AFInfo, &
                              Vx, Vy, useTanInd, useAIDrag, useTIDrag, useHubLoss, useTipLoss, hubLossConst, tipLossConst, SkewWakeMod, &
                              UA_Flag, p_UA, xd_UA, OtherState_UA, &
                              ErrStat, ErrMsg)  

!TODO: Need to establish better FPP and nearzero values based on ReKi GJH 7/17/2014 DO NOT RELEASE WITHOUT FIXING THIS!!!!!!
   REAL(DbKi), PARAMETER  :: FPP = epsilon(1.0_ReKi) !1.d-6
   REAL(DbKi), PARAMETER  :: nearzero = 1.d-6

   REAL(ReKi),             INTENT(IN   ) :: x1,x2,Tolerance
   INTEGER,                INTENT(IN   ) :: maxIndIterations
   REAL(ReKi),             INTENT(  OUT) :: valueAtRoot                 ! the actual residual at resultant
   INTEGER,                INTENT(  OUT) :: niter                       ! number of iterations performed
   REAL(ReKi),             INTENT(IN   ) :: psi
   REAL(ReKi),             INTENT(IN   ) :: chi0
   INTEGER,                INTENT(IN   ) :: numReIterations
   REAL(ReKi),             INTENT(IN   ) :: airDens
   REAL(ReKi),             INTENT(IN   ) :: mu
   INTEGER,                INTENT(IN   ) :: numBlades
   REAL(ReKi),             INTENT(IN   ) :: rlocal  
   REAL(ReKi),             INTENT(IN   ) :: rtip
   REAL(ReKi),             INTENT(IN   ) :: chord 
   REAL(ReKi),             INTENT(IN   ) :: theta         
   TYPE(AFInfoType),       INTENT(IN   ) :: AFInfo
   REAL(ReKi),             INTENT(IN   ) :: Vx
   REAL(ReKi),             INTENT(IN   ) :: Vy
   LOGICAL,                INTENT(IN   ) :: useTanInd 
   LOGICAL,                INTENT(IN   ) :: useAIDrag
   LOGICAL,                INTENT(IN   ) :: useTIDrag
   LOGICAL,                INTENT(IN   ) :: useHubLoss
   LOGICAL,                INTENT(IN   ) :: useTipLoss
   REAL(ReKi),             INTENT(IN   ) :: hubLossConst
   REAL(ReKi),             INTENT(IN   ) :: tipLossConst 
   INTEGER,                INTENT(IN   ) :: SkewWakeMod   ! Skewed wake model
   LOGICAL,                intent(in   ) :: UA_Flag
   TYPE(UA_ParameterType),       INTENT(IN   ) :: p_UA           ! Parameters
   TYPE(UA_DiscreteStateType),   INTENT(IN   ) :: xd_UA          ! Discrete states at Time
   TYPE(UA_OtherStateType),      INTENT(IN   ) :: OtherState_UA  ! Other/optimization states 
   INTEGER(IntKi),         INTENT(  OUT) :: ErrStat       ! Error status of the operation
   CHARACTER(*),           INTENT(  OUT) :: ErrMsg        ! Error message if ErrStat /= ErrID_None
  
   
   REAL(ReKi) resultant, AA, BB, CC, DD, EE, FA, FB, FC, Tol1, PP, QQ, RR, SS, xm
   INTEGER i, done

  ErrStat = ErrID_None
  ErrMsg  = ""
  resultant = 0.0_ReKi 
  BrentRoots = 0.0_ReKi
  i = 0; done = 0;
  AA = x1
  BB = x2
  FA = UncoupledErrFn(AA, psi, chi0, numReIterations, airDens, mu, numBlades, rlocal, rtip, chord, theta,  AFInfo, &
                              Vx, Vy, useTanInd, useAIDrag, useTIDrag, useHubLoss, useTipLoss, hubLossConst, tipLossConst, SkewWakeMod, &
                              UA_Flag, p_UA, xd_UA, OtherState_UA, &
                              ErrStat, ErrMsg)
  FB = UncoupledErrFn(BB, psi, chi0, numReIterations, airDens, mu, numBlades, rlocal, rtip, chord, theta, AFInfo, &
                              Vx, Vy, useTanInd, useAIDrag, useTIDrag, useHubLoss, useTipLoss, hubLossConst, tipLossConst, SkewWakeMod, &
                              UA_Flag, p_UA, xd_UA, OtherState_UA, &
                              ErrStat, ErrMsg)
  
  if (RootBracketed(FA,FB).eq.0) then 
    ErrStat = ErrID_Warn
    ErrMsg  = 'The supplied brackets contain zero!  Cannot use root finding method!'
    return
  else 
    FC = FB;
    do while (done .eq. 0 .and. i < maxIndIterations)
      
       
      if (RootBracketed(FC,FB) .eq. 0) then
        CC = AA; FC = FA; DD = BB - AA; EE = DD
      endif
      if (abs(FC) < abs(FB)) then
        AA = BB; BB = CC; CC = AA
        FA = FB; FB = FC; FC = FA
      endif
      Tol1 = 2.0 * FPP * abs(BB) + 0.5 * Tolerance
      xm = 0.5 * (CC-BB)
      if ((abs(xm) <= Tol1) .or. (abs(FA) < nearzero)) then
        ! A root has been found
        resultant = BB;
        done = 1
        valueAtRoot = UncoupledErrFn(BB, psi, chi0, numReIterations, airDens, mu, numBlades, rlocal, rtip, chord, theta,  AFInfo, &
                              Vx, Vy, useTanInd, useAIDrag, useTIDrag, useHubLoss, useTipLoss, hubLossConst, tipLossConst, SkewWakeMod, &
                              UA_Flag, p_UA, xd_UA, OtherState_UA, &
                              ErrStat, ErrMsg)
      else 
        if ((abs(EE) >= Tol1) .and. (abs(FA) > abs(FB))) then
          SS = FB/ FA;
          if (abs(AA - CC) < nearzero) then
            PP = 2.0 * xm * SS;
            QQ = 1.0 - SS;
          else 
            QQ = FA/FC;
            RR = FB /FC;
            PP = SS * (2.0 * xm * QQ * (QQ - RR) - (BB-AA) * (RR - 1.0));
            QQ = (QQ - 1.0) * (RR - 1.0) * (SS - 1.0);
          endif
          if (PP > nearzero) QQ = -QQ;
          PP = abs(PP);
          if ((2.0 * PP) < Minimum(3.0*xm *QQ-abs(Tol1 * QQ), abs(EE * QQ))) then
            EE = DD;  DD = PP/QQ;
          else 
            DD = xm;   EE = DD;
          endif
        else 
          DD = xm;
          EE = DD;
        endif
        AA = BB;
        FA = FB;
        if (abs(DD) > Tol1) then 
          BB = BB + DD;
        else 
          if (xm > 0) then 
            BB = BB + abs(Tol1)
          else 
            BB = BB - abs(Tol1)
          endif
        endif
      !---------------------------------------------------------- 
      ! DEBUG SECTION ONLY --------------------------------------
      !if (AA < x1 .OR. AA > x2 .OR. BB < x1 .OR. BB > x2 ) then
      !   ErrStat = ErrID_FATAL
      !   ErrMsg  = 'Cannot use root finding method!'
      !   return
      !end if
      !----------------------------------------------------------
      
        FB = UncoupledErrFn(BB, psi, chi0, numReIterations, airDens, mu, numBlades, rlocal, rtip, chord, theta, AFInfo, &
                              Vx, Vy, useTanInd, useAIDrag, useTIDrag, useHubLoss, useTipLoss, hubLossConst, tipLossConst, SkewWakeMod, &
                              UA_Flag, p_UA, xd_UA, OtherState_UA, &
                              ErrStat, ErrMsg)
        i=i+1
      endif
   end do
   if (i >= maxIndIterations) then
      ErrStat = ErrID_Fatal
      ErrMsg  = 'Too many iterations! No solution found to induction factor solve.'
      return
   end if
      
  endif
  niter = i
  BrentRoots = resultant
end function BrentRoots   
   
end module Brent