@ECHO OFF

REM The calling syntax for this script is
REM                  Compile_FAST [{32 | 64}] [-RegistryOnly]
REM
REM Add the "32" to the command line to compile FAST in 32-bit.
REM Add the "64" to the command line to compile FAST in 64-bit.
REM default is 32.

rem SET BITS=%1

SET RegOnly=1
IF /I "%1"=="-REGISTRYONLY" goto SetPaths
IF /I "%2"=="-REGISTRYONLY" goto SetPaths
SET RegOnly=0

REM ----------------------------------------------------------------------------
REM                   set compiler internal variables
REM ----------------------------------------------------------------------------
REM    You can run this bat file from the IVF compiler's command prompt (and not
REM    do anything in this section). If you choose not to run from the IVF command
REM    prompt, you must call the compiler's script to set internal variables.
REM    TIP: Right click on the IVF Compiler's Command Prompt shortcut, click
REM    properties, and copy the target (without cmd.exe and/or its switches) here:

rem call "C:\Program Files (x86)\Intel\Composer XE 2011 SP1\bin\ipsxe-comp-vars.bat" ia32 vs2010


SET Lines=-----

IF "%INCLUDE%"=="" (
rem we haven't called the compiler, yet

IF /I "%1"=="64" ( ECHO %Lines% Using intel64 compiler %Lines%
CALL "C:\Program Files (x86)\Intel\Composer XE 2013 SP1\bin\ipsxe-comp-vars.bat" intel64 vs2013
SET BITS=64
) ELSE ( ECHO  %Lines% Using ia32 compiler %Lines%
CALL "C:\Program Files (x86)\Intel\Composer XE 2013 SP1\bin\ipsxe-comp-vars.bat" ia32 vs2013
SET BITS=32
)

) ELSE (
rem we have called the compiler before

IF /I "%1"=="64" ( %Lines% Using existing compiler settings: intel64 %Lines%
SET BITS=64
) ELSE ( ECHO  %Lines% Using existing compiler settings: ia32 %Lines%
SET BITS=32
)

)


:SetPaths
REM ----------------------------------------------------------------------------
REM ------------------------- LOCAL PATHS --------------------------------------
REM ----------------------------------------------------------------------------
REM -- USERS MAY EDIT THESE PATHS TO POINT TO FOLDERS ON THEIR LOCAL MACHINES. -
REM -- NOTE: do not use quotation marks around the path names!!!! --------------
REM ----------------------------------------------------------------------------
REM ----------------------------------------------------------------------------

SET Registry=..\bin\Registry_win32.exe
SET FAST_Loc=..\Source

SET NWTC_Lib_Loc=%FAST_Loc%\dependencies\NWTC_Library
SET NETLIB_Loc=%FAST_Loc%\dependencies\NetLib
SET ED_Loc=%FAST_Loc%\dependencies\ElastoDyn
SET BD_Loc=%FAST_Loc%\dependencies\BeamDyn
SET SrvD_Loc=%FAST_Loc%\dependencies\ServoDyn
SET TMD_Loc=%SrvD_Loc%
SET AD14_Loc=%FAST_Loc%\dependencies\AeroDyn14
SET AD_Loc=%FAST_Loc%\dependencies\AeroDyn
SET DWM_Loc=%AD14_Loc%
SET IfW_Loc=%FAST_Loc%\dependencies\InflowWind
SET OpFM_Loc=%FAST_Loc%\dependencies\OpenFOAM
SET HD_Loc=%FAST_Loc%\dependencies\HydroDyn
SET SD_Loc=%FAST_Loc%\dependencies\SubDyn
SET MAP_Loc=%FAST_Loc%\dependencies\MAP
SET FEAM_Loc=%FAST_Loc%\dependencies\FEAMooring
SET MD_Loc=%FAST_Loc%\dependencies\MoorDyn
SET Orca_Loc=%FAST_Loc%\dependencies\OrcaFlex
SET IceF_Loc=%FAST_Loc%\dependencies\IceFloe
SET IceD_Loc=%FAST_Loc%\dependencies\IceDyn

SET AFI_Loc=%AD_Loc%
SET UA_Loc=%AD_Loc%
SET BEMT_Loc=%AD_Loc%


SET MAP_Include_Lib=%MAP_Loc%\map_win32.lib
IF "%BITS%"=="64" SET MAP_Include_Lib=%MAP_Loc%\map_x64.lib
SET HD_Reg_Loc=%HD_Loc%
SET IfW_Reg_Loc=%IfW_Loc%
SET IceF_RanLux_Loc=%IceF_Loc%
SET FEAM_Reg_Loc=%FEAM_Loc%
SET Orca_Reg_Loc=%Orca_Loc%

SET MAP_Loc_R=%MAP_Loc%
SET MAP_Loc_F=%MAP_Loc%

REM ----------------------------------------------------------------------------
REM The following script changes the above paths for Bonnie Jonkman; other users
REM    can ignore these lines or create their own Set_FAST_paths.bat file
IF /I "%2"=="dev" CALL Set_FAST_paths.bat
IF /I "%3"=="dev" CALL Set_FAST_paths.bat
REM ----------------------------------------------------------------------------


REM ----------------------------------------------------------------------------
REM -------------------- LOCAL VARIABLES ---------------------------------------
REM ----------------------------------------------------------------------------

SET ROOT_NAME=..\bin\FAST_iwin%BITS%
SET INTER_DIR=Obj_iwin%BITS%

:: /nologo /fpp /stand:f03 /Qdiag-disable:5268 /traceback /libs:static /threads /Qmkl:sequential /c
:: /Qmkl:sequential used to include the intel math kernel library

SET COMPOPTS=/threads  /O2 /inline:speed /traceback /real_size:32 /fpp
SET LINKOPTS=/link %MAP_Include_Lib% /LARGEADDRESSAWARE /STACK:999999999

SET LINES=++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

:SourceFiles
REM ----------------------------------------------------------------------------
REM -------------------- LIST OF ALL SOURCE FILES ------------------------------
REM ----------------------------------------------------------------------------

SET NWTC_SOURCES=^
 "%NWTC_Lib_Loc%\SingPrec.f90"^
 "%NWTC_Lib_Loc%\NWTC_Base.f90" ^
 "%NWTC_Lib_Loc%\SysIVF.f90" ^
 "%NWTC_Lib_Loc%\NWTC_Library_Types.f90" ^
 "%NWTC_Lib_Loc%\NWTC_IO.f90" ^
 "%NWTC_Lib_Loc%\NWTC_Num.f90" ^
 "%NWTC_Lib_Loc%\ModMesh_Types.f90" ^
 "%NWTC_Lib_Loc%\ModMesh.f90" ^
 "%NETLIB_Loc%\dlasrt2.f" ^
 "%NETLIB_Loc%\slasrt2.f" ^
 "%NETLIB_Loc%\NWTC_ScaLAPACK.f90" ^
 "%NETLIB_Loc%\NWTC_LAPACK.f90" ^
 "%NWTC_Lib_Loc%\ModMesh_Mapping.f90" ^
 "%NWTC_Lib_Loc%\NWTC_Library.f90" ^
 "%NETLIB_Loc%\fftpack4.1.f" ^
 "%NETLIB_Loc%\NWTC_FFTPACK.f90" ^
 "%NETLIB_Loc%\dierckx_fitpack.f" ^
 "%NETLIB_Loc%\NWTC_FitPack.f90"


SET IfW_SOURCES=^
 "%IfW_Loc%\IFW_BladedFFWind_Types.f90" ^
 "%IfW_Loc%\IFW_BladedFFWind.f90" ^
 "%IfW_Loc%\IFW_TSFFWind_Types.f90" ^
 "%IfW_Loc%\IFW_TSFFWind.f90" ^
 "%IfW_Loc%\IFW_HAWCWind_Types.f90" ^
 "%IfW_Loc%\IFW_HAWCWind.f90" ^
 "%IfW_Loc%\IFW_UniformWind_Types.f90"^
 "%IfW_Loc%\IFW_UniformWind.f90" ^
 "%IfW_Loc%\IFW_UserWind_Types.f90"^
 "%IfW_Loc%\IFW_UserWind.f90" ^
 "%IfW_Loc%\Lidar_Types.f90"^
 "%IfW_Loc%\InflowWind_Types.f90"^
 "%IfW_Loc%\InflowWind_Subs.f90"^
 "%IfW_Loc%\Lidar.f90"^
 "%IfW_Loc%\InflowWind.f90"

rem SET OpFM_SOURCES=^

SET Orca_SOURCES=^
 "%Orca_Loc%\OrcaFlexInterface_Types.f90"^
 "%Orca_Loc%\OrcaFlexInterface.f90"



SET AD14_SOURCES=^
 "%AD14_Loc%\AeroDyn14_Types.f90"^
 "%AD14_Loc%\GenSubs.f90"^
 "%AD14_Loc%\AeroSubs.f90"^
 "%AD14_Loc%\AeroDyn14.f90"

SET DWM_SOURCES=^
 "%DWM_Loc%\DWM_Types.f90"^
 "%DWM_Loc%\DWM_Wake_Sub_ver2.f90"^
 "%DWM_Loc%\DWM.f90"


SET AFI_SOURCES=^
 "%AFI_Loc%\AirfoilInfo_Types.f90"^
 "%AFI_Loc%\AirfoilInfo.f90"

SET UA_SOURCES=^
 "%UA_Loc%\UnsteadyAero_Types.f90"^
 "%UA_Loc%\UnsteadyAero.f90"

SET BEMT_SOURCES=^
 "%BEMT_Loc%\BEMT_Types.f90"^
 "%BEMT_Loc%\BladeElement.f90"^
 "%BEMT_Loc%\BEMTCoupled.f90"^
 "%BEMT_Loc%\nwtc_bemt_minpack.f90"^
 "%BEMT_Loc%\BEMTUncoupled.f90"^
 "%BEMT_Loc%\Brent.f90"^
 "%BEMT_Loc%\fmin_fcn.f90"^
 "%BEMT_Loc%\mod_root1dim.f90"^
 "%BEMT_Loc%\BEMT.f90"

SET AD_SOURCES=^
 "%AD_Loc%\AeroDyn_Types.f90"^
 "%AD_Loc%\AeroDyn_IO.f90"^
 "%AD_Loc%\AeroDyn.f90"

SET BD_SOURCES=^
 "%BD_Loc%\BeamDyn_Types.f90"^
 "%BD_Loc%\BeamDyn_Subs.f90"^
 "%BD_Loc%\BeamDyn_IO.f90"^
 "%BD_Loc%\BeamDyn.f90"


SET ED_SOURCES=^
 "%ED_Loc%\ElastoDyn_Types.f90" ^
 "%ED_Loc%\ElastoDyn.f90"


SET SrvD_SOURCES=^
 "%TMD_Loc%\TMD_Types.f90" ^
 "%TMD_Loc%\TMD.f90" ^
 "%SrvD_Loc%\ServoDyn_Types.f90" ^
 "%SrvD_Loc%\BladedInterface.f90" ^
 "%SrvD_Loc%\ServoDyn.f90" ^
 "%SrvD_Loc%\PitchCntrl_ACH.f90" ^
 "%SrvD_Loc%\UserSubs.f90" ^
 "%SrvD_Loc%\UserVSCont_KP.f90"


SET HD_SOURCES=^
 "%HD_Loc%\SS_Radiation_Types.f90" ^
 "%HD_Loc%\SS_Radiation.f90" ^
 "%HD_Loc%\Waves_Types.f90" ^
 "%HD_Loc%\UserWaves.f90" ^
 "%HD_Loc%\Waves.f90" ^
 "%HD_Loc%\Waves2_Types.f90" ^
 "%HD_Loc%\Waves2_Output.f90" ^
 "%HD_Loc%\Waves2.f90" ^
 "%HD_Loc%\Morison_Types.f90" ^
 "%HD_Loc%\Morison_Output.f90" ^
 "%HD_Loc%\Morison.f90" ^
 "%HD_Loc%\Current_Types.f90" ^
 "%HD_Loc%\Current.f90" ^
 "%HD_Loc%\Conv_Radiation_Types.f90" ^
 "%HD_Loc%\Conv_Radiation.f90" ^
 "%HD_Loc%\WAMIT_Types.f90" ^
 "%HD_Loc%\WAMIT_Interp.f90" ^
 "%HD_Loc%\WAMIT_Output.f90" ^
 "%HD_Loc%\WAMIT.f90" ^
 "%HD_Loc%\WAMIT2_Types.f90" ^
 "%HD_Loc%\WAMIT2_Output.f90" ^
 "%HD_Loc%\WAMIT2.f90" ^
 "%HD_Loc%\HydroDyn_Types.f90" ^
 "%HD_Loc%\HydroDyn_Output.f90" ^
 "%HD_Loc%\HydroDyn_Input.f90" ^
 "%HD_Loc%\HydroDyn.f90"


SET SD_SOURCES=^
 "%SD_Loc%\qsort_c_module.f90" ^
 "%SD_Loc%\SubDyn_Types.f90" ^
 "%SD_Loc%\SD_FEM.f90" ^
 "%SD_Loc%\SubDyn_Output.f90" ^
 "%SD_Loc%\SubDyn.f90"


SET MAP_SOURCES=^
 "%MAP_Loc_R%\MAP_Types.f90" ^
 "%MAP_Loc_F%\MAP.f90"


SET FEAM_SOURCES=^
 "%FEAM_Loc%\FEAMooring_Types.f90" ^
 "%FEAM_Loc%\FEAM.f90"

SET MD_SOURCES=^
 "%MD_Loc%\MoorDyn_Types.f90" ^
 "%MD_Loc%\MoorDyn_IO.f90" ^
 "%MD_Loc%\MoorDyn.f90"



SET IceF_SOURCES=^
 "%IceF_RanLux_Loc%\RANLUX.f90" ^
 "%IceF_Loc%\IceFloe_Types.f90" ^
 "%IceF_Loc%\iceLog.F90" ^
 "%IceF_Loc%\iceInput.f90" ^
 "%IceF_Loc%\IceFloeBase.F90" ^
 "%IceF_Loc%\coupledCrushing.F90" ^
 "%IceF_Loc%\crushingIEC.F90" ^
 "%IceF_Loc%\crushingISO.F90" ^
 "%IceF_Loc%\intermittentCrushing.F90" ^
 "%IceF_Loc%\lockInISO.F90" ^
 "%IceF_Loc%\randomCrushing.F90" ^
 "%IceF_Loc%\IceFlexBase.F90" ^
 "%IceF_Loc%\IceFlexIEC.f90" ^
 "%IceF_Loc%\IceFlexISO.f90" ^
 "%IceF_Loc%\IceFloe.f90"

SET IceD_SOURCES=^
 "%IceD_Loc%\IceDyn_Types.f90"^
 "%IceD_Loc%\IceDyn.f90"


SET FAST_SOURCES=^
 "%OpFM_Loc%\OpenFOAM_Types.f90" ^
 "%FAST_LOC%\FAST_Types.f90" ^
 "%OpFM_Loc%\OpenFOAM.f90" ^
 "%FAST_LOC%\FAST_Mods.f90" ^
 "%FAST_LOC%\FAST_Subs.f90" ^
 "%FAST_LOC%\FAST_Prog.f90"


:RunRegistry
REM ----------------------------------------------------------------------------
REM ---------------- RUN THE REGISTRY TO AUTO-GENERATE FILES -------------------
REM ----------------------------------------------------------------------------
REM note that I'm changing directories, only to put the auto-generated files in their respective locations

ECHO %Lines%
ECHO Running the FAST Registry to auto-generate source files:
ECHO.

SET CURR_LOC=%FAST_Loc%

%REGISTRY% "%CURR_LOC%\FAST_Registry.txt" -I "%NWTC_Lib_Loc%" -I "%ED_Loc%" -I "%SrvD_Loc%" -I "%AD14_Loc%" -I^
 "%AD_Loc%" -I "%BEMT_Loc%" -I "%UA_Loc%" -I "%AFI_Loc%" -I "%BD_Loc%" -I^
 "%IfW_Reg_Loc%" -I "%DWM_LOC%" -I "%SD_Loc%" -I "%HD_Reg_Loc%" -I "%MAP_Loc_R%" -I "%FEAM_Reg_Loc%"  -I^
 "%IceF_Loc%" -I "%IceD_Loc%" -I "%TMD_Loc%" -I "%MD_Loc%" -I "%OpFM_Loc%" -I "%Orca_Reg_Loc%" -noextrap -O "%CURR_LOC%"


ECHO %Lines%
SET CURR_LOC=%ED_Loc%
CALL ::RunRegistry_fmt1 ElastoDyn

ECHO %Lines%
SET CURR_LOC=%BD_Loc%
%REGISTRY% "%CURR_LOC%\Registry_BeamDyn.txt" -I "%NWTC_Lib_Loc%" -O "%CURR_LOC%"


ECHO %Lines%
SET CURR_LOC=%TMD_Loc%
CALL ::RunRegistry_fmt1 TMD

ECHO %Lines%
SET CURR_LOC=%SrvD_Loc%
CALL ::RunRegistry_fmt1 ServoDyn


ECHO %Lines%
SET CURR_LOC=%IfW_Loc%
CALL :RunRegistry_IfW IfW_BladedFFWind -noextrap
CALL :RunRegistry_IfW IfW_TSFFWind -noextrap
CALL :RunRegistry_IfW IfW_HAWCWind -noextrap
CALL :RunRegistry_IfW IfW_UniformWind -noextrap
CALL :RunRegistry_IfW IfW_UserWind -noextrap
CALL :RunRegistry_IfW Lidar
CALL :RunRegistry_IfW InflowWind


ECHO %Lines%
SET CURR_LOC=%AD14_Loc%
%REGISTRY% "%CURR_LOC%\Registry-AD14.txt" -I "%NWTC_Lib_Loc%" -I "%IfW_Reg_Loc%" -I "%DWM_Loc%" -O "%CURR_LOC%"

ECHO %Lines%
SET CURR_LOC=%AD_Loc%
%REGISTRY% "%CURR_LOC%\AeroDyn_Registry.txt" -I "%NWTC_Lib_Loc%" -I "%BEMT_Loc%" -I "%UA_Loc%" -I "%AFI_Loc%" -O "%CURR_LOC%"

ECHO %Lines%
SET CURR_LOC=%BEMT_Loc%
%REGISTRY% "%CURR_LOC%\BEMT_Registry.txt" -I "%NWTC_Lib_Loc%" -I "%UA_Loc%" -I "%AFI_Loc%" -O "%CURR_LOC%"

ECHO %Lines%
SET CURR_LOC=%AFI_Loc%
%REGISTRY% "%CURR_LOC%\AirfoilInfo_Registry.txt" -I "%NWTC_Lib_Loc%" -noextrap -O "%CURR_LOC%"

ECHO %Lines%
SET CURR_LOC=%UA_Loc%
%REGISTRY% "%CURR_LOC%\UnsteadyAero_Registry.txt" -I "%NWTC_Lib_Loc%" -I "%AFI_Loc%" -O "%CURR_LOC%"


ECHO %Lines%
SET CURR_LOC=%DWM_Loc%
%REGISTRY% "%CURR_LOC%\Registry-DWM.txt" -I "%NWTC_Lib_Loc%" -I "%IfW_Reg_Loc%" -O "%CURR_LOC%"


ECHO %Lines%
SET CURR_LOC=%FEAM_Reg_Loc%
%REGISTRY% "%CURR_LOC%\FEAM_Registry.txt" -I "%NWTC_Lib_Loc%" -O "%CURR_LOC%"

ECHO %Lines%
SET CURR_LOC=%MD_Loc%
CALL ::RunRegistry_fmt1 MoorDyn

ECHO %Lines%
SET CURR_LOC=%Orca_Loc%
%REGISTRY% "%Orca_Reg_Loc%\OrcaFlexInterface.txt" -I "%NWTC_Lib_Loc%" -I "%Orca_Reg_Loc%" -O "%CURR_LOC%"


ECHO %Lines%
SET CURR_LOC=%MAP_Loc_R%
%REGISTRY% "%CURR_LOC%\MAP_Registry.txt" -ccode -I "%NWTC_Lib_Loc%" -O "%CURR_LOC%"


ECHO %Lines%
SET CURR_LOC=%IceF_Loc%
%REGISTRY% "%CURR_LOC%\IceFloe_FASTRegistry.inp" -I "%NWTC_Lib_Loc%" -O "%CURR_LOC%"


ECHO %Lines%
SET CURR_LOC=%IceD_Loc%
%REGISTRY% "%CURR_LOC%\Registry_IceDyn.txt" -I "%NWTC_Lib_Loc%" -O "%CURR_LOC%"

ECHO %Lines%
SET CURR_LOC=%OpFM_Loc%
%REGISTRY% "%CURR_LOC%\OpenFOAM_Registry.txt" -I "%NWTC_Lib_Loc%" -ccode -O "%CURR_LOC%"


ECHO %Lines%
SET CURR_LOC=%HD_Loc%
CALL ::RunRegistry_HD  Current
CALL ::RunRegistry_HD  Waves
CALL ::RunRegistry_HD  Waves2
CALL ::RunRegistry_HD  SS_Radiation
CALL ::RunRegistry_HD  Conv_Radiation
CALL ::RunRegistry_HD  WAMIT
CALL ::RunRegistry_HD  WAMIT2
CALL ::RunRegistry_HD  Morison
CALL ::RunRegistry_HD  HydroDyn


ECHO %Lines%
SET CURR_LOC=%SD_Loc%
CALL ::RunRegistry_fmt1  SubDyn


ECHO %Lines%
rem CALL :setRegistryValues MAP
REM SET CURR_LOC=%MAP_Loc%
rem need the syntax for generating the c-to-fortran code...


IF %RegOnly%==1 goto end

:ivf
REM ----------------------------------------------------------------------------
REM ---------------- COMPILE WITH INTEL VISUAL FORTRAN -------------------------
REM ----------------------------------------------------------------------------

REM                           compile
rem goto link_now


:: we'll make this intermediate directory if it doesn't exist
if not exist %INTER_DIR% mkdir %INTER_DIR%

:: and we'll clean it out so we don't get extra files when we link
if exist %INTER_DIR%\*.mod DEL %INTER_DIR%\*.mod
if exist %INTER_DIR%\*.obj DEL %INTER_DIR%\*.obj


:: ECHO %Lines%
:: ECHO Compiling FAST and its component modules to create %ROOT_NAME%.exe:

rem ifort %COMPOPTS% %NWTC_Files%  %LINKOPTS% /out:%ROOT_NAME%.exe
rem NOTE that I'm compiling the modules separately then linking them later. I split it up because the list of source files was getting too long ("Input line too long" error)

ECHO %Lines%
ECHO Compiling NWTC Library:
ifort %COMPOPTS% %NWTC_SOURCES%  /Qmkl:sequential  /c /object:%INTER_DIR%\ /module:%INTER_DIR%\
IF %ERRORLEVEL% NEQ 0 GOTO checkError


ECHO %Lines%
ECHO Compiling InflowWind:
ifort %COMPOPTS% %IfW_SOURCES%  /c /object:%INTER_DIR%\ /module:%INTER_DIR%\
IF %ERRORLEVEL% NEQ 0 GOTO checkError


ECHO %Lines%
ECHO Compiling DWM:
ifort %COMPOPTS% %DWM_SOURCES%   /c /object:%INTER_DIR%\ /module:%INTER_DIR%\
IF %ERRORLEVEL% NEQ 0 GOTO checkError


ECHO %Lines%
ECHO Compiling AeroDyn14:
ifort %COMPOPTS% %AD14_SOURCES%   /c /object:%INTER_DIR%\ /module:%INTER_DIR%\
IF %ERRORLEVEL% NEQ 0 GOTO checkError


ECHO %Lines%
ECHO Compiling AeroDyn:
ifort %COMPOPTS% %AFI_SOURCES% %UA_SOURCES% %BEMT_SOURCES%  %AD_SOURCES%  /c /object:%INTER_DIR%\ /module:%INTER_DIR%\
IF %ERRORLEVEL% NEQ 0 GOTO checkError


ECHO %Lines%
ECHO Compiling ElastoDyn:
ifort %COMPOPTS% %ED_SOURCES%   /c /object:%INTER_DIR%\ /module:%INTER_DIR%\
IF %ERRORLEVEL% NEQ 0 GOTO checkError

ECHO %Lines%
ECHO Compiling BeamDyn:
ifort %COMPOPTS% %BD_SOURCES%   /c /object:%INTER_DIR%\ /module:%INTER_DIR%\
IF %ERRORLEVEL% NEQ 0 GOTO checkError


ECHO %Lines%
ECHO Compiling ServoDyn:
ifort %COMPOPTS% %SrvD_SOURCES% /c /object:%INTER_DIR%\ /module:%INTER_DIR%\
IF %ERRORLEVEL% NEQ 0 GOTO checkError


ECHO %Lines%
ECHO Compiling HydroDyn:
ifort %COMPOPTS% %HD_SOURCES%   /c /object:%INTER_DIR%\ /module:%INTER_DIR%\
IF %ERRORLEVEL% NEQ 0 GOTO checkError


ECHO %Lines%
ECHO Compiling SubDyn:
ifort %COMPOPTS% %SD_SOURCES%  /c /object:%INTER_DIR%\ /module:%INTER_DIR%\
IF %ERRORLEVEL% NEQ 0 GOTO checkError


ECHO %Lines%
ECHO Compiling MAP:
ifort %COMPOPTS% %MAP_SOURCES%  /c /object:%INTER_DIR%\ /module:%INTER_DIR%\
IF %ERRORLEVEL% NEQ 0 GOTO checkError


ECHO %Lines%
ECHO Compiling FEAMooring:
ifort %COMPOPTS% %FEAM_SOURCES%  /c /object:%INTER_DIR%\ /module:%INTER_DIR%\
IF %ERRORLEVEL% NEQ 0 GOTO checkError

ECHO %Lines%
ECHO Compiling MoorDyn:
ifort %COMPOPTS% %MD_SOURCES%  /c /object:%INTER_DIR%\ /module:%INTER_DIR%\
IF %ERRORLEVEL% NEQ 0 GOTO checkError

ECHO %Lines%
ECHO Compiling OrcaFlex Interface:
ifort %COMPOPTS% %Orca_SOURCES%  /c /object:%INTER_DIR%\ /module:%INTER_DIR%\
IF %ERRORLEVEL% NEQ 0 GOTO checkError


ECHO %Lines%
ECHO Compiling IceFloe:
ifort %COMPOPTS% %IceF_SOURCES%  /c /object:%INTER_DIR%\ /module:%INTER_DIR%\
IF %ERRORLEVEL% NEQ 0 GOTO checkError

ECHO %Lines%
ECHO Compiling IceDyn:
ifort %COMPOPTS% %IceD_SOURCES%  /c /object:%INTER_DIR%\ /module:%INTER_DIR%\
IF %ERRORLEVEL% NEQ 0 GOTO checkError


rem ECHO %Lines%
rem ECHO Compiling OpenFOAM integrations:
rem ifort %COMPOPTS% %OpFM_SOURCES%  /c /object:%INTER_DIR%\ /module:%INTER_DIR%\
rem IF %ERRORLEVEL% NEQ 0 GOTO checkError

ECHO %Lines%
ECHO Compiling FAST glue code:
ifort %COMPOPTS% %FAST_SOURCES% /c /object:%INTER_DIR%\ /module:%INTER_DIR%\
IF %ERRORLEVEL% NEQ 0 GOTO checkError


:link_now
ECHO %Lines%
ECHO Linking compiled modules to form %ROOT_NAME%.exe:
ifort %INTER_DIR%\*.obj %LINKOPTS% /out:%ROOT_NAME%.exe

:checkError
ECHO %Lines%
IF %ERRORLEVEL% NEQ 0 (
ECHO Error creating %ROOT_NAME%.exe
) ELSE (
ECHO %ROOT_NAME%.exe was created.

)

GOTO END

REM ----------------------------------------------------------------------------
:: Some subroutines for the registry stuff:

:RunRegistry_HD
SET ModuleName=%1
%REGISTRY% %HD_Reg_Loc%\%ModuleName%.txt -I "%NWTC_Lib_Loc%" -I "%HD_Reg_Loc%" -O "%CURR_LOC%"
EXIT /B

:RunRegistry_fmt1
SET ModuleName=%1
%REGISTRY% %CURR_LOC%\%ModuleName%_Registry.txt -I "%NWTC_Lib_Loc%" -I "%TMD_Loc%" -O "%CURR_LOC%"
EXIT /B

:RunRegistry_IfW
SET ModuleName=%1
%REGISTRY% "%IfW_Reg_Loc%\%ModuleName%.txt" %2 -I "%NWTC_Lib_Loc%" -I "%IfW_Reg_Loc%" -O "%CURR_LOC%"
EXIT /B


:end
REM ----------------------------------------------------------------------------
REM ------------------------- CLEAR MEMORY -------------------------------------
REM ----------------------------------------------------------------------------
ECHO. 


SET BITS=
set LINES=
SET ROOT_NAME=
SET REGISTRY=

SET NWTC_Lib_Loc=
SET NETLIB_Loc=
SET ED_Loc=
SET BD_Loc=
SET SrvD_Loc=
SET TMD_Loc=
SET AD_Loc=
SET DWM_Loc=
SET IfW_Loc=
SET IfW_Reg_Loc=
SET OpFM_Loc=
SET HD_Loc=
SET HD_Reg_Loc=
SET SD_Loc=
SET MAP_Loc=
SET FAST_Loc=
SET MAP_Include_Lib=
SET FEAM_Loc=
SET MD_Loc=
SET IceF_Loc=
SET IceF_RanLux_Loc=
SET Orca_Loc=
SET Orca_Reg_Loc=

SET NWTC_SOURCES=
SET IfW_SOURCES=
SET AD_SOURCES=
SET DWM_SOURCES=
SET ED_SOURCES=
SET BD_SOURCES=
SET HD_SOURCES=
SET SrvD_SOURCES=
SET SD_SOURCES=
SET MAP_SOURCES=
SET FEAM_SOURCES=
SET Orca_SOURCES=
SET IceF_SOURCES=
SET IceD_SOURCES=
SET FAST_SOURCES=

SET COMPOPTS=
SET LINKOPTS=

SET INTER_DIR=
SET CURR_LOC=
SET REGONLY=
SET MODULENAME=
:Done
EXIT /B