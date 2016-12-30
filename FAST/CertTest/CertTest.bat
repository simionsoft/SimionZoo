@ECHO OFF
@ECHO.


REM  Set up environment variables.  You will probably have to change these.


@SET Compare=FC /T
@SET CRUNCH=..\bin\crunch_win32.exe
rem @SET CRUNCH=Call Crunch

@SET MATLAB=matlab
@SET MBC_SOURCE=C:\Users\bjonkman\Documents\DATA\Downloads\MBC\Source
rem @SET MBC_SOURCE=C:\Users\bjonkman\Data\DesignCodes\MBC\Source
@SET DateTime=DateTime.exe
@SET Editor=NotePad.EXE
@SET CompareFile=CertTest.out

SET FST_DIR=.
:: SET FST_DIR=AD14forAD15Compare
:: SET FST_DIR=AD15

::=======================================================================================================
IF /I "%1"=="-X64" GOTO x64releaseVer
IF /I "%1"=="-DEBUG" GOTO debugVer
IF /I "%1"=="-GFORTRAN" GOTO gfortran
IF /I "%1"=="-IFORT" GOTO ifort
IF /I "%1"=="-DEVBUILD" GOTO devBuild
IF /I "%1"=="-DEV" GOTO devBuild
IF /I "%1"=="-DEVDEBUG" GOTO devDebugBuild

:releaseVer
@SET EXE_VER=Using released version of FAST (IVF/VS)
@SET FAST=..\bin\FAST_win32.exe
goto CertTest

:x64releaseVer
@SET EXE_VER=Using x64 released version of FAST (IVF/VS)
@SET FAST=..\bin\FAST_x64.exe
goto CertTest


:debugVer
@SET EXE_VER=Using FAST compiled in debug mode (IVF/VS)
@SET FAST=..\bin\FAST_debug_win32.exe
goto CertTest

:gfortran
@SET EXE_VER=Using FAST compiled with makefile (gfortran)
@SET FAST=..\bin\FAST_gwin32.exe
SET PATH=%PATH%;C:\LAPACK\win32;
goto CertTest

:ifort
@SET EXE_VER=Using FAST compiled with Compile_FAST.bat (IVF)
@SET FAST=..\compiling\FAST_iwin32.exe
goto CertTest

:devBuild
@SET EXE_VER=Using FAST compiled with Visual Studio Project, release mode (IVF/VS)
@SET FAST=..\bin\FAST_dev_win32.exe
goto CertTest

:devDebugBuild
@SET EXE_VER=Using FAST compiled with Visual Studio Project, debug mode (IVF/VS)
@SET FAST=..\bin\FAST_dev_debug_win32.exe
goto CertTest

::=======================================================================================================
:CertTest
@SET UseCrunch=0
IF /I "%2"=="-Crunch" @SET UseCrunch=1
IF /I "%1"=="-Crunch" @SET UseCrunch=1

::=======================================================================================================
REM  FAST test sequence definition:

@SET  TEST01=Test #01: AWT-27CR2 with many DOFs with fixed yaw error and steady wind.  AA plots.
@SET  TEST02=Test #02: AWT-27CR2 with many DOFs with startup and shutdown and steady wind.  Time plots.
@SET  TEST03=Test #03: AWT-27CR2 with many DOFs with free yaw and steady wind.  AA plots.
@SET  TEST04=Test #04: AWT-27CR2 with many DOFs with free yaw and FF turbulence.  PMF plots.
@SET  TEST05=Test #05: AWT-27CR2 with many DOFs with startup and shutdown and FF turbulence.  Time plots.
@SET  TEST06=Test #06: AOC 15/50 with many DOFs with gen start loss of grid and tip-brake shutdown.  Time plots.
@SET  TEST07=Test #07: AOC 15/50 with many DOFs with free yaw and FF turbulence.  PMF plots.
@SET  TEST08=Test #08: AOC 15/50 with many DOFs with fixed yaw error and steady wind.  AA plots.
@SET  TEST09=Test #09: UAE Phase VI (downwind) with many DOFs with yaw ramp and a steady wind.  Time plots.
@SET  TEST10=Test #10: UAE Phase VI (upwind) with no DOFs in a ramped wind.  Time plots.
@SET  TEST11=Test #11: WindPACT 1.5 MW Baseline with many DOFs undergoing a pitch failure.  Time plots.
@SET  TEST12=Test #12: WindPACT 1.5 MW Baseline with many DOFs with VS and VP and ECD wind.  Time plots.
@SET  TEST13=Test #13: WindPACT 1.5 MW Baseline with many DOFs with VS and VP and FF turbulence.  PMF plots.
@SET  TEST14=Test #14: WindPACT 1.5 MW Baseline with many DOFs and system linearization.  Column chart.
@SET  TEST15=Test #15: SWRT with many DOFs with free yaw tail-furl and VS and EOG wind.  Time plots.
@SET  TEST16=Test #16: SWRT with many DOFs with free yaw tail-furl and VS and EDC wind.  Time plots.
@SET  TEST17=Test #17: SWRT with many DOFs with free yaw tail-furl and VS and FF turbulence.  PMF plots.
@SET  TEST18=Test #18: NREL 5 MW Baseline Land-based Turbine
@SET  TEST19=Test #19: NREL 5 MW Baseline Offshore Turbine with OC3-Monopile Configuration
@SET  TEST20=Test #20: NREL 5 MW Baseline Offshore Turbine with OC3-Tripod Configuration
@SET  TEST21=Test #21: NREL 5 MW Baseline Offshore Turbine with OC4-Jacket Configuration
@SET  TEST22=Test #22: NREL 5 MW Baseline Offshore Turbine with ITI-Barge Configuration
@SET  TEST23=Test #23: NREL 5 MW Baseline Offshore Turbine with MIT-NREL TLP Configuration
@SET  TEST24=Test #24: NREL 5 MW Baseline Offshore Turbine with OC3-Hywind Configuration
@SET  TEST25=Test #25: NREL 5 MW Baseline Offshore Turbine with OC4-DeepCwind Semi-submersible Configuration
@SET  TEST26=Test #26: NREL 5 MW Baseline Land-based Turbine with BeamDyn for blade dynamics

@SET  DASHES=---------------------------------------------------------------------------------------------
@SET  POUNDS=#############################################################################################

@IF EXIST %CompareFile%  DEL %CompareFile%

ECHO.                                               >> %CompareFile%
ECHO           ************************************ >> %CompareFile%
ECHO           **  FAST Acceptance Test Results  ** >> %CompareFile%
ECHO           ************************************ >> %CompareFile%

ECHO.                                                                             >> %CompareFile%
ECHO ############################################################################ >> %CompareFile%
ECHO # Inspect this file for any differences between your results and the saved # >> %CompareFile%
ECHO # results.  Any differing lines and the two lines surrounding them will be # >> %CompareFile%
ECHO # listed.  The only differences should be the time stamps at the start of  # >> %CompareFile%
ECHO # each file.                                                               # >> %CompareFile%
ECHO #                                                                          # >> %CompareFile%
ECHO # If you are running on something other than a PC, you may see differences # >> %CompareFile%
ECHO # in the last significant digit of many of the numbers.                    # >> %CompareFile%
ECHO ############################################################################ >> %CompareFile%

ECHO.                                            >> %CompareFile%
ECHO Date and time this acceptance test was run: >> %CompareFile%
%DateTime%                                       >> %CompareFile%
ECHO.                                            >> %CompareFile%


ECHO.                                            >> %CompareFile%
ECHO %EXE_VER%                                   >> %CompareFile%
ECHO FAST called with this command:              >> %CompareFile%
ECHO %FAST%                                      >> %CompareFile%
ECHO.                                            >> %CompareFile%


echo %DASHES%
echo %EXE_VER%
echo %FAST%
echo %DASHES%


rem *******************************************************
:Test1
@CALL :GenTestHeader %Test01%
@CALL :RunFASTandCrunch 01 out

@CALL :CompareFiles 01 sts
@CALL :CompareFiles 01 azi
@CALL :CompareFiles 01 sum
@CALL :CompareFiles 01 AD.sum


rem *******************************************************
:Test2
@CALL :GenTestHeader %Test02%
@CALL :RunFASTandCrunch 02 outb

@IF NOT EXIST Test%TEST%.out   GOTO ERROR

@CALL :CompareFiles 02 sts
@CALL :CompareFiles 02 ED.sum
@CALL :CompareFiles 02 AD.sum


rem *******************************************************
:Test3
@CALL :GenTestHeader %Test03%
@CALL :RunFASTandCrunch 03 out

@CALL :CompareFiles 03 sts
@CALL :CompareFiles 03 azi
@CALL :CompareFiles 03 ED.sum
@CALL :CompareFiles 03 AD.sum


rem *******************************************************
:Test4
@CALL :GenTestHeader %Test04%
@CALL :RunFASTandCrunch 04 outb

@CALL :CompareFiles 04 sts
@CALL :CompareFiles 04 pmf
@CALL :CompareFiles 04 ED.sum
@CALL :CompareFiles 04 AD.sum


rem *******************************************************
:Test5
@CALL :GenTestHeader %Test05%
@CALL :RunFASTandCrunch 05 out

@CALL :CompareFiles 05 sts
@CALL :CompareFiles 05 ED.sum
@CALL :CompareFiles 05 AD.sum


rem *******************************************************
:Test6
@CALL :GenTestHeader %Test06%
@CALL :RunFASTandCrunch 06 out

@CALL :CompareFiles 06 sts
@CALL :CompareFiles 06 sum
@CALL :CompareFiles 06 AD.sum

rem *******************************************************
:Test7
@CALL :GenTestHeader %Test07%
@CALL :RunFASTandCrunch 07 out

@CALL :CompareFiles 07 sts
@CALL :CompareFiles 07 pmf
@CALL :CompareFiles 07 ED.sum
@CALL :CompareFiles 07 AD.sum

rem *******************************************************
:Test8
@CALL :GenTestHeader %Test08%
@CALL :RunFASTandCrunch 08 out

@CALL :CompareFiles 08 sts
@CALL :CompareFiles 08 azi
@CALL :CompareFiles 08 ED.sum
@CALL :CompareFiles 08 AD.sum

rem *******************************************************
:Test9
@CALL :GenTestHeader %Test09%
@CALL :RunFASTandCrunch 09 out

@CALL :CompareFiles 09 sts
@CALL :CompareFiles 09 ED.sum
@CALL :CompareFiles 09 AD.sum

rem *******************************************************
:Test10
@CALL :GenTestHeader %Test10%
@CALL :RunFASTandCrunch 10 out


@CALL :CompareFiles 10 sts
@CALL :CompareFiles 10 ED.sum
@CALL :CompareFiles 10 AD.sum


rem *******************************************************
:Test11
@CALL :GenTestHeader %Test11%
@CALL :RunFASTandCrunch 11 out

@CALL :CompareFiles 11 sts
@CALL :CompareFiles 11 sum
@CALL :CompareFiles 11 AD.sum


rem *******************************************************
:Test12
@CALL :GenTestHeader %Test12%
@CALL :RunFASTandCrunch 12 out

@CALL :CompareFiles 12 sts
@CALL :CompareFiles 12 ED.sum
@CALL :CompareFiles 12 AD.sum

rem *******************************************************
:Test13
@CALL :GenTestHeader %Test13%
@CALL :RunFASTandCrunch 13 out

@CALL :CompareFiles 13 sts
@CALL :CompareFiles 13 pmf
@CALL :CompareFiles 13 ED.sum
@CALL :CompareFiles 13 AD.sum

rem *******************************************************
:Test14
@echo FAST %TEST14%


echo %POUNDS%
@echo Skipping this test until linearization is included.
echo %POUNDS%
GOTO Test15

rem Run FAST.

rem %FAST% Test%TEST%.fst

IF ERRORLEVEL 1  GOTO ERROR

@IF NOT EXIST Test%TEST%.lin  GOTO ERROR

Perform an eigenanalysis in MATLAB:
echo. Running Matlab to calculate eigenvalues. If an error occurs, close Matlab to continue CertTest....
%MATLAB% /wait /r addpath('%MBC_SOURCE%');Test%TEST% /logfile Test%TEST%.eig

@rem echo. Call to Matlab completed.

IF ERRORLEVEL 1  GOTO MATLABERROR

@IF NOT EXIST Test%TEST%.eig  GOTO MATLABERROR

echo.                                            >> %CompareFile%
echo %POUNDS%                                    >> %CompareFile%
echo.                                            >> %CompareFile%
echo %TEST14%                                    >> %CompareFile%

@CALL :CompareFiles 14 eig
@CALL :CompareFiles 14 sum


rem *******************************************************
:Test15
:MATLABERROR
@CALL :GenTestHeader %Test15%
@CALL :RunFASTandCrunch 15 out

@CALL :CompareFiles 15 sts
@CALL :CompareFiles 15 sum
@CALL :CompareFiles 15 AD.sum

rem *******************************************************
:Test16
@CALL :GenTestHeader %Test16%
@CALL :RunFASTandCrunch 16 out

@CALL :CompareFiles 16 sts
@CALL :CompareFiles 16 ED.sum
@CALL :CompareFiles 16 AD.sum


rem *******************************************************
:Test17
@SET TEST=17

@CALL :GenTestHeader %Test17%
@CALL :RunFASTandCrunch 17  out

@CALL :CompareFiles 17 sts
@CALL :CompareFiles 17 pmf
@CALL :CompareFiles 17 ED.sum
@CALL :CompareFiles 17 AD.sum


rem *******************************************************
:Test18
@CALL :GenTestHeader %Test18%
@CALL :RunFASTandCrunch 18 outb
@CALL :CompareOutput 18


rem *******************************************************
:Test19
@CALL :GenTestHeader %Test19%
@CALL :RunFASTandCrunch 19 outb
@CALL :CompareOutput 19
rem @CALL :CompareFiles 19 HD.sum


rem *******************************************************
:Test20
@CALL :GenTestHeader %Test20%
@CALL :RunFASTandCrunch 20 outb
@CALL :CompareOutput 20
rem @CALL :CompareFiles 19 HD.sum
rem @CALL :CompareFiles 19 SD.sum

rem *******************************************************
:Test21
@CALL :GenTestHeader %Test21%
@CALL :RunFASTandCrunch 21 outb
@CALL :CompareOutput 21
rem @CALL :CompareFiles 21 HD.sum
rem @CALL :CompareFiles 21 SD.sum


rem *******************************************************
:Test22
@CALL :GenTestHeader %Test22%
@CALL :RunFASTandCrunch 22 outb
@CALL :CompareOutput 22

rem *******************************************************
:Test23
@CALL :GenTestHeader %Test23%
@CALL :RunFASTandCrunch 23 outb
@CALL :CompareOutput 23


rem *******************************************************
:Test24
@CALL :GenTestHeader %Test24%
@CALL :RunFASTandCrunch 24 outb
@CALL :CompareOutput 24

rem *******************************************************
:Test25
@CALL :GenTestHeader %Test25%
@CALL :RunFASTandCrunch 25 outb
@CALL :CompareOutput 25


rem *******************************************************
:Test26
@CALL :GenTestHeader %Test26%
@CALL :RunFASTandCrunch 26 outb
@CALL :CompareOutput 26

rem ******************************************************
rem  Let's look at the comparisons.
:MatlabComparisons

rem %MATLAB% /r PlotCertTestResults('.','.\TstFiles');exit;


%Editor% %CompareFile%
goto END

rem ******************************************************
:GenTestHeader
echo %POUNDS%
@echo FAST %*
echo %POUNDS%

echo.                                    >> %CompareFile%
echo %POUNDS%                            >> %CompareFile%
echo.                                    >> %CompareFile%
echo %*                                  >> %CompareFile%
EXIT /B

rem ******************************************************
:RunFASTandCrunch
:: Run FAST.
@SET TEST=%1

%FAST% %FST_DIR%\Test%1.fst

:: IF ERRORLEVEL 1  GOTO ERROR
:: @IF NOT EXIST Test%1.%2  GOTO ERROR

echo %DASHES%
@IF "%UseCrunch%"=="1" (
:: Crunch the FAST output.
%CRUNCH% Test%1.cru

IF ERRORLEVEL 1  GOTO ERROR
@IF NOT EXIST Test%1.sts  GOTO ERROR
)

EXIT /B

rem ******************************************************
:CompareOutput

@CALL :CompareFiles %1 sts
@CALL :CompareFiles %1 sum
@CALL :CompareFiles %1 AD.sum

EXIT /B

:CompareFiles

@IF "%UseCrunch%"=="0" (
@IF /I "%2"=="sts" GOTO EndCompareFiles
@IF /I "%2"=="azi" GOTO EndCompareFiles
@IF /I "%2"=="pmf" GOTO EndCompareFiles
)

echo %DASHES%                          >> %CompareFile%
%Compare% %FST_DIR%\Test%1.%2 TstFiles\Test%1.%2 >> %CompareFile%

:EndCompareFiles
EXIT /B



:ERROR
:: Sets clears memory and stops the batch immediately
@echo ** An error has occurred in Test #%TEST% **
@echo ** An error has occurred in Test #%TEST% ** >> %CompareFile%

@call :end
@call :__ErrorExit 2> nul
EXIT /B

:__ErrorExit
rem Creates a syntax error, stops immediately
()
EXIT /B


:END

@SET UseCrunch=
@SET CRUNCH=
@SET MATLAB=
@SET MBC_SOURCE=
@SET Compare=
@SET CompareFile=
@SET DASHES=
@SET DateTime=
@SET Editor=
@SET FAST=
@SET POUNDS=
@SET TEST=
@SET TEST01=
@SET TEST02=
@SET TEST03=
@SET TEST04=
@SET TEST05=
@SET TEST06=
@SET TEST07=
@SET TEST08=
@SET TEST09=
@SET TEST10=
@SET TEST11=
@SET TEST12=
@SET TEST13=
@SET TEST14=
@SET TEST15=
@SET TEST16=
@SET TEST17=
@SET TEST18=
@SET TEST19=
@SET TEST20=
@SET TEST21=
@SET TEST22=
@SET TEST23=
@SET TEST24=
@SET TEST25=
@SET TEST26=

SET EXE_VER=
SET FST_DIR=

type Bell.txt
@echo Processing complete.

EXIT /B