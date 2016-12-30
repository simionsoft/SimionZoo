@ECHO OFF

set lines=======================================================================
echo %lines%
IF "%1"=="" (
ECHO.
ECHO   The calling syntax for this script is
ECHO             RunRegistry ModuleName
ECHO.
GOTO Done
)


REM ----------------------------------------------------------------------------
REM ------------------------- LOCAL PATHS --------------------------------------
REM ----------------------------------------------------------------------------
REM -- USERS MAY EDIT THESE PATHS TO POINT TO FOLDERS ON THEIR LOCAL MACHINES. -
REM -- NOTE: do not use quotation marks around the path names!!!! --------------
REM ----------------------------------------------------------------------------
REM ----------------------------------------------------------------------------

SET FAST_Loc=..\..\Source
SET Registry=..\..\bin\Registry_win32.exe

SET NWTC_Lib_Loc=%FAST_Loc%\dependencies\NWTC_Library
SET ED_Loc=%FAST_Loc%\dependencies\ElastoDyn
SET BD_Loc=%FAST_Loc%\dependencies\BeamDyn
SET SrvD_Loc=%FAST_Loc%\dependencies\ServoDyn
SET TMD_Loc=%SrvD_Loc%
SET AD_Loc=%FAST_Loc%\dependencies\AeroDyn
SET BEMT_Loc=%AD_Loc%
SET UA_Loc=%AD_Loc%
SET AFI_Loc=%AD_Loc%
SET AD14_Loc=%FAST_Loc%\dependencies\AeroDyn14
SET DWM_Loc=%AD14_Loc%
SET IfW_Loc=%FAST_Loc%\dependencies\InflowWind
SET HD_Loc=%FAST_Loc%\dependencies\HydroDyn
SET SD_Loc=%FAST_Loc%\dependencies\SubDyn
SET MAP_Loc=%FAST_Loc%\dependencies\MAP
SET FEAM_Loc=%FAST_Loc%\dependencies\FEAMooring
SET IceF_Loc=%FAST_Loc%\dependencies\IceFloe
SET IceD_Loc=%FAST_Loc%\dependencies\IceDyn
SET MD_Loc=%FAST_Loc%\dependencies\MoorDyn
SET OpFM_Loc=%FAST_Loc%\dependencies\OpenFOAM
SET Orca_Loc=%FAST_Loc%\dependencies\OrcaFlex

SET HD_Reg_Loc=%HD_Loc%
SET IfW_Reg_Loc=%IfW_Loc%
SET FEAM_Reg_Loc=%FEAM_Loc%
SET Orca_Reg_Loc=%Orca_Loc%

SET MAP_Loc_R=%MAP_Loc%


IF /I "%2"=="dev" CALL ..\Set_FAST_paths.bat

SET ModuleName=%1

GOTO %ModuleName%

REM ----------------------------------------------------------------------------
REM ---------------- RUN THE REGISTRY TO AUTO-GENERATE FILES -------------------
REM ----------------------------------------------------------------------------
:FAST
ECHO on
SET CURR_LOC=%FAST_Loc%
%REGISTRY% "%CURR_LOC%\FAST_Registry.txt" -I "%NWTC_Lib_Loc%" -I "%ED_Loc%" -I "%SrvD_Loc%" -I "%AD14_Loc%" -I^
 "%AD_Loc%" -I "%BEMT_Loc%" -I "%UA_Loc%" -I "%AFI_Loc%" -I "%BD_Loc%" -I^
 "%IfW_Reg_Loc%" -I "%DWM_LOC%" -I "%SD_Loc%" -I "%HD_Reg_Loc%" -I "%MAP_Loc_R%" -I "%FEAM_Reg_Loc%"  -I^
 "%IceF_Loc%" -I "%IceD_Loc%" -I "%TMD_Loc%" -I "%MD_Loc%" -I "%OpFM_Loc%" -I "%Orca_Reg_Loc%" -noextrap -O "%CURR_LOC%"

echo off
GOTO checkError


:BeamDyn
SET CURR_LOC=%BD_Loc%
%REGISTRY% "%CURR_LOC%\Registry_BeamDyn.txt" -I "%NWTC_Lib_Loc%" -O "%CURR_LOC%"
GOTO checkError


:ElastoDyn
SET CURR_LOC=%ED_Loc%
%REGISTRY% "%CURR_LOC%\%ModuleName%_Registry.txt" -I "%NWTC_Lib_Loc%" -O "%CURR_LOC%"
GOTO checkError


:ServoDyn
SET CURR_LOC=%SrvD_Loc%
%REGISTRY% "%CURR_LOC%\%ModuleName%_Registry.txt" -I "%NWTC_Lib_Loc%" -I "%TMD_Loc%" -O "%CURR_LOC%"
GOTO checkError

:TMD
SET CURR_LOC=%TMD_Loc%
%REGISTRY% "%CURR_LOC%\%ModuleName%_Registry.txt" -I "%NWTC_Lib_Loc%" -O "%CURR_LOC%"
GOTO checkError


:InflowWind
:Lidar
SET CURR_LOC=%IfW_Loc%
%REGISTRY% "%IfW_Reg_Loc%\%ModuleName%.txt" -I "%NWTC_Lib_Loc%" -I "%IfW_Reg_Loc%" -O "%CURR_LOC%"
GOTO checkError


:IfW_TSFFWind
:IfW_HAWCWind
:IfW_BladedFFWind
:IfW_UniformWind
:IfW_UserWind
SET CURR_LOC=%IfW_Loc%
%REGISTRY% "%IfW_Reg_Loc%\%ModuleName%.txt" -I "%NWTC_Lib_Loc%" -I "%IfW_Reg_Loc%" -noextrap  -O "%CURR_LOC%"
GOTO checkError


:OpenFOAM
SET CURR_LOC=%OpFM_Loc%
%REGISTRY% "%CURR_LOC%\OpenFOAM_Registry.txt" -I "%NWTC_Lib_Loc%" -ccode -O "%CURR_LOC%"
GOTO checkError


:AeroDyn
SET CURR_LOC=%AD_Loc%
%REGISTRY% "%CURR_LOC%\AeroDyn_Registry.txt" -I "%NWTC_Lib_Loc%" -I "%BEMT_Loc%" -I "%UA_Loc%" -I "%AFI_Loc%" -O "%CURR_LOC%"
GOTO checkError

:BEMT
SET CURR_LOC=%BEMT_Loc%
%REGISTRY% "%CURR_LOC%\BEMT_Registry.txt" -I "%NWTC_Lib_Loc%" -I "%UA_Loc%" -I "%AFI_Loc%" -O "%CURR_LOC%"
GOTO checkError

:AFI
SET CURR_LOC=%AFI_Loc%
%REGISTRY% "%CURR_LOC%\AirfoilInfo_Registry.txt" -I "%NWTC_Lib_Loc%" -noextrap -O "%CURR_LOC%"
GOTO checkError

:UA
SET CURR_LOC=%UA_Loc%
%REGISTRY% "%CURR_LOC%\UnsteadyAero_Registry.txt" -I "%NWTC_Lib_Loc%" -I "%AFI_Loc%" -O "%CURR_LOC%"
GOTO checkError


:AeroDyn14
SET CURR_LOC=%AD14_Loc%
%REGISTRY% "%CURR_LOC%\Registry-AD14.txt" -I "%NWTC_Lib_Loc%" -I "%AD14_Loc%" -I "%DWM_Loc%" -I "%IfW_Reg_Loc%" -O "%CURR_LOC%"
GOTO checkError

:DWM
SET CURR_LOC=%DWM_Loc%
%REGISTRY% "%CURR_LOC%\Registry-DWM.txt" -I "%NWTC_Lib_Loc%" -I "%IfW_Reg_Loc%"  -O "%CURR_LOC%"
GOTO checkError

:HydroDyn
:Current
:Waves
:Waves2
:SS_Radiation
:Conv_Radiation
:WAMIT
:WAMIT2
:Morison
SET CURR_LOC=%HD_Loc%
%REGISTRY% "%HD_Reg_Loc%\%ModuleName%.txt" -I "%NWTC_Lib_Loc%" -I "%HD_Reg_Loc%"  -O "%CURR_LOC%"
GOTO checkError


:SubDyn
SET CURR_LOC=%SD_Loc%
%REGISTRY% "%CURR_LOC%\%ModuleName%_Registry.txt" -I "%NWTC_Lib_Loc%"  -O "%CURR_LOC%"
GOTO checkError

:MAP
SET CURR_LOC=%MAP_Loc_R%
IF /I "%2"=="dev" (
%REGISTRY% "%CURR_LOC%\%ModuleName%_Registry.txt" -ccode -I "%NWTC_Lib_Loc%"  -O "%CURR_LOC%"
)
GOTO checkError

:FEAMooring
SET CURR_LOC=%FEAM_Loc%
%REGISTRY% "%FEAM_Reg_LOC%\FEAM_Registry.txt" -I "%NWTC_Lib_Loc%"  -O "%CURR_LOC%"
GOTO checkError

:MoorDyn
SET CURR_LOC=%MD_Loc%
%REGISTRY% "%CURR_LOC%\MoorDyn_Registry.txt" -I "%NWTC_Lib_Loc%"  -O "%CURR_LOC%"
GOTO checkError


:IceFloe
SET CURR_LOC=%IceF_Loc%
%REGISTRY% "%CURR_LOC%\IceFloe_FASTRegistry.inp" -I "%NWTC_Lib_Loc%"  -O "%CURR_LOC%"
GOTO checkError


:IceDyn
SET CURR_LOC=%IceD_Loc%
%REGISTRY% "%CURR_LOC%\Registry_%ModuleName%.txt" -I "%NWTC_Lib_Loc%"  -O "%CURR_LOC%"
GOTO checkError


:OrcaFlexInterface
SET CURR_LOC=%Orca_Loc%
%REGISTRY% "%Orca_Reg_Loc%\%ModuleName%.txt" -I "%NWTC_Lib_Loc%" -I "%Orca_Reg_Loc%"  -O "%CURR_LOC%"
GOTO checkError


:checkError
ECHO.
IF %ERRORLEVEL% NEQ 0 (
ECHO Error running FAST Registry for %ModuleName%.
) ELSE (
ECHO Registry for %ModuleName% completed.
REM COPY /Y "%ModuleName%_Types.f90"   "%CURR_LOC%"
rem IF /I "%ModuleName%"=="MAP" COPY /Y "%ModuleName%_Types.h" "%CURR_LOC%"
)




:end
REM ----------------------------------------------------------------------------
REM ------------------------- CLEAR MEMORY -------------------------------------
REM ----------------------------------------------------------------------------
ECHO. 


SET REGISTRY=

SET NWTC_Lib_Loc=
SET ED_Loc=
SET SrvD_Loc=
SET AD14_Loc=
SET AD_Loc=
SET AFI_Loc=
SET BEMT_Loc=
SET UA_Loc=
SET DWM_Loc=
SET IfW_Loc=
SET HD_Loc=
SET SD_Loc=
SET MAP_Loc=
SET FEAM_Loc=
SET IceF_Loc=
SET ID_Loc=
SET FAST_Loc=
SET MAP_Include_Lib=
SET HD_Reg_Loc=
SET IfW_Reg_Loc=
SET FEAM_Reg_Loc=
SET MD_Loc=
SET OpFM_Loc=
SET Orca_Loc=
SET Orca_Reg_Loc=

SET ModuleName=
SET CURR_LOC=
:Done
echo %lines%
set lines=