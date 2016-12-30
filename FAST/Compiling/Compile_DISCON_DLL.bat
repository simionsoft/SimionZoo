@ECHO OFF

REM Syntax for this script is:
REM
REM  Compile_DISCON_DLL FileName [64]
REM
REM square brackets [ ] are used to indicate that the values inside are optional.
REM
REM  Examples:
REM    Compile_DISCON_DLL Discon 64
REM    Compile_DISCON_DLL Discon_ITIBarge
REM    Compile_DISCON_DLL Discon_OC3Hywind
REM
REM  Note that the FileName is assumed to end in .f90. It is also hard-coded to
REM  look for the file in FilePath = ..\CertTest\5MW_Baseline\ServoData\Source\

REM ----------------------------------------------------------------------------
REM                   set compiler internal variables
REM ----------------------------------------------------------------------------
REM    You can run this bat file from the IVF compiler's command prompt (and not
REM    do anything in this section). If you choose not to run from the IVF command
REM    prompt, you must call the compiler's script to set internal variables.
REM    TIP: Right click on the IVF Compiler's Command Prompt shortcut, click
REM    properties, and copy the target (without cmd.exe and/or its switches) here:

ECHO.

IF "%INCLUDE%"=="" (

IF /I "%2"=="64" ( ECHO //// Using intel64 compiler \\\\
CALL "C:\Program Files (x86)\Intel\Composer XE 2011 SP1\bin\ipsxe-comp-vars.bat" intel64 vs2010
SET Suffix=_x64
) ELSE ( ECHO  //// Using ia32 compiler \\\\
CALL "C:\Program Files (x86)\Intel\Composer XE 2011 SP1\bin\ipsxe-comp-vars.bat" ia32 vs2010
SET Suffix=_Win32
)

) ELSE ( ECHO //// Using existing compiler settings \\\\

IF /I "%2"=="64" ( ECHO //// assuming intel64 \\\\
SET Suffix=_x64
) ELSE ( ECHO  //// assuming ia32 \\\\
SET Suffix=_Win32
)


)

REM ----------------------------------------------------------------------------
REM                   set paths and file names
REM ----------------------------------------------------------------------------

SET FilePath=..\CertTest\5MW_Baseline\ServoData\Source\
SET InFileName=%1
rem SET OutFileName=%FilePath%..\%InFileName%%Suffix%
SET OutFileName=%InFileName%%Suffix%
SET InFileName=%FilePath%%InFileName%.f90



REM ----------------------------------------------------------------------------
REM                   set compiler and linker options
REM ----------------------------------------------------------------------------

REM Use /libs:static if the DLL will be used on other PCs. (will result in a
REM   larger dll than using /libs:dll)

REM "/iface:cref /names:uppercase" is the same as "iface:default"

SET CompOpts= /nologo /O2 /inline /traceback /libs:static /threads /iface:cref /names:uppercase
rem SET CompOpts= /nologo /O2 /inline /traceback /libs:dll /threads /iface:cref /names:uppercase


rem SET CompOpts= /nologo /O2 /inline /traceback /libs:dll /threads
rem SET LinkOpts=/link


REM ----------------------------------------------------------------------------
REM                   compile DLL
REM ----------------------------------------------------------------------------

ECHO.
ECHO Creating %OutFileName%.dll:

CALL IFORT /dll %CompOpts% %InFileName% /exe:%OutFileName%.dll

ECHO.


REM ----------------------------------------------------------------------------
REM                   clear variables
REM ----------------------------------------------------------------------------

DEL *.obj

SET CompOpts=
SET LinkOpts=
SET InFileName=
SET OutFileName=
SET FilePath=



