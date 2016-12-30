!=======================================================================
SUBROUTINE DISCON ( avrSWAP, aviFAIL, accINFILE, avcOUTNAME, avcMSG ) BIND (C, NAME='DISCON')
!DEC$ ATTRIBUTES DLLEXPORT :: DISCON

   ! This is a dummy routine for holding the place of a user-specified
   ! Bladed-style DLL controller.  Modify this code to create your own
   ! DLL controller. 
   
   ! Modified by B. Jonkman, National Renewable Energy Laboratory
   ! to conform to ISO C Bindings (standard Fortran 2003) and 
   ! compile with either gfortran or Intel Visual Fortran (IVF)
   ! DO NOT REMOVE or MODIFY LINES starting with "!DEC$" or "!GCC$"
   ! !DEC$ specifies attributes for IVF and !GCC$ specifies attributes for gfortran
   
USE, INTRINSIC :: ISO_C_Binding

IMPLICIT                        NONE
!GCC$ ATTRIBUTES DLLEXPORT :: DISCON


   ! Passed Variables:

REAL(C_FLOAT),          INTENT(INOUT) :: avrSWAP   (*)                          ! The swap array, used to pass data between the DLL controller and the calling program. 
INTEGER(C_INT),         INTENT(INOUT) :: aviFAIL                                ! A flag used to indicate the success of this DLL call set as follows: 0 if the DLL call was successful, >0 if the DLL call was successful but cMessage should be issued as a warning messsage, <0 if the DLL call was unsuccessful or for any other reason the simulation is to be stopped at this point with cMessage as the error message.
CHARACTER(KIND=C_CHAR), INTENT(IN)    :: accINFILE (NINT(avrSWAP(50)))          ! The name of the parameter input file, 'DISCON.IN'.
CHARACTER(KIND=C_CHAR), INTENT(IN)    :: avcOUTNAME(NINT(avrSWAP(51)))          ! OUTNAME (Simulation RootName) 
CHARACTER(KIND=C_CHAR), INTENT(INOUT) :: avcMSG    (NINT(avrSWAP(49)))          ! MESSAGE (Message from DLL to simulation code [ErrMsg])  The message which will be displayed by the calling program if aviFAIL <> 0.        


   ! Local Variables:

!BJJ: Please note that for conformity to FAST Modularization Framework, no variables in this DLL should contain the SAVE attribute.

REAL(4)                      :: rMeasuredPitch                                  ! Current value of the pitch angle of blade 1 (rad).
REAL(4)                      :: rMeasuredSpeed                                  ! Current value of the generator (HSS) speed (rad/s).
REAL(4), SAVE                :: rPitchDemand                                    ! Desired collective pitch angles returned by this DLL (rad).
REAL(4)                      :: rTime                                           ! Current simulation time (sec).

INTEGER(4)                   :: ErrStat
INTEGER(4)                   :: I                                               ! Generic index.
INTEGER(4)                   :: iStatus                                         ! A status flag set by the simulation as follows: 0 if this is the first call, 1 for all subsequent time steps, -1 if this is the final call at the end of the simulation.
INTEGER(4), PARAMETER        :: Un            = 87                              ! I/O unit for pack/unpack (checkpoint & restart)
 
CHARACTER(SIZE(accINFILE)-1) :: InFile                                          ! a Fortran version of the input C string (not considered an array here)    [subtract 1 for the C null-character]
CHARACTER(SIZE(avcOUTNAME)-1):: RootName                                        ! a Fortran version of the input C string (not considered an array here)    [subtract 1 for the C null-character]
CHARACTER(SIZE(avcMSG)-1)    :: ErrMsg                                          ! a Fortran version of the C string argument (not considered an array here) [subtract 1 for the C null-character] 



   ! Convert C character arrays to Fortran strings:
   
RootName = TRANSFER( avcOUTNAME(1:LEN(RootName)), RootName )
I = INDEX(RootName,C_NULL_CHAR) - 1       ! if this has a c null character at the end...
IF ( I > 0 ) RootName = RootName(1:I)     ! remove it

InFile = TRANSFER( accINFILE(1:LEN(InFile)),  InFile )
I = INDEX(InFile,C_NULL_CHAR) - 1         ! if this has a c null character at the end...
IF ( I > 0 ) InFile = InFile(1:I)         ! remove it



   ! Load variables from calling program (See Appendix A of Bladed User's Guide):

iStatus        = NINT( avrSWAP( 1) )

rTime          =       avrSWAP( 2)
rMeasuredPitch =       avrSWAP( 4)
rMeasuredSpeed =       avrSWAP(20)


   ! Initialize aviFAIL to 0:

aviFAIL        = 0



IF ( iStatus == 0 )  THEN  ! .TRUE. if we are on the first call to the DLL

   ! Read any External Controller Parameters specified in the User Interface
   !   and initialize variables:


   ! READ IN DATA CONTAINED IN FILE InFile HERE
   aviFAIL  = 0   ! SET aviFAIL AND ErrMsg IF ERROR RESULTS
   ErrMsg   = ''  !


   ! Variable initializations:
   
   rPitchDemand = rMeasuredPitch
   
   
ELSEIF (iStatus == -1)  ! .TRUE. if we are on the last call to the DLL
   
   ! Clean up any allocated arrays and close files
     
                  
ENDIF


   ! Set return values using previous demand if a sample delay is required:

avrSWAP(45) = rPitchDemand


   ! Main control calculations:

IF ( ( iStatus >= 0 ) .AND. ( aviFAIL >= 0 ) )  THEN  ! Only compute control calculations if no error has occured and we are not on the last time step

   ! PLACE MAIN CONTROL CALCULATIONS HERE BASED ON VALUES IN avrSWAP
   ! WRITE OUTPUT DATA TO FILE RootName (APPENDED WITH AN APPROPRIATE EXTENSION) HERE IF DESIRED
   aviFAIL  = 0   ! SET aviFAIL AND cMessage IF ERROR RESULTS
   ErrMsg   = ''  !

ELSEIF ( iStatus == -8 )  THEN ! .TRUE. if we need to pack data for FAST checkpoint
   
   OPEN( Un, FILE=TRIM( InFile ), STATUS='UNKNOWN', FORM='UNFORMATTED' , ACCESS='STREAM', IOSTAT=ErrStat, ACTION='WRITE' )

   IF ( ErrStat /= 0 ) THEN
      ErrMsg  = 'Cannot open file "'//TRIM( InFile )//'". Another program may have locked it for writing.'
      aviFAIL = -1
   ELSE
   
      ! write all static variables to the checkpoint file (inverse of unpack):   
      ! WRITE( Un, IOSTAT=ErrStat ) GenSpeedF               ! Filtered HSS (generator) speed, rad/s.
      
      CLOSE ( Un )
      
   END IF   
   
ELSEIF( iStatus == -9 ) THEN ! .TRUE. if we need to unpack data for FAST restart

   OPEN( Un, FILE=TRIM( InFile ), STATUS='OLD', FORM='UNFORMATTED', ACCESS='STREAM', IOSTAT=ErrStat, ACTION='READ' )

   IF ( ErrStat /= 0 ) THEN
      aviFAIL = -1
      ErrMsg  = ' Cannot open file "'//TRIM( InFile )//'" for reading. Another program may have locked.'
   ELSE
      
      ! READ all static variables from the restart file (inverse of pack):   
      !READ( Un, IOSTAT=ErrStat ) GenSpeedF               ! Filtered HSS (generator) speed, rad/s.
   
      CLOSE ( Un )
   END IF
   
ENDIF


   ! Transfer the character string back to C character array (null-terminated)
   
avcMSG = TRANSFER( TRIM(ErrMsg)//C_NULL_CHAR, avcMSG )


RETURN
END SUBROUTINE DISCON
!=======================================================================

