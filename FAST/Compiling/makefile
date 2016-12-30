#================================================================================#
# This makefile created by B. Jonkman.                                           #
# (c) 2013-2015 National Renewable Energy Laboratory                             #
#                                                                                #
# It has been tested on                                                          #
#     Windows 7 with gfortran using mingw32-make.exe.                            #
#                                                                                #
#================================================================================#


   # Name of compiler to use and flags to use.
FC      = gfortran

# 32-bit or 64-bit?
BITS = 32
#BITS = 64

#================================================================================#
# if compiling FAST as a shared library, use
# FAST_driver = FAST_Library
# to generate a stand-alone version of FAST, use
FAST_driver = FAST_Prog
#================================================================================#



FFLAGS  = -O2 -m$(BITS) -fbacktrace -ffree-line-length-none -x f95-cpp-input
LDFLAGS = -O2 -m$(BITS) -fbacktrace

#FFLAGS  = -O0 -m$(BITS) -fbacktrace -ffree-line-length-none -x f95-cpp-input -DDOUBLE_PRECISION  -fdefault-real-8 -g -pg
#LDFLAGS = -O0 -m$(BITS) -fbacktrace  -g -pg

#.........................................
# preprocessor definitions:
#  -DDOUBLE_PRECISION
#
# some useful gfortran options:
# -Wl,--stack=999999999,--large-address-aware
#
#  -DFPE_TRAP_ENABLED
#  -Wl,--large-address-aware              # not necessary when $(BITS) is 64 (64-bit target)
#  -Wl,--stack=999999999                  # not necessary when $(BITS) is 64
#  -Wconversion-extra -Wconversion
#  -fdefault-real-8                       # note this also promotes doubles to 16-bytes if possible
#  -fcheck=bounds,do,mem,pointer -std=f2003 -O0 -v -Wall
#  -pg                                    # generate debugging info for debugger
# http://gcc.gnu.org/onlinedocs/gfortran/Option-Index.html#Option-Index
#
# makefile options:
#  --warn-undefined-variables
#.........................................


   # Location of source files for FAST, and its modules.

FAST_DIR     = ../Source

NWTC_Lib_DIR = $(FAST_DIR)/dependencies/NWTC_Library
NETLIB_DIR   = $(FAST_DIR)/dependencies/NetLib
ED_DIR       = $(FAST_DIR)/dependencies/ElastoDyn
BD_DIR       = $(FAST_DIR)/dependencies/BeamDyn
SrvD_DIR     = $(FAST_DIR)/dependencies/ServoDyn
AD14_DIR     = $(FAST_DIR)/dependencies/AeroDyn14
AD_DIR       = $(FAST_DIR)/dependencies/AeroDyn
IfW_DIR      = $(FAST_DIR)/dependencies/InflowWind
OpFM_DIR     = $(FAST_DIR)/dependencies/OpenFOAM
HD_DIR       = $(FAST_DIR)/dependencies/HydroDyn
SD_DIR       = $(FAST_DIR)/dependencies/SubDyn
MAP_DIR      = $(FAST_DIR)/dependencies/MAP
FEAM_DIR     = $(FAST_DIR)/dependencies/FEAMooring
MD_DIR       = $(FAST_DIR)/dependencies/MoorDyn
Orca_DIR     = $(FAST_DIR)/dependencies/OrcaFlex
IceF_DIR     = $(FAST_DIR)/dependencies/IceFloe
IceD_DIR     = $(FAST_DIR)/dependencies/IceDyn

TMD_DIR      = $(SrvD_DIR)
DWM_DIR      = $(AD14_DIR)
AFI_DIR      = $(AD_DIR)
UA_DIR       = $(AD_DIR)
BEMT_DIR     = $(AD_DIR)

HD_DIR_Reg   = $(HD_DIR)
IfW_DIR_Reg  = $(IfW_DIR)
MAP_DIR_Reg  = $(MAP_DIR)
Orca_DIR_Reg = $(Orca_DIR)

BIN_DIR      = ../bin



   # Names and locations of the Registry, MAP libraries, and instructions for linking with LAPACK.
   # You will probably need to change these for your system.

ifeq ($(OS),Windows_NT)

   Registry     = $(BIN_DIR)/Registry_win32.exe
   MAP_lib      = $(MAP_DIR)/MAP_win32.lib
   LDFLAGS     := $(LDFLAGS) -llapack -lblas -LC:/LAPACK/win32
   LAPACK_LINK :=

   UNAME       := $(OS)
else

   Registry     = $(FAST_DIR)/dependencies/registry.exe
   MAP_lib      = $(BIN_DIR)/libmap-1.10.01.so

   UNAME := $(shell uname -s)

   ifeq ($(UNAME), Darwin)
         # Mac needs some additional flags prepended for linking to get lapack working:
      LDFLAGS     := -framework Accelerate $(LDFLAGS)
      LAPACK_LINK :=
      #DLL_EXT     := dylib
   else
        # ifeq ($(UNAME), Linux)
        # bjj verify this: # I've been told that the lapack linking needs to occur at the end of the line (after all the object files) on linux:
      LAPACK_LINK  = -llapack -lblas
      #DLL_EXT     := so
   endif

  # some options to dynamically load .so libraries (loaded when called in ServoDyn) for DLL control
  # the dl library (-ldl) is used for dynamic libraries; needs to be used in conjunction with preprocessor directive USE_DLL_INTERFACE
  FFLAGS := $(FFLAGS) -DUSE_DLL_INTERFACE
  LAPACK_LINK := $(LAPACK_LINK) -ldl

endif


   # Destination and RootName for executable

DEST_DIR    = $(BIN_DIR)

ifeq ($(FAST_driver),FAST_Library)
   LDFLAGS := $(LDFLAGS) -shared
   OUTPUT_NAME = FAST_Library

   ifeq ($(UNAME), Linux)
      LDFLAGS := $(LDFLAGS) -fPIC
   endif
else
   OUTPUT_NAME = FAST
endif


   #==========================================================#
   # You should not need to change anything beyond this point #
   #==========================================================#

   # System-specific settings.

ifeq ($(OS),Windows_NT)
      # Windows
   DEL_CMD   = del
   INTER_DIR = Obj_win$(BITS)
   MD_CMD    = @mkdir
   CP_CMD    = copy
   OBJ_EXT   = .obj
   PATH_SEP  = \\
   SYS_FILE  = SysGnuWin

   ifeq ($(FAST_driver),FAST_Library)
      EXE_EXT   = _gwin$(BITS).dll
   else
      EXE_EXT   = _gwin$(BITS).exe
   endif


else
      # Linux/Mac
   DEL_CMD   = rm -f
   INTER_DIR = Obj_lin$(BITS)
   MD_CMD    = @mkdir -p
   CP_CMD    = cp
   OBJ_EXT   = .o
   PATH_SEP  = /
   SYS_FILE  = SysGnuLinux

   ifeq ($(FAST_driver),FAST_Library)
      EXE_EXT   = _glin$(BITS).so
   else
      EXE_EXT   = _glin$(BITS)
   endif

endif


   # Source files (by module)

LIB_SOURCES =                  \
	SingPrec.f90                \
	NWTC_Base.f90               \
	$(SYS_FILE).f90             \
	NWTC_Library_Types.f90      \
	NWTC_IO.f90                 \
	NWTC_Num.f90                \
	ModMesh_Types.f90           \
	ModMesh.f90                 \
	ModMesh_Mapping.f90         \
	NWTC_Library.f90

NETLIB_SOURCES=                \
	NWTC_ScaLAPACK.f90          \
	NWTC_FFTPACK.f90            \
	NWTC_LAPACK.f90             \
	fftpack4.1.f                \
	dlasrt2.f                   \
	slasrt2.f                   \
	NWTC_FitPack.f90            \
	dierckx_fitpack.f


IfW_SOURCES =                  \
	IfW_BladedFFWind_Types.f90  \
	IfW_BladedFFWind.f90        \
	IfW_TSFFWind_Types.f90      \
	IfW_TSFFWind.f90            \
	IfW_HAWCWind_Types.f90      \
	IfW_HAWCWind.f90            \
	IfW_UniformWind_Types.f90   \
	IfW_UniformWind.f90         \
	IfW_UserWind_Types.f90      \
	IfW_UserWind.f90            \
	InflowWind_Subs.f90         \
	InflowWind_Types.f90        \
	Lidar_Types.f90             \
	Lidar.f90                   \
	InflowWind.f90


OpFM_SOURCES =                 \
	OpenFOAM_Types.f90          \
	OpenFOAM.f90

AD14_SOURCES =                 \
	AeroDyn14_Types.f90         \
	GenSubs.f90                 \
	AeroSubs.f90                \
	AeroDyn14.f90

DWM_SOURCES =                  \
	DWM_Types.f90               \
	DWM.f90                     \
	DWM_Wake_Sub_ver2.f90

AFI_SOURCES =                  \
	AirfoilInfo_Types.f90       \
	AirfoilInfo.f90

UA_SOURCES =                   \
	UnsteadyAero_Types.f90      \
	UnsteadyAero.f90

BEMT_SOURCES =                 \
	BEMT_Types.f90              \
	BladeElement.f90            \
	BEMTCoupled.f90             \
	BEMTUncoupled.f90           \
	Brent.f90                   \
	fmin_fcn.f90                \
	mod_root1dim.f90            \
	nwtc_bemt_minpack.f90       \
	BEMT.f90

AD_SOURCES =                   \
	AeroDyn_Types.f90           \
	AeroDyn_IO.f90              \
	AeroDyn.f90

BD_SOURCES =                   \
	BeamDyn_Types.f90           \
	BeamDyn_IO.f90              \
	BeamDyn_Subs.f90            \
	BeamDyn.f90

HD_SOURCES =                   \
	SS_Radiation_Types.f90      \
	SS_Radiation.f90            \
	Waves2_Types.f90            \
	Waves2_Output.f90           \
	Waves2.f90                  \
	Waves_Types.f90             \
	UserWaves.f90               \
	Waves.f90                   \
	Current_Types.f90           \
	Current.f90                 \
	Morison_Types.f90           \
	Morison_Output.f90          \
	Morison.f90                 \
	Conv_Radiation_Types.f90    \
	Conv_Radiation.f90          \
	WAMIT2_Types.f90            \
	WAMIT2_Output.f90           \
	WAMIT2.f90                  \
	WAMIT_Types.f90             \
	WAMIT_Interp.f90            \
	WAMIT_Output.f90            \
	WAMIT.f90                   \
	HydroDyn_Output.f90         \
	HydroDyn_Types.f90          \
	HydroDyn_Input.f90          \
	HydroDyn.f90

MAP_SOURCES =                  \
	MAP_Types.f90               \
	MAP.f90

FEAM_SOURCES =                 \
	FEAMooring_Types.f90        \
	FEAM.f90

MD_SOURCES =                   \
	MoorDyn_Types.f90           \
	MoorDyn_IO.f90              \
	MoorDyn.f90

Orca_SOURCES =                 \
	OrcaFlexInterface_Types.f90 \
	OrcaFlexInterface.f90

SD_SOURCES =                   \
	qsort_c_module.f90          \
	SD_FEM.f90                  \
	SubDyn_Types.f90            \
	SubDyn_Output.f90           \
	SubDyn.f90


ED_SOURCES =                   \
	ElastoDyn_Types.f90         \
	ElastoDyn.f90

SrvD_SOURCES =                 \
	TMD_Types.f90               \
	TMD.f90                     \
	ServoDyn_Types.f90          \
	ServoDyn.f90                \
	PitchCntrl_ACH.f90          \
	BladedInterface.f90         \
	UserSubs.f90                \
	UserVSCont_KP.f90


IceF_SOURCES =                 \
	IceFloe_Types.f90           \
	iceLog.F90                  \
	coupledCrushing.F90         \
	crushingIEC.F90             \
	crushingISO.F90             \
	IceFlexBase.F90             \
	IceFlexIEC.f90              \
	IceFlexISO.f90              \
	IceFloeBase.F90             \
	iceInput.f90                \
	intermittentCrushing.F90    \
	lockInISO.F90               \
	randomCrushing.F90          \
	RANLUX.f90                  \
	IceFloe.f90

IceD_SOURCES =                 \
	IceDyn_Types.f90            \
	IceDyn.f90


FAST_SOURCES =                 \
	FAST_Types.f90              \
	FAST_Mods.f90               \
	FAST_Subs.f90               \
	$(FAST_driver).f90

vpath %.f90       $(NWTC_Lib_DIR) $(AD14_DIR) $(IfW_DIR) $(HD_DIR) $(ED_DIR) $(SrvD_DIR) $(TMD_DIR) $(SD_DIR) $(MAP_DIR) \
                  $(FAST_DIR) $(NETLIB_DIR) $(FEAM_DIR) $(MD_DIR) $(IceF_DIR) $(IceD_DIR) $(DWM_DIR) $(AFI_DIR) $(UA_DIR) \
                  $(BEMT_DIR) $(AD_DIR) $(BD_DIR) $(OpFM_DIR) $(Orca_DIR)
vpath %.f         $(NETLIB_DIR)
vpath %.mod       $(INTER_DIR)
vpath %$(OBJ_EXT) $(INTER_DIR)
vpath %.txt       $(FAST_DIR) $(AD14_DIR) $(IfW_DIR_Reg) $(HD_DIR_Reg) $(SD_DIR) $(ED_DIR) $(SrvD_DIR) $(TMD_DIR) $(IceD_DIR) \
                  $(DWM_DIR) $(FEAM_DIR) $(MD_DIR) $(MAP_DIR) $(AFI_DIR) $(UA_DIR) $(BEMT_DIR) $(AD_DIR) $(BD_DIR) $(OpFM_DIR)
vpath %.inp       $(IceF_DIR) $(Orca_DIR_Reg)
#apparently vpath doesn't work as expected if I use %.F90 (is it not case sensitive???)
#vpath %.F90      $(IceF_DIR)
vpath %           $(IceF_DIR)


ALL_SOURCES = $(FAST_SOURCES) $(LIB_SOURCES) $(NETLIB_SOURCES) $(IfW_SOURCES) $(AD14_SOURCES) $(DWM_SOURCES) \
              $(OpFM_SOURCES) $(SrvD_SOURCES) $(ED_SOURCES) $(HD_SOURCES)  $(SD_SOURCES) \
              $(AFI_SOURCES) $(UA_SOURCES) $(BEMT_SOURCES) $(AD_SOURCES) \
              $(MAP_SOURCES) $(FEAM_SOURCES) $(MD_SOURCES) $(Orca_SOURCES) $(IceF_SOURCES) $(IceD_SOURCES) $(BD_SOURCES)
ALL_OBJS    = $(ALL_SOURCES:.f90=$(OBJ_EXT))
ALL_OBJS   := $(ALL_OBJS:.F90=$(OBJ_EXT))       #note the upper case here (from IceFloe)
ALL_OBJS   := $(ALL_OBJS:.f=$(OBJ_EXT))


   # Rule to do everything.

all:     default
# use this for "all" for debugging: print variables:
#all:     ; $(info $$IceF_SOURCES is [${IceF_SOURCES}]) echo debugging
#
default:  $(INTER_DIR) $(DEST_DIR)/$(OUTPUT_NAME)$(EXE_EXT)


   # General rules for compiling the files.

%$(OBJ_EXT): %.f90
	$(FC) -I $(INTER_DIR) $(FFLAGS) -c $< -o $(INTER_DIR)/$@ -J $(INTER_DIR) -B $(INTER_DIR)

%$(OBJ_EXT): %.F90
	$(FC) -I $(INTER_DIR) $(FFLAGS) -c $< -o $(INTER_DIR)/$@ -J $(INTER_DIR) -B $(INTER_DIR)

%$(OBJ_EXT): %.f
	$(FC) -I $(INTER_DIR) $(FFLAGS) -c $< -o $(INTER_DIR)/$@ -J $(INTER_DIR) -B $(INTER_DIR)



   # General rules for creating _Types.f90 files from Registry files.

$(FAST_DIR)/FAST_Types.f90: $(FAST_DIR)/FAST_Registry.txt
	$(Registry) $< -I $(NWTC_Lib_DIR) -I $(ED_DIR) -I $(SrvD_DIR) -I $(SD_DIR) -I $(TMD_DIR) -I $(AD14_DIR) -I $(AD_DIR) -I $(BD_DIR) \
	-I $(HD_DIR_Reg) -I $(IceF_DIR) -I $(IceD_DIR) -I $(MAP_DIR) -I $(FEAM_DIR) -I $(MD_DIR) -I $(IfW_DIR_Reg) -I $(DWM_DIR) -I $(OpFM_DIR) \
	-I $(Orca_DIR_Reg) -noextrap -O $(FAST_DIR)

$(TMD_DIR)/TMD_Types.f90: $(TMD_DIR)/TMD_Registry.txt
	$(Registry) $< -I $(NWTC_Lib_DIR) -O $(TMD_DIR)

$(SrvD_DIR)/ServoDyn_Types.f90: $(SrvD_DIR)/ServoDyn_Registry.txt
	$(Registry) $< -I $(NWTC_Lib_DIR) -I $(TMD_DIR) -O $(SrvD_DIR)

$(ED_DIR)/ElastoDyn_Types.f90: $(ED_DIR)/ElastoDyn_Registry.txt
	$(Registry) $< -I $(NWTC_Lib_DIR)  -O $(ED_DIR)

$(BD_DIR)/BeamDyn_Types.f90: $(BD_DIR)/Registry_BeamDyn.txt
	$(Registry) $< -I $(NWTC_Lib_DIR)  -O $(BD_DIR)

$(AD14_DIR)/AeroDyn14_Types.f90: $(AD14_DIR)/Registry-AD14.txt
	$(Registry) $< -I $(NWTC_Lib_DIR) -I $(IfW_DIR_Reg) -I $(DWM_DIR)  -O $(AD14_DIR)

$(DWM_DIR)/DWM_Types.f90: $(DWM_DIR)/Registry-DWM.txt
	$(Registry) $< -I $(NWTC_Lib_DIR) -I $(IfW_DIR_Reg)  -O $(DWM_DIR)

$(AD_DIR)/AeroDyn_Types.f90: $(AD_DIR)/AeroDyn_Registry.txt
	$(Registry) $< -I $(NWTC_Lib_DIR) -I $(AFI_DIR) -I $(UA_DIR) -I $(BEMT_DIR) -O $(AD_DIR)

$(AFI_DIR)/AirfoilInfo_Types.f90: $(AFI_DIR)/AirfoilInfo_Registry.txt
	$(Registry) $< -I $(NWTC_Lib_DIR) -O $(AFI_DIR) -noextrap

$(UA_DIR)/UnsteadyAero_Types.f90: $(UA_DIR)/UnsteadyAero_Registry.txt
	$(Registry) $< -I $(NWTC_Lib_DIR) -I $(AFI_DIR) -O $(UA_DIR)

$(BEMT_DIR)/BEMT_Types.f90: $(BEMT_DIR)/BEMT_Registry.txt
	$(Registry) $< -I $(NWTC_Lib_DIR) -I $(AFI_DIR) -I $(UA_DIR) -O $(BEMT_DIR)

$(SD_DIR)/SubDyn_Types.f90: $(SD_DIR)/SubDyn_Registry.txt
	$(Registry) $< -I $(NWTC_Lib_DIR)  -O $(SD_DIR)

$(MAP_DIR)/MAP_Types.f90: $(MAP_DIR_Reg)/MAP_Registry.txt
	$(Registry) $< -ccode -I $(NWTC_Lib_DIR) -O $(MAP_DIR)

$(FEAM_DIR)/FEAMooring_Types.f90: $(FEAM_DIR)/FEAM_Registry.txt
	$(Registry) $< -I $(NWTC_Lib_DIR) -O $(FEAM_DIR)

$(MD_DIR)/MoorDyn_Types.f90: $(MD_DIR)/MoorDyn_Registry.txt
	$(Registry) $< -I $(NWTC_Lib_DIR)  -O $(MD_DIR)

$(IceF_DIR)/IceFloe_Types.f90: $(IceF_DIR)/IceFloe_FASTRegistry.inp
	$(Registry) $< -I $(NWTC_Lib_DIR)  -O $(IceF_DIR)

$(IceD_DIR)/IceDyn_Types.f90: $(IceD_DIR)/Registry_IceDyn.txt
	$(Registry) $< -I $(NWTC_Lib_DIR)  -O $(IceD_DIR)

$(OpFM_DIR)/OpenFOAM_Types.f90: $(OpFM_DIR)/OpenFOAM_Registry.txt
	$(Registry) $< -I $(NWTC_Lib_DIR) -ccode -O $(OpFM_DIR)

$(Orca_DIR)/OrcaFlexInterface_Types.f90: $(Orca_DIR_Reg)/OrcaFlexInterface.txt
	$(Registry) $< -I $(NWTC_Lib_DIR) -I $(Orca_DIR_Reg)  -O $(Orca_DIR)


   # InflowWind Registry files:
$(IfW_DIR)/InflowWind_Types.f90: $(IfW_DIR_Reg)/InflowWind.txt
	$(Registry) $< -I $(NWTC_Lib_DIR) -I $(IfW_DIR_Reg) -O $(IfW_DIR)

$(IfW_DIR)/Lidar_Types.f90: $(IfW_DIR_Reg)/Lidar.txt
	$(Registry) $< -I $(NWTC_Lib_DIR) -O $(IfW_DIR)

$(IfW_DIR)/IfW_%_Types.f90: $(IfW_DIR_Reg)/IfW_%.txt
	$(Registry) $< -I $(NWTC_Lib_DIR) -noextrap  -O $(IfW_DIR)


   # HydroDyn Registry files:
$(HD_DIR)/%_Types.f90: $(HD_DIR_Reg)/%.txt
	$(Registry) $< -I $(NWTC_Lib_DIR) -I $(HD_DIR_Reg)  -O $(HD_DIR)


   #  Dependency rules.
#NWTC Library dependency rules:
NWTC_Base$(OBJ_EXT):              SingPrec$(OBJ_EXT)
$(SYS_FILE)$(OBJ_EXT):            NWTC_Base$(OBJ_EXT)
NWTC_Library_Types$(OBJ_EXT):     $(SYS_FILE)$(OBJ_EXT)
NWTC_IO$(OBJ_EXT):                NWTC_Library_Types$(OBJ_EXT)
NWTC_Num$(OBJ_EXT):               NWTC_IO$(OBJ_EXT)
ModMesh_Types$(OBJ_EXT):          NWTC_Num$(OBJ_EXT)
ModMesh$(OBJ_EXT):                ModMesh_Types$(OBJ_EXT)
ModMesh_Mapping$(OBJ_EXT):        ModMesh$(OBJ_EXT) NWTC_LAPACK$(OBJ_EXT)
NWTC_Library$(OBJ_EXT):           ModMesh$(OBJ_EXT)  ModMesh_Mapping$(OBJ_EXT)

NWTC_LAPACK$(OBJ_EXT):            NWTC_Base$(OBJ_EXT)
NWTC_ScaLAPACK$(OBJ_EXT):         NWTC_Base$(OBJ_EXT) dlasrt2$(OBJ_EXT) slasrt2$(OBJ_EXT)
NWTC_FFTPACK$(OBJ_EXT):           NWTC_Library$(OBJ_EXT) fftpack4.1$(OBJ_EXT)
#fftpack4.1$(OBJ_EXT):             SingPrec$(OBJ_EXT)


NWTC_FitPack$(OBJ_EXT):           NWTC_Library$(OBJ_EXT) dierckx_fitpack$(OBJ_EXT)


#InflowWind dependency rules:
IfW_UniformWind_Types$(OBJ_EXT):    NWTC_Library$(OBJ_EXT) $(IfW_DIR)/IfW_UniformWind_Types.f90
IfW_IniformWind$(OBJ_EXT):          NWTC_Library$(OBJ_EXT) IfW_UniformWind_Types$(OBJ_EXT)
IfW_TSFFWind_Types$(OBJ_EXT):       NWTC_Library$(OBJ_EXT) $(IfW_DIR)/IfW_TSFFWind_Types.f90
IfW_TSFFWind$(OBJ_EXT):             NWTC_Library$(OBJ_EXT) IfW_TSFFWind_Types$(OBJ_EXT)
IfW_HAWCWind_Types$(OBJ_EXT):       NWTC_Library$(OBJ_EXT) $(IfW_DIR)/IfW_HAWCWind_Types.f90
IfW_HAWCWind$(OBJ_EXT):             NWTC_Library$(OBJ_EXT) IfW_HAWCWind_Types$(OBJ_EXT)
IfW_BladedFFWind_Types$(OBJ_EXT):   NWTC_Library$(OBJ_EXT) $(IfW_DIR)/IfW_BladedFFWind_Types.f90
IfW_BladedFFWind$(OBJ_EXT):         NWTC_Library$(OBJ_EXT) IfW_BladedFFWind_Types$(OBJ_EXT)
IfW_UserWind_Types$(OBJ_EXT):       NWTC_Library$(OBJ_EXT) $(IfW_DIR)/IfW_UserWind_Types.f90
IfW_UserWind$(OBJ_EXT):             NWTC_Library$(OBJ_EXT) IfW_UserWind_Types$(OBJ_EXT)
Lidar_Types$(OBJ_EXT):              NWTC_Library$(OBJ_EXT) $(IfW_DIR)/Lidar_Types.f90
Lidar$(OBJ_EXT):                    NWTC_Library$(OBJ_EXT) Lidar_Types$(OBJ_EXT) InflowWind_Subs$(OBJ_EXT)
InflowWind_Types$(OBJ_EXT):         NWTC_Library$(OBJ_EXT)  IfW_UniformWind_Types$(OBJ_EXT) Lidar_Types$(OBJ_EXT) \
                                      IfW_TSFFWind_Types$(OBJ_EXT) IfW_BladedFFWind_Types$(OBJ_EXT) IfW_UserWind_Types$(OBJ_EXT) \
                                      IfW_HAWCWind_Types$(OBJ_EXT) $(IfW_DIR)/InflowWind_Types.f90
InflowWind_Subs$(OBJ_EXT):          NWTC_Library$(OBJ_EXT)  InflowWind_Types$(OBJ_EXT) IfW_UniformWind$(OBJ_EXT) \
                                      IfW_TSFFWind$(OBJ_EXT) IfW_BladedFFWind$(OBJ_EXT) IfW_UserWind$(OBJ_EXT) IfW_HAWCWind$(OBJ_EXT)
InflowWind$(OBJ_EXT):               NWTC_Library$(OBJ_EXT)  InflowWind_Subs$(OBJ_EXT) Lidar$(OBJ_EXT)


#AeroDyn14 dependency rules:
AeroDyn14_Types$(OBJ_EXT):        NWTC_Library$(OBJ_EXT)  DWM_Types$(OBJ_EXT)  InflowWind_Types$(OBJ_EXT) $(AD14_DIR)/AeroDyn14_Types.f90
GenSubs$(OBJ_EXT):                NWTC_Library$(OBJ_EXT)  AeroDyn14_Types$(OBJ_EXT)
AeroSubs$(OBJ_EXT):               NWTC_Library$(OBJ_EXT)  AeroDyn14_Types$(OBJ_EXT) GenSubs$(OBJ_EXT)
AeroDyn14$(OBJ_EXT):              AeroDyn14_Types$(OBJ_EXT) AeroSubs$(OBJ_EXT) GenSubs$(OBJ_EXT) DWM$(OBJ_EXT) DWM_Types$(OBJ_EXT)

#DWM dependency rules:
DWM_Types$(OBJ_EXT):              NWTC_Library$(OBJ_EXT) InflowWind_Types$(OBJ_EXT) $(DWM_DIR)/DWM_Types.f90
DWM_Wake_Sub_ver2$(OBJ_EXT):      NWTC_Library$(OBJ_EXT) DWM_Types$(OBJ_EXT) InflowWind$(OBJ_EXT)
DWM$(OBJ_EXT):                    NWTC_Library$(OBJ_EXT) DWM_Types$(OBJ_EXT) DWM_Wake_Sub_ver2$(OBJ_EXT)

#AeroDyn/AFI dependency rules:
AirfoilInfo_Types$(OBJ_EXT):      NWTC_Library$(OBJ_EXT) $(AFI_DIR)/AirfoilInfo_Types.f90
AirfoilInfo$(OBJ_EXT):            AirfoilInfo_Types$(OBJ_EXT)  NWTC_FitPack$(OBJ_EXT)

#AeroDyn/UA dependency rules:
UnsteadyAero_Types$(OBJ_EXT):     NWTC_Library$(OBJ_EXT)  AirfoilInfo_Types$(OBJ_EXT)  $(UA_DIR)/UnsteadyAero_Types.f90
UnsteadyAero$(OBJ_EXT):           AirfoilInfo$(OBJ_EXT)  UnsteadyAero_Types$(OBJ_EXT)


#AeroDyn/BEMT dependency rules:
BEMT_Types$(OBJ_EXT):             NWTC_Library$(OBJ_EXT)  AirfoilInfo_Types$(OBJ_EXT)  UnsteadyAero_Types$(OBJ_EXT)  $(BEMT_DIR)/BEMT_Types.f90
BladeElement$(OBJ_EXT):           NWTC_Library$(OBJ_EXT)  UnsteadyAero$(OBJ_EXT)  UnsteadyAero_Types$(OBJ_EXT)  AirfoilInfo_Types$(OBJ_EXT)
BEMTCoupled$(OBJ_EXT):            NWTC_Library$(OBJ_EXT)  BladeElement$(OBJ_EXT)
nwtc_bemt_minpack$(OBJ_EXT):      NWTC_Library$(OBJ_EXT)  BEMTCoupled$(OBJ_EXT)    AirfoilInfo_Types$(OBJ_EXT)  UnsteadyAero_Types$(OBJ_EXT)
BEMTUncoupled$(OBJ_EXT):          NWTC_Library$(OBJ_EXT)  BladeElement$(OBJ_EXT)
Brent$(OBJ_EXT):                  NWTC_Library$(OBJ_EXT)  BEMTUncoupled$(OBJ_EXT)  BEMT_Types$(OBJ_EXT)
fmin_fcn$(OBJ_EXT):               NWTC_Library$(OBJ_EXT)  BEMTUncoupled$(OBJ_EXT)  AirfoilInfo_Types$(OBJ_EXT)  UnsteadyAero_Types$(OBJ_EXT)
mod_root1dim$(OBJ_EXT):           NWTC_Library$(OBJ_EXT)  fmin_fcn$(OBJ_EXT)
BEMT$(OBJ_EXT):                   NWTC_Library$(OBJ_EXT)  BEMT_Types$(OBJ_EXT)  BEMTUncoupled$(OBJ_EXT)  BEMTCoupled$(OBJ_EXT)  \
                                                          Brent$(OBJ_EXT)  UnsteadyAero$(OBJ_EXT)  BladeElement$(OBJ_EXT) \
                                                          mod_root1dim$(OBJ_EXT) fmin_fcn$(OBJ_EXT) nwtc_bemt_minpack$(OBJ_EXT)

#AeroDyn dependency rules:
AeroDyn_Types$(OBJ_EXT):          NWTC_Library$(OBJ_EXT)  AirfoilInfo_Types$(OBJ_EXT)  BEMT_Types$(OBJ_EXT)  UnsteadyAero_Types$(OBJ_EXT)  $(AD_DIR)/AeroDyn_Types.f90
AeroDyn_IO$(OBJ_EXT):             NWTC_Library$(OBJ_EXT)  AeroDyn_Types$(OBJ_EXT) BladeElement$(OBJ_EXT)
AeroDyn$(OBJ_EXT):                NWTC_Library$(OBJ_EXT)  AeroDyn_IO$(OBJ_EXT)  AirfoilInfo$(OBJ_EXT)  BEMT$(OBJ_EXT)


#BeamDyn dependency rules:
BeamDyn_Types$(OBJ_EXT):          NWTC_Library$(OBJ_EXT)  $(BD_DIR)/BeamDyn_Types.f90
BeamDyn_IO$(OBJ_EXT):             BeamDyn_Types$(OBJ_EXT)  BeamDyn_Subs$(OBJ_EXT)
BeamDyn_Subs$(OBJ_EXT):           BeamDyn_Types$(OBJ_EXT)
BeamDyn$(OBJ_EXT):                BeamDyn_IO$(OBJ_EXT)  BeamDyn_Subs$(OBJ_EXT)


#HydroDyn dependency rules:
SS_Radiation_Types$(OBJ_EXT):     NWTC_Library$(OBJ_EXT) $(HD_DIR)/SS_Radiation_Types.f90
SS_Radiation$(OBJ_EXT):           SS_Radiation_Types$(OBJ_EXT)

Waves2_Types$(OBJ_EXT):           NWTC_Library$(OBJ_EXT) $(HD_DIR)/Waves2_Types.f90
Waves2_Output$(OBJ_EXT):          Waves2_Types$(OBJ_EXT)
Waves2$(OBJ_EXT):                 NWTC_Library$(OBJ_EXT)  Waves2_Types$(OBJ_EXT)  NWTC_FFTPACK$(OBJ_EXT)  \
                                     Waves2_Output$(OBJ_EXT)  Waves$(OBJ_EXT)

Waves_Types$(OBJ_EXT):            NWTC_Library$(OBJ_EXT) $(HD_DIR)/Waves_Types.f90
UserWaves$(OBJ_EXT):              Waves_Types$(OBJ_EXT)   NWTC_FFTPACK$(OBJ_EXT)
Waves$(OBJ_EXT):                  Waves_Types$(OBJ_EXT)   NWTC_FFTPACK$(OBJ_EXT)  UserWaves$(OBJ_EXT)

Current_Types$(OBJ_EXT):          NWTC_Library$(OBJ_EXT) $(HD_DIR)/Current_Types.f90
Current$(OBJ_EXT):                Current_Types$(OBJ_EXT)

Morison_Types$(OBJ_EXT):          NWTC_Library$(OBJ_EXT) $(HD_DIR)/Morison_Types.f90
Morison_Output$(OBJ_EXT):         Morison_Types$(OBJ_EXT) Waves$(OBJ_EXT)
Morison$(OBJ_EXT):                Morison_Types$(OBJ_EXT) Morison_Output$(OBJ_EXT)

Conv_Radiation_Types$(OBJ_EXT):   NWTC_Library$(OBJ_EXT) $(HD_DIR)/Conv_Radiation_Types.f90
Conv_Radiation$(OBJ_EXT):         Conv_Radiation_Types$(OBJ_EXT)

WAMIT2_Types$(OBJ_EXT):           NWTC_Library$(OBJ_EXT) $(HD_DIR)/WAMIT2_Types.f90
WAMIT2_Output$(OBJ_EXT):          NWTC_Library$(OBJ_EXT)  WAMIT2_Types$(OBJ_EXT)
WAMIT2$(OBJ_EXT):                 NWTC_Library$(OBJ_EXT)  WAMIT_Interp$(OBJ_EXT)  WAMIT2_Output$(OBJ_EXT)  \
                                    NWTC_FFTPACK$(OBJ_EXT)  Waves$(OBJ_EXT)

WAMIT_Types$(OBJ_EXT):            NWTC_Library$(OBJ_EXT) Conv_Radiation_Types$(OBJ_EXT) SS_Radiation_Types$(OBJ_EXT) \
                                    Waves_Types$(OBJ_EXT) $(HD_DIR)/WAMIT_Types.f90
WAMIT$(OBJ_EXT):                  WAMIT_Types$(OBJ_EXT) WAMIT_Output$(OBJ_EXT) Waves_Types$(OBJ_EXT) Conv_Radiation$(OBJ_EXT) \
                                    SS_Radiation$(OBJ_EXT) NWTC_FFTPACK$(OBJ_EXT) WAMIT_Interp$(OBJ_EXT)

WAMIT_Interp$(OBJ_EXT):           NWTC_Library$(OBJ_EXT)
WAMIT_Output$(OBJ_EXT):           WAMIT_Types$(OBJ_EXT) Waves$(OBJ_EXT)
HydroDyn_Output$(OBJ_EXT):        HydroDyn_Types$(OBJ_EXT) Waves$(OBJ_EXT)

HydroDyn_Types$(OBJ_EXT):         NWTC_Library$(OBJ_EXT)  Current_Types$(OBJ_EXT)  Waves_Types$(OBJ_EXT)  Waves2_Types$(OBJ_EXT) \
                                     Conv_Radiation_Types$(OBJ_EXT) SS_Radiation_Types$(OBJ_EXT) \
                                     WAMIT_Types$(OBJ_EXT)  WAMIT2_Types$(OBJ_EXT)  Morison_Types$(OBJ_EXT) $(HD_DIR)/HydroDyn_Types.f90
HydroDyn_Input$(OBJ_EXT):         HydroDyn_Types$(OBJ_EXT)  Waves$(OBJ_EXT) Morison$(OBJ_EXT)  HydroDyn_Output$(OBJ_EXT)  \
                                     WAMIT_Output$(OBJ_EXT)  Waves2_Output$(OBJ_EXT)  WAMIT2_Output$(OBJ_EXT)
HydroDyn$(OBJ_EXT):               HydroDyn_Types$(OBJ_EXT)  HydroDyn_Input$(OBJ_EXT)  HydroDyn_Output$(OBJ_EXT)  WAMIT$(OBJ_EXT) \
                                     Current$(OBJ_EXT)  Morison$(OBJ_EXT)  Waves2$(OBJ_EXT)  WAMIT2$(OBJ_EXT)


# SubDyn dependency rules:
SubDyn_Types$(OBJ_EXT):           NWTC_Library$(OBJ_EXT) $(SD_DIR)/SubDyn_Types.f90
SD_FEM$(OBJ_EXT):                 NWTC_Library$(OBJ_EXT) SubDyn_Types$(OBJ_EXT) qsort_c_module$(OBJ_EXT)
SubDyn_Output$(OBJ_EXT):          SD_FEM$(OBJ_EXT)
SubDyn$(OBJ_EXT):                 SubDyn_Output$(OBJ_EXT)  SD_FEM$(OBJ_EXT)  qsort_c_module$(OBJ_EXT) NWTC_ScaLAPACK$(OBJ_EXT) NWTC_LAPACK$(OBJ_EXT)


# MAP dependency rules:
MAP_Types$(OBJ_EXT):              NWTC_Library$(OBJ_EXT)  $(MAP_DIR)/MAP_Types.f90
MAP$(OBJ_EXT):                    NWTC_Library$(OBJ_EXT)  MAP_Types$(OBJ_EXT)


# FEAMooring dependency rules:
FEAMooring_Types$(OBJ_EXT):       NWTC_Library$(OBJ_EXT)  $(FEAM_DIR)/FEAMooring_Types.f90
FEAM$(OBJ_EXT):                   NWTC_Library$(OBJ_EXT)  FEAMooring_Types$(OBJ_EXT)

# MoorDyn dependency rules:
MoorDyn_Types$(OBJ_EXT):          NWTC_Library$(OBJ_EXT)  $(MD_DIR)/MoorDyn_Types.f90
MoorDyn_IO$(OBJ_EXT):             NWTC_Library$(OBJ_EXT)  MoorDyn_Types$(OBJ_EXT)
MoorDyn$(OBJ_EXT):                NWTC_Library$(OBJ_EXT)  MoorDyn_IO$(OBJ_EXT)


# OrcaFlex dependency rules:
OrcaFlexInterface_Types$(OBJ_EXT): NWTC_Library$(OBJ_EXT)  $(Orca_DIR)/OrcaFlexInterface_Types.f90
OrcaFlexInterface$(OBJ_EXT):       NWTC_Library$(OBJ_EXT)  OrcaFlexInterface_Types$(OBJ_EXT)


# ElastoDyn dependency rules:
ElastoDyn_Types$(OBJ_EXT):        NWTC_Library$(OBJ_EXT)  $(ED_DIR)/ElastoDyn_Types.f90
ElastoDyn$(OBJ_EXT):              NWTC_Library$(OBJ_EXT)  ElastoDyn_Types$(OBJ_EXT)  NWTC_LAPACK$(OBJ_EXT)


# ServoDyn dependency rules:
TMD_Types$(OBJ_EXT):              NWTC_Library$(OBJ_EXT)  $(TMD_DIR)/TMD_Types.f90
TMD$(OBJ_EXT):                    NWTC_Library$(OBJ_EXT)  TMD_Types$(OBJ_EXT)
ServoDyn_Types$(OBJ_EXT):         NWTC_Library$(OBJ_EXT)  TMD_Types$(OBJ_EXT) $(SrvD_DIR)/ServoDyn_Types.f90
BladedInterface$(OBJ_EXT):        NWTC_Library$(OBJ_EXT)  ServoDyn_Types$(OBJ_EXT)
ServoDyn$(OBJ_EXT):               NWTC_Library$(OBJ_EXT)  ServoDyn_Types$(OBJ_EXT)  PitchCntrl_ACH$(OBJ_EXT)  UserSubs$(OBJ_EXT) \
                                     UserVSCont_KP$(OBJ_EXT)  BladedInterface$(OBJ_EXT) TMD$(OBJ_EXT)
PitchCntrl_ACH$(OBJ_EXT):         NWTC_Library$(OBJ_EXT)
UserSubs$(OBJ_EXT):               NWTC_Library$(OBJ_EXT)
UserVSCont_KP$(OBJ_EXT):          NWTC_Library$(OBJ_EXT)


# IceFloe dependency rules:
#RANLUX$(OBJ_EXT):
iceLog$(OBJ_EXT):                 NWTC_Library$(OBJ_EXT)
iceInput$(OBJ_EXT):               iceLog$(OBJ_EXT) NWTC_Library$(OBJ_EXT)
IceFloeBase$(OBJ_EXT):            NWTC_Library$(OBJ_EXT) iceInput$(OBJ_EXT)  RANLUX$(OBJ_EXT) IceFloe_Types$(OBJ_EXT)
coupledCrushing$(OBJ_EXT):        IceFloeBase$(OBJ_EXT)
crushingIEC$(OBJ_EXT):            IceFloeBase$(OBJ_EXT)
crushingISO$(OBJ_EXT):            IceFloeBase$(OBJ_EXT)
intermittentCrushing$(OBJ_EXT):   crushingISO$(OBJ_EXT)
lockInISO$(OBJ_EXT):              crushingISO$(OBJ_EXT)
randomCrushing$(OBJ_EXT):         crushingISO$(OBJ_EXT)
IceFlexBase$(OBJ_EXT):            IceFloeBase$(OBJ_EXT)
IceFlexIEC$(OBJ_EXT):             IceFlexBase$(OBJ_EXT)
IceFlexISO$(OBJ_EXT):             IceFlexBase$(OBJ_EXT)
IceFloe_Types$(OBJ_EXT):          NWTC_Library$(OBJ_EXT) $(IceF_DIR)/IceFloe_Types.f90
IceFloe$(OBJ_EXT):                IceFloe_Types$(OBJ_EXT) iceLog$(OBJ_EXT) coupledCrushing$(OBJ_EXT) crushingIEC$(OBJ_EXT) \
                                    crushingISO$(OBJ_EXT) IceFlexBase$(OBJ_EXT) IceFlexIEC$(OBJ_EXT) IceFlexISO$(OBJ_EXT)  \
                                    intermittentCrushing$(OBJ_EXT) lockInISO$(OBJ_EXT) randomCrushing$(OBJ_EXT)


# IceDyn dependency rules:
IceDyn_Types$(OBJ_EXT):           NWTC_Library$(OBJ_EXT) $(IceD_DIR)/IceDyn_Types.f90
IceDyn$(OBJ_EXT):                 NWTC_Library$(OBJ_EXT) IceDyn_Types$(OBJ_EXT)


#OpenFOAM integration dependency rules:
OpenFOAM_Types$(OBJ_EXT):         NWTC_Library$(OBJ_EXT) $(OpFM_DIR)/OpenFOAM_Types.f90
OpenFOAM$(OBJ_EXT):               NWTC_Library$(OBJ_EXT)  FAST_Types$(OBJ_EXT)


# FAST dependency rules:
FAST_Types$(OBJ_EXT):             NWTC_Library$(OBJ_EXT)  ElastoDyn_Types$(OBJ_EXT)  ServoDyn_Types$(OBJ_EXT)   \
                                     AeroDyn_Types$(OBJ_EXT)  AeroDyn14_Types$(OBJ_EXT)  InflowWind_Types$(OBJ_EXT) \
                                     SubDyn_Types$(OBJ_EXT)  HydroDyn_Types$(OBJ_EXT)  BeamDyn_Types$(OBJ_EXT) \
                                     MAP_Types$(OBJ_EXT)  FEAMooring_Types$(OBJ_EXT)  MoorDyn_Types$(OBJ_EXT)   \
                                     IceFloe_Types$(OBJ_EXT)  IceDyn_Types$(OBJ_EXT) $(FAST_DIR)/FAST_Types.f90  \
                                     OpenFOAM_Types$(OBJ_EXT) OrcaFlexInterface_Types$(OBJ_EXT)
FAST_Mods$(OBJ_EXT):              NWTC_Library$(OBJ_EXT)  FAST_Types$(OBJ_EXT)
FAST_Subs$(OBJ_EXT):              NWTC_Library$(OBJ_EXT)  FAST_Mods$(OBJ_EXT)  NWTC_LAPACK$(OBJ_EXT)  AeroDyn14$(OBJ_EXT) \
                                     InflowWind$(OBJ_EXT)  ServoDyn$(OBJ_EXT)  ElastoDyn$(OBJ_EXT) SubDyn$(OBJ_EXT)     \
                                     HydroDyn$(OBJ_EXT)  MAP$(OBJ_EXT)  FEAM$(OBJ_EXT)  MoorDyn$(OBJ_EXT)  OpenFOAM$(OBJ_EXT)  \
                                     IceFloe$(OBJ_EXT) IceDyn$(OBJ_EXT)  AeroDyn$(OBJ_EXT)  BeamDyn$(OBJ_EXT) OrcaFlexInterface$(OBJ_EXT)
$(FAST_driver)$(OBJ_EXT):         NWTC_Library$(OBJ_EXT)  FAST_Subs$(OBJ_EXT)



   # Make sure the destination directory for the intermediate files exist.

$(INTER_DIR):
	$(MD_CMD) $(INTER_DIR)


   # For linking FAST.

$(DEST_DIR)/$(OUTPUT_NAME)$(EXE_EXT): $(ALL_OBJS) | $(INTER_DIR)
	$(FC) $(LDFLAGS) -I $(INTER_DIR) -o $(DEST_DIR)/$(OUTPUT_NAME)$(EXE_EXT) \
	$(foreach src, $(ALL_OBJS), $(addprefix $(INTER_DIR)/,$(src))) $(MAP_lib) $(LAPACK_LINK)

   # Cleanup afterwards.

clean:
	$(DEL_CMD) $(INTER_DIR)$(PATH_SEP)*.mod $(INTER_DIR)$(PATH_SEP)*$(OBJ_EXT)

superclean:
	$(DEL_CMD) $(INTER_DIR)$(PATH_SEP)*.mod $(INTER_DIR)$(PATH_SEP)*$(OBJ_EXT)
	$(DEL_CMD) "$(DEST_DIR)$(PATH_SEP)$(OUTPUT_NAME)$(EXE_EXT)"
	$(DEL_CMD) "$(FAST_DIR)$(PATH_SEP)FAST_Types.f90"
	$(DEL_CMD) "$(TMD_DIR)$(PATH_SEP)TMD_Types.f90"
	$(DEL_CMD) "$(SrvD_DIR)$(PATH_SEP)ServoDyn_Types.f90"
	$(DEL_CMD) "$(ED_DIR)$(PATH_SEP)ElastoDyn_Types.f90"
	$(DEL_CMD) "$(AD14_DIR)$(PATH_SEP)AeroDyn14_Types.f90"
	$(DEL_CMD) "$(DWM_DIR)$(PATH_SEP)DWM_Types.f90"
	$(DEL_CMD) "$(SD_DIR)$(PATH_SEP)SubDyn_Types.f90"
	$(DEL_CMD) "$(FEAM_DIR)$(PATH_SEP)FEAMooring_Types.f90"
	$(DEL_CMD) "$(MD_DIR)$(PATH_SEP)MoorDyn_Types.f90"
	$(DEL_CMD) "$(MAP_DIR)$(PATH_SEP)MAP_Types.f90"
	$(DEL_CMD) "$(IceF_DIR)$(PATH_SEP)IceFloe_Types.f90"
	$(DEL_CMD) "$(IceD_DIR)$(PATH_SEP)IceDyn_Types.f90"
	$(DEL_CMD) "$(OpFM_DIR)$(PATH_SEP)OpenFOAM_Types.f90"
	$(DEL_CMD) "$(IfW_DIR)$(PATH_SEP)InflowWind_Types.f90"
	$(DEL_CMD) "$(IfW_DIR)$(PATH_SEP)Lidar_Types.f90"
	$(DEL_CMD) "$(IfW_DIR)$(PATH_SEP)"IfW_*_Types.f90
	$(DEL_CMD) "$(HD_DIR)$(PATH_SEP)"*_Types.f90
	$(DEL_CMD) "$(AFI_DIR)$(PATH_SEP)AirfoilInfo_Types.f90"
	$(DEL_CMD) "$(UA_DIR)$(PATH_SEP)UnsteadyAero_Types.f90"
	$(DEL_CMD) "$(BEMT_DIR)$(PATH_SEP)BEMT_Types.f90"
	$(DEL_CMD) "$(AD_DIR)$(PATH_SEP)AeroDyn_Types.f90"
	$(DEL_CMD) "$(BD_DIR)$(PATH_SEP)BeamDyn_Types.f90"
	$(DEL_CMD) "$(Orca_DIR)$(PATH_SEP)OrcaFlexInterface_Types.f90"
	$(DEL_CMD) *_Types.f90
