module nwtc_bemt_minpack
   
   use NWTC_Library
   use AirFoilInfo_Types
   use BEMTCoupled
   use UnsteadyAero_Types
   
   implicit none
   
   private
   
   type, public :: hybrd_fcnArgs
 
      real(ReKi)                 :: psi
      real(ReKi)                 :: chi0
      real(ReKi)                 :: airDens
      real(ReKi)                 :: mu
      integer                    :: numBlades
      real(ReKi)                 :: rlocal   
      real(ReKi)                 :: rtip   
      real(ReKi)                 :: chord 
      real(ReKi)                 :: theta         
      integer                    :: AFindx
      type(AFInfoType)           :: AFInfo
      real(ReKi)                 :: Vx
      real(ReKi)                 :: Vy
      real(ReKi)                 :: Vinf
      logical                    :: useTanInd 
      logical                    :: useAIDrag
      logical                    :: useTIDrag
      logical                    :: useHubLoss
      logical                    :: useTipLoss
      real(ReKi)                 :: hubLossConst
      real(ReKi)                 :: tipLossConst
      integer(IntKi)             :: SkewWakeMod
      logical                    :: UA_Flag
      type(UA_ParameterType)     :: p_UA           ! Parameters
      type(UA_DiscreteStateType) :: xd_UA          ! Discrete states at Time
      type(UA_OtherStateType)    :: OtherState_UA  ! Other/optimization states
      integer(IntKi)             :: errStat        ! Error status of the operation
      character(ErrMsgLen)       :: errMsg         ! Error message if ErrStat /= ErrID_None
   end type hybrd_fcnArgs
   
   public :: NWTC_MINPACK_hybrd1
   
   
   
contains

subroutine fcn(n, x, fvec, iflag, fcnArgs)
   integer   ,           intent(in   )   :: n
   integer   ,           intent(  out)   :: iflag
   real(ReKi),           intent(in   )   ::  x(n)
   real(ReKi),           intent(  out)   :: fvec(n)
   type(hybrd_fcnArgs),  intent(inout)   :: fcnArgs
   
   
   integer(IntKi)       :: errStat       ! Error status of the operation
   character(ErrMsgLen) :: errMsg        ! Error message if ErrStat /= ErrID_None
   iflag = 0
   
   ! Call the BEMTC_ElementalErrFn subroutine to compute the residual
   fvec = BEMTC_ElementalErrFn( x(1), x(2), fcnArgs%psi, fcnArgs%chi0, fcnArgs%airDens, fcnArgs%mu, fcnArgs%numBlades, fcnArgs%rlocal, fcnArgs%rtip, fcnArgs%chord, fcnArgs%theta,  fcnArgs%AFInfo, &
                              fcnArgs%Vx, fcnArgs%Vy, fcnArgs%Vinf, fcnArgs%useTanInd, fcnArgs%useAIDrag, fcnArgs%useTIDrag, fcnArgs%useHubLoss, fcnArgs%useTipLoss,  fcnArgs%hubLossConst, fcnArgs%tipLossConst, fcnArgs%SkewWakeMod, &
                              fcnArgs%UA_Flag, fcnArgs%p_UA, fcnArgs%xd_UA, fcnArgs%OtherState_UA, &
                              errStat, errMsg)
   
   
   
   fcnArgs%errStat = errStat
   fcnArgs%errMsg  = errMsg
      
   if (ErrStat /= ErrID_None) then
      iflag = 1
      
   end if
   
end subroutine fcn

!..............................................................................
! The following subroutines are minpack routines hybrd1.f and its dependencies.
! These routines have been modified for free-format fortran 90 and to use 
! NWTC_Library precision parameters for single/double precision. The routines
! call the BEMT fcn routine above.
! It should also be noted that spmpar.f and dpmpar.f have been replaced with calls
! to epsilon() and huge(). calls to specific max() and min() routines have been
! replaced with the generic versions as well.
!..............................................................................

   subroutine dogleg(n,r,lr,diag,qtb,delta,x,wa1,wa2)
      integer n,lr
      real(ReKi) delta
      real(ReKi) r(lr),diag(n),qtb(n),x(n),wa1(n),wa2(n)
!     **********
!
!     subroutine dogleg
!
!     given an m by n matrix a, an n by n nonsingular diagonal
!     matrix d, an m-vector b, and a positive number delta, the
!     problem is to determine the convex combination x of the
!     gauss-newton and scaled gradient directions that minimizes
!     (a*x - b) in the least squares sense, subject to the
!     restriction that the euclidean norm of d*x be at most delta.
!
!     this subroutine completes the solution of the problem
!     if it is provided with the necessary information from the
!     qr factorization of a. that is, if a = q*r, where q has
!     orthogonal columns and r is an upper triangular matrix,
!     then dogleg expects the full upper triangle of r and
!     the first n components of (q transpose)*b.
!
!     the subroutine statement is
!
!       subroutine dogleg(n,r,lr,diag,qtb,delta,x,wa1,wa2)
!
!     where
!
!       n is a positive integer input variable set to the order of r.
!
!       r is an input array of length lr which must contain the upper
!         triangular matrix r stored by rows.
!
!       lr is a positive integer input variable not less than
!         (n*(n+1))/2.
!
!       diag is an input array of length n which must contain the
!         diagonal elements of the matrix d.
!
!       qtb is an input array of length n which must contain the first
!         n elements of the vector (q transpose)*b.
!
!       delta is a positive input variable which specifies an upper
!         bound on the euclidean norm of d*x.
!
!       x is an output array of length n which contains the desired
!         convex combination of the gauss-newton direction and the
!         scaled gradient direction.
!
!       wa1 and wa2 are work arrays of length n.
!
!     subprograms called
!
!       minpack-supplied ... enorm
!
!       fortran-supplied ... abs,max,min,sqrt,epsilon
!
!     argonne national laboratory. minpack project. march 1980.
!     burton s. garbow, kenneth e. hillstrom, jorge j. more
!
!     **********
      integer i,j,jj,jp1,k,l
      real(ReKi) alpha,bnorm,epsmch,gnorm,one,qnorm,sgnorm,sum,temp,zero
      data one,zero /1.0_ReKi,0.0_ReKi/
!
!     epsmch is the machine precision.
!
      epsmch = EPSILON(epsmch)  ! spmpar(1)
!
!     first, calculate the gauss-newton direction.
!
      jj = (n*(n + 1))/2 + 1
      do 50 k = 1, n
         j = n - k + 1
         jp1 = j + 1
         jj = jj - k
         l = jj + 1
         sum = zero
         if (n .lt. jp1) go to 20
         do 10 i = jp1, n
            sum = sum + r(l)*x(i)
            l = l + 1
   10       continue
   20    continue
         temp = r(jj)
         if (temp .ne. zero) go to 40
         l = j
         do 30 i = 1, j
            temp = max(temp,abs(r(l)))
            l = l + n - i
   30       continue
         temp = epsmch*temp
         if (temp .eq. zero) temp = epsmch
   40    continue
         x(j) = (qtb(j) - sum)/temp
   50    continue
!
!     test whether the gauss-newton direction is acceptable.
!
      do 60 j = 1, n
         wa1(j) = zero
         wa2(j) = diag(j)*x(j)
   60    continue
      qnorm = enorm(n,wa2)
      if (qnorm .le. delta) go to 140
!
!     the gauss-newton direction is not acceptable.
!     next, calculate the scaled gradient direction.
!
      l = 1
      do 80 j = 1, n
         temp = qtb(j)
         do 70 i = j, n
            wa1(i) = wa1(i) + r(l)*temp
            l = l + 1
   70       continue
         wa1(j) = wa1(j)/diag(j)
   80    continue
!
!     calculate the norm of the scaled gradient and test for
!     the special case in which the scaled gradient is zero.
!
      gnorm = enorm(n,wa1)
      sgnorm = zero
      alpha = delta/qnorm
      if (gnorm .eq. zero) go to 120
!
!     calculate the point along the scaled gradient
!     at which the quadratic is minimized.
!
      do 90 j = 1, n
         wa1(j) = (wa1(j)/gnorm)/diag(j)
   90    continue
      l = 1
      do 110 j = 1, n
         sum = zero
         do 100 i = j, n
            sum = sum + r(l)*wa1(i)
            l = l + 1
  100       continue
         wa2(j) = sum
  110    continue
      temp = enorm(n,wa2)
      sgnorm = (gnorm/temp)/temp
!
!     test whether the scaled gradient direction is acceptable.
!
      alpha = zero
      if (sgnorm .ge. delta) go to 120
!
!     the scaled gradient direction is not acceptable.
!     finally, calculate the point along the dogleg
!     at which the quadratic is minimized.
!
      bnorm = enorm(n,qtb)
      temp = (bnorm/gnorm)*(bnorm/qnorm)*(sgnorm/delta)
      temp = temp - (delta/qnorm)*(sgnorm/delta)**2   &
             + sqrt((temp-(delta/qnorm))**2           &
                    +(one-(delta/qnorm)**2)*(one-(sgnorm/delta)**2))
      alpha = ((delta/qnorm)*(one - (sgnorm/delta)**2))/temp
  120 continue
!
!     form appropriate convex combination of the gauss-newton
!     direction and the scaled gradient direction.
!
      temp = (one - alpha)*min(sgnorm,delta)
      do 130 j = 1, n
         x(j) = temp*wa1(j) + alpha*x(j)
  130    continue
  140 continue
      return
!
!     last card of subroutine dogleg.
!
      end subroutine dogleg
!..............................................................................
   real(ReKi) function enorm(n,x)
      
      integer,    intent(in   ) :: n
      real(ReKi), intent(in   ) :: x(n)
!     **********
!
!     function enorm
!
!     given an n-vector x, this function calculates the
!     euclidean norm of x.
!
!     the euclidean norm is computed by accumulating the sum of
!     squares in three different sums. the sums of squares for the
!     small and large components are scaled so that no overflows
!     occur. non-destructive underflows are permitted. underflows
!     and overflows do not occur in the computation of the unscaled
!     sum of squares for the intermediate components.
!     the definitions of small, intermediate and large components
!     depend on two constants, rdwarf and rgiant. the main
!     restrictions on these constants are that rdwarf**2 not
!     underflow and rgiant**2 not overflow. the constants
!     given here are suitable for every known computer.
!
!     the function statement is
!
!       real(ReKi) function enorm(n,x)
!
!     where
!
!       n is a positive integer input variable.
!
!       x is an input array of length n.
!
!     subprograms called
!
!       fortran-supplied ... abs,sqrt
!
!     argonne national laboratory. minpack project. march 1980.
!     burton s. garbow, kenneth e. hillstrom, jorge j. more
!
!     **********
      integer i
      real(ReKi) agiant,floatn,one,rdwarf,rgiant,s1,s2,s3,xabs,x1max,x3max, zero
      data one,zero,rdwarf,rgiant /1.0_ReKi,0.0_ReKi,3.834e-20,1.304e19/  !bjj: those "e" values should be "d" values for ReKi=8
      s1 = zero
      s2 = zero
      s3 = zero
      x1max = zero
      x3max = zero
      floatn = n
      agiant = rgiant/floatn
      do 90 i = 1, n
         xabs = abs(x(i))
         if (xabs .gt. rdwarf .and. xabs .lt. agiant) go to 70
            if (xabs .le. rdwarf) go to 30
!
!              sum for large components.
!
               if (xabs .le. x1max) go to 10
                  s1 = one + s1*(x1max/xabs)**2
                  x1max = xabs
                  go to 20
   10          continue
                  s1 = s1 + (xabs/x1max)**2
   20          continue
               go to 60
   30       continue
!
!              sum for small components.
!
               if (xabs .le. x3max) go to 40
                  s3 = one + s3*(x3max/xabs)**2
                  x3max = xabs
                  go to 50
   40          continue
                  if (xabs .ne. zero) s3 = s3 + (xabs/x3max)**2
   50          continue
   60       continue
            go to 80
   70    continue
!
!           sum for intermediate components.
!
            s2 = s2 + xabs**2
   80    continue
   90    continue
!
!     calculation of norm.
!
      if (s1 .eq. zero) go to 100
         enorm = x1max*sqrt(s1+(s2/x1max)/x1max)
         go to 130
  100 continue
         if (s2 .eq. zero) go to 110
            if (s2 .ge. x3max) enorm = sqrt(s2*(one+(x3max/s2)*(x3max*s3)))
            if (s2 .lt. x3max) enorm = sqrt(x3max*((s2/x3max)+(x3max*s3)))
            go to 120
  110    continue
            enorm = x3max*sqrt(s3)
  120    continue
  130 continue
      return
!
!     last card of function enorm.
!
      end function enorm
!..............................................................................
   subroutine fdjac1( n, x,fvec,fjac,ldfjac,iflag,ml,mu,epsfcn, wa1,wa2, fcnArgs)

      integer,               intent(in   ) :: n, ldfjac,  ml, mu
      integer,               intent(inout) :: iflag
      real(ReKi),            intent(in   ) :: epsfcn
      real(ReKi),            intent(in   ) :: fvec(n)
      real(ReKi),            intent(inout) :: x(n), fjac(ldfjac,n), wa1(n), wa2(n)
      type(hybrd_fcnArgs),   intent(inout) :: fcnArgs     ! inout because calling routine may set  data (errStat and errMsg)
      
      
!     **********
!
!     subroutine fdjac1
!
!     this subroutine computes a forward-difference approximation
!     to the n by n jacobian matrix associated with a specified
!     problem of n functions in n variables. if the jacobian has
!     a banded form, then function evaluations are saved by only
!     approximating the nonzero terms.
!
!     the subroutine statement is
!
!       subroutine fdjac1(n,x,fvec,fjac,ldfjac,iflag,ml,mu,epsfcn,
!                         wa1,wa2, fcnArgs)
!
!     where
!
!       fcn is the name of the user-supplied subroutine which
!         calculates the functions. fcn must be declared
!         in an external statement in the user calling
!         program, and should be written as follows.
!
!         subroutine fcn(n,x,fvec,iflag)
!         integer n,iflag
!         real(ReKi) x(n),fvec(n)
!         ----------
!         calculate the functions at x and
!         return this vector in fvec.
!         ----------
!         return
!         end
!
!         the value of iflag should not be changed by fcn unless
!         the user wants to terminate execution of fdjac1.
!         in this case set iflag to a negative integer.
!
!       n is a positive integer input variable set to the number
!         of functions and variables.
!
!       x is an input array of length n.
!
!       fvec is an input array of length n which must contain the
!         functions evaluated at x.
!
!       fjac is an output n by n array which contains the
!         approximation to the jacobian matrix evaluated at x.
!
!       ldfjac is a positive integer input variable not less than n
!         which specifies the leading dimension of the array fjac.
!
!       iflag is an integer variable which can be used to terminate
!         the execution of fdjac1. see description of fcn.
!
!       ml is a nonnegative integer input variable which specifies
!         the number of subdiagonals within the band of the
!         jacobian matrix. if the jacobian is not banded, set
!         ml to at least n - 1.
!
!       epsfcn is an input variable used in determining a suitable
!         step length for the forward-difference approximation. this
!         approximation assumes that the relative errors in the
!         functions are of the order of epsfcn. if epsfcn is less
!         than the machine precision, it is assumed that the relative
!         errors in the functions are of the order of the machine
!         precision.
!
!       mu is a nonnegative integer input variable which specifies
!         the number of superdiagonals within the band of the
!         jacobian matrix. if the jacobian is not banded, set
!         mu to at least n - 1.
!
!       wa1 and wa2 are work arrays of length n. if ml + mu + 1 is at
!         least n, then the jacobian is considered dense, and wa2 is
!         not referenced.
!
!     subprograms called
!
!       fortran-supplied ... abs,max,sqrt,epsilon
!
!     argonne national laboratory. minpack project. march 1980.
!     burton s. garbow, kenneth e. hillstrom, jorge j. more
!
!     **********
      integer i,j,k,msum
      real(ReKi) eps,epsmch,h,temp,zero
      data zero /0.0_ReKi/
!
!     epsmch is the machine precision.
!
      epsmch = EPSILON(epsmch)  ! spmpar(1)
!
      eps = sqrt(max(epsfcn,epsmch))
      msum = ml + mu + 1
      if (msum .lt. n) go to 40
!
!        computation of dense approximate jacobian.
!
         do 20 j = 1, n
            temp = x(j)
            h = eps*abs(temp)
            if (h .eq. zero) h = eps
            x(j) = temp + h
            call fcn(n,x,wa1, iflag, fcnArgs)
            if (iflag .lt. 0) go to 30
            x(j) = temp
            do 10 i = 1, n
               fjac(i,j) = (wa1(i) - fvec(i))/h
   10          continue
   20       continue
   30    continue
         go to 110
   40 continue
!
!        computation of banded approximate jacobian.
!
         do 90 k = 1, msum
            do 60 j = k, n, msum
               wa2(j) = x(j)
               h = eps*abs(wa2(j))
               if (h .eq. zero) h = eps
               x(j) = wa2(j) + h
   60          continue
            !call fcn(n,x,wa1,iflag)
            call fcn(n, x, wa1, iflag, fcnArgs)
            if (iflag .lt. 0) go to 100
            do 80 j = k, n, msum
               x(j) = wa2(j)
               h = eps*abs(wa2(j))
               if (h .eq. zero) h = eps
               do 70 i = 1, n
                  fjac(i,j) = zero
                  if (i .ge. j - mu .and. i .le. j + ml) fjac(i,j) = (wa1(i) - fvec(i))/h
   70             continue
   80          continue
   90       continue
  100    continue
  110 continue
      return
!
!     last card of subroutine fdjac1.
!
   end subroutine fdjac1
!..............................................................................
   subroutine hybrd( n, x,fvec,xtol,maxfev,ml,mu,epsfcn,diag, &
                         mode,factor,nprint,info,nfev,fjac,ldfjac,r,lr, &
                         qtf,wa1,wa2,wa3,wa4, fcnArgs)
      
      integer n,maxfev,ml,mu,mode,nprint,info,nfev,ldfjac,lr
      real(ReKi) xtol,epsfcn,factor
      real(ReKi) x(n),fvec(n),diag(n),fjac(ldfjac,n),r(lr),qtf(n),wa1(n), wa2(n),wa3(n),wa4(n)
      type(hybrd_fcnArgs), intent(inout) ::  fcnArgs  ! inout because calling routine may set  data (errStat and errMsg)
      
!     **********
!
!     subroutine hybrd
!
!     the purpose of hybrd is to find a zero of a system of
!     n nonlinear functions in n variables by a modification
!     of the powell hybrid method. the user must provide a
!     subroutine which calculates the functions. the jacobian is
!     then calculated by a forward-difference approximation.
!
!     the subroutine statement is
!
!       subroutine hybrd(fcn,n,x,fvec,xtol,maxfev,ml,mu,epsfcn,
!                        diag,mode,factor,nprint,info,nfev,fjac,
!                        ldfjac,r,lr,qtf,wa1,wa2,wa3,wa4, fcnArgs)
!
!     where
!
!       fcn is the name of the user-supplied subroutine which
!         calculates the functions. fcn must be declared
!         in an external statement in the user calling
!         program, and should be written as follows.
!
!         subroutine fcn(n,x,fvec,iflag, fcnArgs)
!         integer n,iflag
!         real(ReKi) x(n),fvec(n)
!         ----------
!         calculate the functions at x and
!         return this vector in fvec.
!         ---------
!         return
!         end
!
!         the value of iflag should not be changed by fcn unless
!         the user wants to terminate execution of hybrd.
!         in this case set iflag to a negative integer.
!
!       n is a positive integer input variable set to the number
!         of functions and variables.
!
!       x is an array of length n. on input x must contain
!         an initial estimate of the solution vector. on output x
!         contains the final estimate of the solution vector.
!
!       fvec is an output array of length n which contains
!         the functions evaluated at the output x.
!
!       xtol is a nonnegative input variable. termination
!         occurs when the relative error between two consecutive
!         iterates is at most xtol.
!
!       maxfev is a positive integer input variable. termination
!         occurs when the number of calls to fcn is at least maxfev
!         by the end of an iteration.
!
!       ml is a nonnegative integer input variable which specifies
!         the number of subdiagonals within the band of the
!         jacobian matrix. if the jacobian is not banded, set
!         ml to at least n - 1.
!
!       mu is a nonnegative integer input variable which specifies
!         the number of superdiagonals within the band of the
!         jacobian matrix. if the jacobian is not banded, set
!         mu to at least n - 1.
!
!       epsfcn is an input variable used in determining a suitable
!         step length for the forward-difference approximation. this
!         approximation assumes that the relative errors in the
!         functions are of the order of epsfcn. if epsfcn is less
!         than the machine precision, it is assumed that the relative
!         errors in the functions are of the order of the machine
!         precision.
!
!       diag is an array of length n. if mode = 1 (see
!         below), diag is internally set. if mode = 2, diag
!         must contain positive entries that serve as
!         multiplicative scale factors for the variables.
!
!       mode is an integer input variable. if mode = 1, the
!         variables will be scaled internally. if mode = 2,
!         the scaling is specified by the input diag. other
!         values of mode are equivalent to mode = 1.
!
!       factor is a positive input variable used in determining the
!         initial step bound. this bound is set to the product of
!         factor and the euclidean norm of diag*x if nonzero, or else
!         to factor itself. in most cases factor should lie in the
!         interval (.1,100.). 100. is a generally recommended value.
!
!       nprint is an integer input variable that enables controlled
!         printing of iterates if it is positive. in this case,
!         fcn is called with iflag = 0 at the beginning of the first
!         iteration and every nprint iterations thereafter and
!         immediately prior to return, with x and fvec available
!         for printing. if nprint is not positive, no special calls
!         of fcn with iflag = 0 are made.
!
!       info is an integer output variable. if the user has
!         terminated execution, info is set to the (negative)
!         value of iflag. see description of fcn. otherwise,
!         info is set as follows.
!
!         info = 0   improper input parameters.
!
!         info = 1   relative error between two consecutive iterates
!                    is at most xtol.
!
!         info = 2   number of calls to fcn has reached or exceeded
!                    maxfev.
!
!         info = 3   xtol is too small. no further improvement in
!                    the approximate solution x is possible.
!
!         info = 4   iteration is not making good progress, as
!                    measured by the improvement from the last
!                    five jacobian evaluations.
!
!         info = 5   iteration is not making good progress, as
!                    measured by the improvement from the last
!                    ten iterations.
!
!       nfev is an integer output variable set to the number of
!         calls to fcn.
!
!       fjac is an output n by n array which contains the
!         orthogonal matrix q produced by the qr factorization
!         of the final approximate jacobian.
!
!       ldfjac is a positive integer input variable not less than n
!         which specifies the leading dimension of the array fjac.
!
!       r is an output array of length lr which contains the
!         upper triangular matrix produced by the qr factorization
!         of the final approximate jacobian, stored rowwise.
!
!       lr is a positive integer input variable not less than
!         (n*(n+1))/2.
!
!       qtf is an output array of length n which contains
!         the vector (q transpose)*fvec.
!
!       wa1, wa2, wa3, and wa4 are work arrays of length n.
!
!     subprograms called
!
!       user-supplied ...... fcn
!
!       minpack-supplied ... dogleg,enorm,fdjac1,
!                            qform,qrfac,r1mpyq,r1updt
!
!       fortran-supplied ... abs,max,min,min,mod,epsilon
!
!     argonne national laboratory. minpack project. march 1980.
!     burton s. garbow, kenneth e. hillstrom, jorge j. more
!
!     **********
      integer i,iflag,iter,j,jm1,l,msum,ncfail,ncsuc,nslow1,nslow2
      integer iwa(1)
      logical jeval,sing
      real(ReKi) actred,delta,epsmch,fnorm,fnorm1,one,pnorm,prered,p1,p5, p001,p0001,ratio,sum,temp,xnorm,zero
      data one,p1,p5,p001,p0001,zero  /1.0_ReKi,1.0e-1,5.0e-1,1.0e-3,1.0e-4,0.0_ReKi/
!
!     epsmch is the machine precision.
!
      epsmch = EPSILON(epsmch)  ! spmpar(1)
!
      info = 0
      iflag = 0
      nfev = 0
!
!     check the input parameters for errors.
!
      if (n .le. 0 .or. xtol .lt. zero .or. maxfev .le. 0          &
          .or. ml .lt. 0 .or. mu .lt. 0 .or. factor .le. zero      &
          .or. ldfjac .lt. n .or. lr .lt. (n*(n + 1))/2) go to 300
      if (mode .ne. 2) go to 20
      do 10 j = 1, n
         if (diag(j) .le. zero) go to 300
   10    continue
   20 continue
!
!     evaluate the function at the starting point
!     and calculate its norm.
!
      iflag = 1
      call fcn(n, x, fvec, iflag, fcnArgs)
      !call fcn(n,x,fvec,iflag)
      nfev = 1
      if (iflag .lt. 0) go to 300
      fnorm = enorm(n,fvec)
!
!     determine the number of calls to fcn needed to compute
!     the jacobian matrix.
!
      msum = min(ml+mu+1,n)
!
!     initialize iteration counter and monitors.
!
      iter = 1
      ncsuc = 0
      ncfail = 0
      nslow1 = 0
      nslow2 = 0
!
!     beginning of the outer loop.
!
   30 continue
         jeval = .true.
!
!        calculate the jacobian matrix.
!
         iflag = 2
         call fdjac1( n,x,fvec,fjac,ldfjac,iflag,ml,mu,epsfcn,wa1, wa2, fcnArgs )
         nfev = nfev + msum
         if (iflag .lt. 0) go to 300
!
!        compute the qr factorization of the jacobian.
!
         call qrfac(n,n,fjac,ldfjac,.false.,iwa,1,wa1,wa2,wa3)
!
!        on the first iteration and if mode is 1, scale according
!        to the norms of the columns of the initial jacobian.
!
         if (iter .ne. 1) go to 70
         if (mode .eq. 2) go to 50
         do 40 j = 1, n
            diag(j) = wa2(j)
            if (wa2(j) .eq. zero) diag(j) = one
   40       continue
   50    continue
!
!        on the first iteration, calculate the norm of the scaled x
!        and initialize the step bound delta.
!
         do 60 j = 1, n
            wa3(j) = diag(j)*x(j)
   60       continue
         xnorm = enorm(n,wa3)
         delta = factor*xnorm
         if (delta .eq. zero) delta = factor
   70    continue
!
!        form (q transpose)*fvec and store in qtf.
!
         do 80 i = 1, n
            qtf(i) = fvec(i)
   80       continue
         do 120 j = 1, n
            if (fjac(j,j) .eq. zero) go to 110
            sum = zero
            do 90 i = j, n
               sum = sum + fjac(i,j)*qtf(i)
   90          continue
            temp = -sum/fjac(j,j)
            do 100 i = j, n
               qtf(i) = qtf(i) + fjac(i,j)*temp
  100          continue
  110       continue
  120       continue
!
!        copy the triangular factor of the qr factorization into r.
!
         sing = .false.
         do 150 j = 1, n
            l = j
            jm1 = j - 1
            if (jm1 .lt. 1) go to 140
            do 130 i = 1, jm1
               r(l) = fjac(i,j)
               l = l + n - i
  130          continue
  140       continue
            r(l) = wa1(j)
            if (wa1(j) .eq. zero) sing = .true.
  150       continue
!
!        accumulate the orthogonal factor in fjac.
!
         call qform(n,n,fjac,ldfjac,wa1)
!
!        rescale if necessary.
!
         if (mode .eq. 2) go to 170
         do 160 j = 1, n
            diag(j) = max(diag(j),wa2(j))
  160       continue
  170    continue
!
!        beginning of the inner loop.
!
  180    continue
!
!           if requested, call fcn to enable printing of iterates.
!
            if (nprint .le. 0) go to 190
            iflag = 0
            if (mod(iter-1,nprint) .eq. 0) then 
               call fcn(n, x, fvec, iflag, fcnArgs)  
               !call fcn(n,x,fvec,iflag)
            end if
            
            if (iflag .lt. 0) go to 300
  190       continue
!
!           determine the direction p.
!
            call dogleg(n,r,lr,diag,qtf,delta,wa1,wa2,wa3)
!
!           store the direction p and x + p. calculate the norm of p.
!
            do 200 j = 1, n
               wa1(j) = -wa1(j)
               wa2(j) = x(j) + wa1(j)
               wa3(j) = diag(j)*wa1(j)
  200          continue
            pnorm = enorm(n,wa3)
!
!           on the first iteration, adjust the initial step bound.
!
            if (iter .eq. 1) delta = min(delta,pnorm)
!
!           evaluate the function at x + p and calculate its norm.
!
            iflag = 1
            call fcn(n, wa2, wa4, iflag, fcnArgs)
            
            nfev = nfev + 1
            if (iflag .lt. 0) go to 300
            fnorm1 = enorm(n,wa4)
!
!           compute the scaled actual reduction.
!
            actred = -one
            if (fnorm1 .lt. fnorm) actred = one - (fnorm1/fnorm)**2
!
!           compute the scaled predicted reduction.
!
            l = 1
            do 220 i = 1, n
               sum = zero
               do 210 j = i, n
                  sum = sum + r(l)*wa1(j)
                  l = l + 1
  210             continue
               wa3(i) = qtf(i) + sum
  220          continue
            temp = enorm(n,wa3)
            prered = zero
            if (temp .lt. fnorm) prered = one - (temp/fnorm)**2
!
!           compute the ratio of the actual to the predicted
!           reduction.
!
            ratio = zero
            if (prered .gt. zero) ratio = actred/prered
!
!           update the step bound.
!
            if (ratio .ge. p1) go to 230
               ncsuc = 0
               ncfail = ncfail + 1
               delta = p5*delta
               go to 240
  230       continue
               ncfail = 0
               ncsuc = ncsuc + 1
               if (ratio .ge. p5 .or. ncsuc .gt. 1) delta = max(delta,pnorm/p5)
               if (abs(ratio-one) .le. p1) delta = pnorm/p5
  240       continue
!
!           test for successful iteration.
!
            if (ratio .lt. p0001) go to 260
!
!           successful iteration. update x, fvec, and their norms.
!
            do 250 j = 1, n
               x(j) = wa2(j)
               wa2(j) = diag(j)*x(j)
               fvec(j) = wa4(j)
  250          continue
            xnorm = enorm(n,wa2)
            fnorm = fnorm1
            iter = iter + 1
  260       continue
!
!           determine the progress of the iteration.
!
            nslow1 = nslow1 + 1
            if (actred .ge. p001) nslow1 = 0
            if (jeval) nslow2 = nslow2 + 1
            if (actred .ge. p1) nslow2 = 0
!
!           test for convergence.
!
            if (delta .le. xtol*xnorm .or. fnorm .eq. zero) info = 1
            if (info .ne. 0) go to 300
!
!           tests for termination and stringent tolerances.
!
            if (nfev .ge. maxfev) info = 2
            if (p1*max(p1*delta,pnorm) .le. epsmch*xnorm) info = 3
            if (nslow2 .eq. 5) info = 4
            if (nslow1 .eq. 10) info = 5
            if (info .ne. 0) go to 300
!
!           criterion for recalculating jacobian approximation
!           by forward differences.
!
            if (ncfail .eq. 2) go to 290
!
!           calculate the rank one modification to the jacobian
!           and update qtf if necessary.
!
            do 280 j = 1, n
               sum = zero
               do 270 i = 1, n
                  sum = sum + fjac(i,j)*wa4(i)
  270             continue
               wa2(j) = (sum - wa3(j))/pnorm
               wa1(j) = diag(j)*((diag(j)*wa1(j))/pnorm)
               if (ratio .ge. p0001) qtf(j) = sum
  280          continue
!
!           compute the qr factorization of the updated jacobian.
!
            call r1updt(n,n,r,lr,wa1,wa2,wa3,sing)
            call r1mpyq(n,n,fjac,ldfjac,wa2,wa3)
            call r1mpyq(1,n,qtf,1,wa2,wa3)
!
!           end of the inner loop.
!
            jeval = .false.
            go to 180
  290    continue
!
!        end of the outer loop.
!
         go to 30
  300 continue
!
!     termination, either normal or user imposed.
!
      if (iflag .lt. 0) info = iflag
      iflag = 0
      if (nprint .gt. 0) call fcn(n, x, fvec, iflag, fcnArgs)
         !call fcn(n,x,fvec,iflag)
      return
!
!     last card of subroutine hybrd.
!
   end subroutine hybrd
!..............................................................................                         
   subroutine NWTC_MINPACK_hybrd1( n, x, fvec, tol, info, wa, lwa, fcnArgs )
      
      integer,    intent(in   ) ::  n, lwa
      integer,    intent(  out) ::  info
      real(ReKi), intent(in   ) ::  tol
      real(ReKi), intent(in   ) ::  x(n),fvec(n)
      real(ReKi), intent(inout) ::  wa(lwa)
      type(hybrd_fcnArgs), intent(inout) ::  fcnArgs   ! inout because calling routine may set  data (errStat and errMsg)
      
!     **********
!
!     subroutine hybrd1
!
!     the purpose of hybrd1 is to find a zero of a system of
!     n nonlinear functions in n variables by a modification
!     of the powell hybrid method. this is done by using the
!     more general nonlinear equation solver hybrd. the user
!     must provide a subroutine which calculates the functions.
!     the jacobian is then calculated by a forward-difference
!     approximation.
!
!     the subroutine statement is
!
!       subroutine hybrd1(n,x,fvec,tol,info,wa,lwa, fcnArgs)
!
!     where
!
!       fcn is the name of the user-supplied subroutine which
!         calculates the functions. fcn must be declared
!         in an external statement in the user calling
!         program, and should be written as follows.
!
!         subroutine fcn(n,x,fvec,iflag)
!         integer n,iflag
!         real x(n),fvec(n)
!         ----------
!         calculate the functions at x and
!         return this vector in fvec.
!         ---------
!         return
!         end
!
!         the value of iflag should not be changed by fcn unless
!         the user wants to terminate execution of hybrd1.
!         in this case set iflag to a negative integer.
!
!       n is a positive integer input variable set to the number
!         of functions and variables.
!
!       x is an array of length n. on input x must contain
!         an initial estimate of the solution vector. on output x
!         contains the final estimate of the solution vector.
!
!       fvec is an output array of length n which contains
!         the functions evaluated at the output x.
!
!       tol is a nonnegative input variable. termination occurs
!         when the algorithm estimates that the relative error
!         between x and the solution is at most tol.
!
!       info is an integer output variable. if the user has
!         terminated execution, info is set to the (negative)
!         value of iflag. see description of fcn. otherwise,
!         info is set as follows.
!
!         info = 0   improper input parameters.
!
!         info = 1   algorithm estimates that the relative error
!                    between x and the solution is at most tol.
!
!         info = 2   number of calls to fcn has reached or exceeded
!                    200*(n+1).
!
!         info = 3   tol is too small. no further improvement in
!                    the approximate solution x is possible.
!
!         info = 4   iteration is not making good progress.
!
!       wa is a work array of length lwa.
!
!       lwa is a positive integer input variable not less than
!         (n*(3*n+13))/2.
!
!     subprograms called
!
!       user-supplied ...... fcn
!
!       minpack-supplied ... hybrd
!
!     argonne national laboratory. minpack project. march 1980.
!     burton s. garbow, kenneth e. hillstrom, jorge j. more
!
!     **********
   integer index,j,lr,maxfev,ml,mode,mu,nfev,nprint
   real(ReKi) epsfcn, xtol
   real(ReKi), parameter  :: factor = 100.0_ReKi
   real(ReKi), parameter  :: one    = 1.0_ReKi
   real(ReKi), parameter  :: zero   = 0.0_ReKi
   info = 0
!
!     check the input parameters for errors.
!
   if (.not. (n .le. 0 .or. tol .lt. zero .or. lwa .lt. (n*(3*n + 13))/2) ) then
!
!     call hybrd.
!
      maxfev = 200*(n + 1)
      xtol = tol
      ml = n - 1
      mu = n - 1
      epsfcn = zero
      mode = 2
      do  j = 1, n
         wa(j) = one
      end do
      nprint = 0
      lr = (n*(n + 1))/2
      index = 6*n + lr
      call hybrd( n,x,fvec,xtol,maxfev,ml,mu,epsfcn,wa(1),mode,  &
                factor,nprint,info,nfev,wa(index+1),n,wa(6*n+1),lr, &
            wa(n+1),wa(2*n+1),wa(3*n+1),wa(4*n+1),wa(5*n+1), fcnArgs)
      
      
      if (info .eq. 5) info = 4
   end if
      return
!
!     last card of subroutine hybrd1.
!
   end subroutine NWTC_MINPACK_hybrd1
!..............................................................................
   subroutine qform(m,n,q,ldq,wa)
      
      integer m,n,ldq
      real(ReKi) q(ldq,m),wa(m)
!     **********
!
!     subroutine qform
!
!     this subroutine proceeds from the computed qr factorization of
!     an m by n matrix a to accumulate the m by m orthogonal matrix
!     q from its factored form.
!
!     the subroutine statement is
!
!       subroutine qform(m,n,q,ldq,wa)
!
!     where
!
!       m is a positive integer input variable set to the number
!         of rows of a and the order of q.
!
!       n is a positive integer input variable set to the number
!         of columns of a.
!
!       q is an m by m array. on input the full lower trapezoid in
!         the first min(m,n) columns of q contains the factored form.
!         on output q has been accumulated into a square matrix.
!
!       ldq is a positive integer input variable not less than m
!         which specifies the leading dimension of the array q.
!
!       wa is a work array of length m.
!
!     subprograms called
!
!       fortran-supplied ... min0
!
!     argonne national laboratory. minpack project. march 1980.
!     burton s. garbow, kenneth e. hillstrom, jorge j. more
!
!     **********
      integer i,j,jm1,k,l,minmn,np1
      real(ReKi) one,sum,temp,zero
      data one,zero /1.0_ReKi,0.0_ReKi/
!
!     zero out upper triangle of q in the first min(m,n) columns.
!
      minmn = min(m,n)
      if (minmn .lt. 2) go to 30
      do 20 j = 2, minmn
         jm1 = j - 1
         do 10 i = 1, jm1
            q(i,j) = zero
   10       continue
   20    continue
   30 continue
!
!     initialize remaining columns to those of the identity matrix.
!
      np1 = n + 1
      if (m .lt. np1) go to 60
      do 50 j = np1, m
         do 40 i = 1, m
            q(i,j) = zero
   40       continue
         q(j,j) = one
   50    continue
   60 continue
!
!     accumulate q from its factored form.
!
      do 120 l = 1, minmn
         k = minmn - l + 1
         do 70 i = k, m
            wa(i) = q(i,k)
            q(i,k) = zero
   70       continue
         q(k,k) = one
         if (wa(k) .eq. zero) go to 110
         do 100 j = k, m
            sum = zero
            do 80 i = k, m
               sum = sum + q(i,j)*wa(i)
   80          continue
            temp = sum/wa(k)
            do 90 i = k, m
               q(i,j) = q(i,j) - temp*wa(i)
   90          continue
  100       continue
  110    continue
  120    continue
      return
!
!     last card of subroutine qform.
!
      end subroutine qform
!..............................................................................
   subroutine qrfac(m,n,a,lda,pivot,ipvt,lipvt,rdiag,acnorm,wa)
      
      integer m,n,lda,lipvt
      integer ipvt(lipvt)
      logical pivot
      real(ReKi) a(lda,n),rdiag(n),acnorm(n),wa(n)
!     **********
!
!     subroutine qrfac
!
!     this subroutine uses householder transformations with column
!     pivoting (optional) to compute a qr factorization of the
!     m by n matrix a. that is, qrfac determines an orthogonal
!     matrix q, a permutation matrix p, and an upper trapezoidal
!     matrix r with diagonal elements of nonincreasing magnitude,
!     such that a*p = q*r. the householder transformation for
!     column k, k = 1,2,...,min(m,n), is of the form
!
!                           t
!           i - (1/u(k))*u*u
!
!     where u has zeros in the first k-1 positions. the form of
!     this transformation and the method of pivoting first
!     appeared in the corresponding linpack subroutine.
!
!     the subroutine statement is
!
!       subroutine qrfac(m,n,a,lda,pivot,ipvt,lipvt,rdiag,acnorm,wa)
!
!     where
!
!       m is a positive integer input variable set to the number
!         of rows of a.
!
!       n is a positive integer input variable set to the number
!         of columns of a.
!
!       a is an m by n array. on input a contains the matrix for
!         which the qr factorization is to be computed. on output
!         the strict upper trapezoidal part of a contains the strict
!         upper trapezoidal part of r, and the lower trapezoidal
!         part of a contains a factored form of q (the non-trivial
!         elements of the u vectors described above).
!
!       lda is a positive integer input variable not less than m
!         which specifies the leading dimension of the array a.
!
!       pivot is a logical input variable. if pivot is set true,
!         then column pivoting is enforced. if pivot is set false,
!         then no column pivoting is done.
!
!       ipvt is an integer output array of length lipvt. ipvt
!         defines the permutation matrix p such that a*p = q*r.
!         column j of p is column ipvt(j) of the identity matrix.
!         if pivot is false, ipvt is not referenced.
!
!       lipvt is a positive integer input variable. if pivot is false,
!         then lipvt may be as small as 1. if pivot is true, then
!         lipvt must be at least n.
!
!       rdiag is an output array of length n which contains the
!         diagonal elements of r.
!
!       acnorm is an output array of length n which contains the
!         norms of the corresponding columns of the input matrix a.
!         if this information is not needed, then acnorm can coincide
!         with rdiag.
!
!       wa is a work array of length n. if pivot is false, then wa
!         can coincide with rdiag.
!
!     subprograms called
!
!       minpack-supplied ... enorm
!
!       fortran-supplied ... max,sqrt,min,epsilon
!
!     argonne national laboratory. minpack project. march 1980.
!     burton s. garbow, kenneth e. hillstrom, jorge j. more
!
!     **********
      integer i,j,jp1,k,kmax,minmn
      real(ReKi) ajnorm,epsmch,one,p05,sum,temp,zero
      data one,p05,zero /1.0_ReKi,5.0e-2,0.0_ReKi/
!
!     epsmch is the machine precision.
!
      epsmch = EPSILON(epsmch)  ! spmpar(1)
!
!     compute the initial column norms and initialize several arrays.
!
      do 10 j = 1, n
         acnorm(j) = enorm(m,a(1,j))
         rdiag(j) = acnorm(j)
         wa(j) = rdiag(j)
         if (pivot) ipvt(j) = j
   10    continue
!
!     reduce a to r with householder transformations.
!
      minmn = min(m,n)
      do 110 j = 1, minmn
         if (.not.pivot) go to 40
!
!        bring the column of largest norm into the pivot position.
!
         kmax = j
         do 20 k = j, n
            if (rdiag(k) .gt. rdiag(kmax)) kmax = k
   20       continue
         if (kmax .eq. j) go to 40
         do 30 i = 1, m
            temp = a(i,j)
            a(i,j) = a(i,kmax)
            a(i,kmax) = temp
   30       continue
         rdiag(kmax) = rdiag(j)
         wa(kmax) = wa(j)
         k = ipvt(j)
         ipvt(j) = ipvt(kmax)
         ipvt(kmax) = k
   40    continue
!
!        compute the householder transformation to reduce the
!        j-th column of a to a multiple of the j-th unit vector.
!
         ajnorm = enorm(m-j+1,a(j,j))
         if (ajnorm .eq. zero) go to 100
         if (a(j,j) .lt. zero) ajnorm = -ajnorm
         do 50 i = j, m
            a(i,j) = a(i,j)/ajnorm
   50       continue
         a(j,j) = a(j,j) + one
!
!        apply the transformation to the remaining columns
!        and update the norms.
!
         jp1 = j + 1
         if (n .lt. jp1) go to 100
         do 90 k = jp1, n
            sum = zero
            do 60 i = j, m
               sum = sum + a(i,j)*a(i,k)
   60          continue
            temp = sum/a(j,j)
            do 70 i = j, m
               a(i,k) = a(i,k) - temp*a(i,j)
   70          continue
            if (.not.pivot .or. rdiag(k) .eq. zero) go to 80
            temp = a(j,k)/rdiag(k)
            rdiag(k) = rdiag(k)*sqrt(max(zero,one-temp**2))
            if (p05*(rdiag(k)/wa(k))**2 .gt. epsmch) go to 80
            rdiag(k) = enorm(m-j,a(jp1,k))
            wa(k) = rdiag(k)
   80       continue
   90       continue
  100    continue
         rdiag(j) = -ajnorm
  110    continue
      return
!
!     last card of subroutine qrfac.
!
   end subroutine qrfac
!..............................................................................
   subroutine r1mpyq(m,n,a,lda,v,w)
      
      integer m,n,lda
      real(ReKi) a(lda,n),v(n),w(n)
!     **********
!
!     subroutine r1mpyq
!
!     given an m by n matrix a, this subroutine computes a*q where
!     q is the product of 2*(n - 1) transformations
!
!           gv(n-1)*...*gv(1)*gw(1)*...*gw(n-1)
!
!     and gv(i), gw(i) are givens rotations in the (i,n) plane which
!     eliminate elements in the i-th and n-th planes, respectively.
!     q itself is not given, rather the information to recover the
!     gv, gw rotations is supplied.
!
!     the subroutine statement is
!
!       subroutine r1mpyq(m,n,a,lda,v,w)
!
!     where
!
!       m is a positive integer input variable set to the number
!         of rows of a.
!
!       n is a positive integer input variable set to the number
!         of columns of a.
!
!       a is an m by n array. on input a must contain the matrix
!         to be postmultiplied by the orthogonal matrix q
!         described above. on output a*q has replaced a.
!
!       lda is a positive integer input variable not less than m
!         which specifies the leading dimension of the array a.
!
!       v is an input array of length n. v(i) must contain the
!         information necessary to recover the givens rotation gv(i)
!         described above.
!
!       w is an input array of length n. w(i) must contain the
!         information necessary to recover the givens rotation gw(i)
!         described above.
!
!     subroutines called
!
!       fortran-supplied ... abs,sqrt
!
!     argonne national laboratory. minpack project. march 1980.
!     burton s. garbow, kenneth e. hillstrom, jorge j. more
!
!     **********
      integer i,j,nmj,nm1
      real(ReKi) cos,one,sin,temp
      data one /1.0_ReKi/
!
!     apply the first set of givens rotations to a.
!
      nm1 = n - 1
      if (nm1 .lt. 1) go to 50
      do 20 nmj = 1, nm1
         j = n - nmj
         if (abs(v(j)) .gt. one) cos = one/v(j)
         if (abs(v(j)) .gt. one) sin = sqrt(one-cos**2)
         if (abs(v(j)) .le. one) sin = v(j)
         if (abs(v(j)) .le. one) cos = sqrt(one-sin**2)
         do 10 i = 1, m
            temp   = cos*a(i,j) - sin*a(i,n)
            a(i,n) = sin*a(i,j) + cos*a(i,n)
            a(i,j) = temp
   10       continue
   20    continue
!
!     apply the second set of givens rotations to a.
!
      do 40 j = 1, nm1
         if (abs(w(j)) .gt. one) cos = one/w(j)
         if (abs(w(j)) .gt. one) sin = sqrt(one-cos**2)
         if (abs(w(j)) .le. one) sin = w(j)
         if (abs(w(j)) .le. one) cos = sqrt(one-sin**2)
         do 30 i = 1, m
            temp   =  cos*a(i,j) + sin*a(i,n)
            a(i,n) = -sin*a(i,j) + cos*a(i,n)
            a(i,j) = temp
   30       continue
   40    continue
   50 continue
      return
!
!     last card of subroutine r1mpyq.
!
   end subroutine r1mpyq
!..............................................................................
   subroutine r1updt(m,n,s,ls,u,v,w,sing)
   
      integer,    intent(in   ) :: m,n,ls
      logical,    intent(  out) :: sing
      real(ReKi), intent(in   ) :: u(m)
      real(ReKi), intent(inout) :: s(ls), v(n)
      real(ReKi), intent(  out) :: w(m)
!     **********
!
!     subroutine r1updt
!
!     given an m by n lower trapezoidal matrix s, an m-vector u,
!     and an n-vector v, the problem is to determine an
!     orthogonal matrix q such that
!
!                   t
!           (s + u*v )*q
!
!     is again lower trapezoidal.
!
!     this subroutine determines q as the product of 2*(n - 1)
!     transformations
!
!           gv(n-1)*...*gv(1)*gw(1)*...*gw(n-1)
!
!     where gv(i), gw(i) are givens rotations in the (i,n) plane
!     which eliminate elements in the i-th and n-th planes,
!     respectively. q itself is not accumulated, rather the
!     information to recover the gv, gw rotations is returned.
!
!     the subroutine statement is
!
!       subroutine r1updt(m,n,s,ls,u,v,w,sing)
!
!     where
!
!       m is a positive integer input variable set to the number
!         of rows of s.
!
!       n is a positive integer input variable set to the number
!         of columns of s. n must not exceed m.
!
!       s is an array of length ls. on input s must contain the lower
!         trapezoidal matrix s stored by columns. on output s contains
!         the lower trapezoidal matrix produced as described above.
!
!       ls is a positive integer input variable not less than
!         (n*(2*m-n+1))/2.
!
!       u is an input array of length m which must contain the
!         vector u.
!
!       v is an array of length n. on input v must contain the vector
!         v. on output v(i) contains the information necessary to
!         recover the givens rotation gv(i) described above.
!
!       w is an output array of length m. w(i) contains information
!         necessary to recover the givens rotation gw(i) described
!         above.
!
!       sing is a logical output variable. sing is set true if any
!         of the diagonal elements of the output s are zero. otherwise
!         sing is set false.
!
!     subprograms called
!
!       minpack-supplied ... 
!
!       fortran-supplied ... abs,sqrt,huge
!
!     argonne national laboratory. minpack project. march 1980.
!     burton s. garbow, kenneth e. hillstrom, jorge j. more,
!     john l. nazareth
!
!     **********
      integer i,j,jj,l,nmj,nm1
      real(ReKi) cos,cotan,giant,one,p5,p25,sin,tan,tau,temp,zero
      data one,p5,p25,zero /1.0_ReKi,5.0e-1,2.5e-1,0.0_ReKi/
!
!     giant is the largest magnitude.
!
      giant = HUGE(giant) !spmpar(3)
!
!     initialize the diagonal element pointer.
!
      jj = (n*(2*m - n + 1))/2 - (m - n)
!
!     move the nontrivial part of the last column of s into w.
!
      l = jj
      do 10 i = n, m
         w(i) = s(l)
         l = l + 1
   10    continue
!
!     rotate the vector v into a multiple of the n-th unit vector
!     in such a way that a spike is introduced into w.
!
      nm1 = n - 1
      if (nm1 .lt. 1) go to 70
      do 60 nmj = 1, nm1
         j = n - nmj
         jj = jj - (m - j + 1)
         w(j) = zero
         if (v(j) .eq. zero) go to 50
!
!        determine a givens rotation which eliminates the
!        j-th element of v.
!
         if (abs(v(n)) .ge. abs(v(j))) go to 20
            cotan = v(n)/v(j)
            sin = p5/sqrt(p25+p25*cotan**2)
            cos = sin*cotan
            tau = one
            if (abs(cos)*giant .gt. one) tau = one/cos
            go to 30
   20    continue
            tan = v(j)/v(n)
            cos = p5/sqrt(p25+p25*tan**2)
            sin = cos*tan
            tau = sin
   30    continue
!
!        apply the transformation to v and store the information
!        necessary to recover the givens rotation.
!
         v(n) = sin*v(j) + cos*v(n)
         v(j) = tau
!
!        apply the transformation to s and extend the spike in w.
!
         l = jj
         do 40 i = j, m
            temp = cos*s(l) - sin*w(i)
            w(i) = sin*s(l) + cos*w(i)
            s(l) = temp
            l = l + 1
   40       continue
   50    continue
   60    continue
   70 continue
!
!     add the spike from the rank 1 update to w.
!
      do 80 i = 1, m
         w(i) = w(i) + v(n)*u(i)
   80    continue
!
!     eliminate the spike.
!
      sing = .false.
      if (nm1 .lt. 1) go to 140
      do 130 j = 1, nm1
         if (w(j) .eq. zero) go to 120
!
!        determine a givens rotation which eliminates the
!        j-th element of the spike.
!
         if (abs(s(jj)) .ge. abs(w(j))) go to 90
            cotan = s(jj)/w(j)
            sin = p5/sqrt(p25+p25*cotan**2)
            cos = sin*cotan
            tau = one
            if (abs(cos)*giant .gt. one) tau = one/cos
            go to 100
   90    continue
            tan = w(j)/s(jj)
            cos = p5/sqrt(p25+p25*tan**2)
            sin = cos*tan
            tau = sin
  100    continue
!
!        apply the transformation to s and reduce the spike in w.
!
         l = jj
         do 110 i = j, m
            temp = cos*s(l) + sin*w(i)
            w(i) = -sin*s(l) + cos*w(i)
            s(l) = temp
            l = l + 1
  110       continue
!
!        store the information necessary to recover the
!        givens rotation.
!
         w(j) = tau
  120    continue
!
!        test for zero diagonal elements in the output s.
!
         if (s(jj) .eq. zero) sing = .true.
         jj = jj + (m - j + 1)
  130    continue
  140 continue
!
!     move w back into the last column of the output s.
!
      l = jj
      do 150 i = n, m
         s(l) = w(i)
         l = l + 1
  150    continue
      if (s(jj) .eq. zero) sing = .true.
      return
!
!     last card of subroutine r1updt.
!
   end subroutine r1updt
!..............................................................................
end module nwtc_bemt_minpack
