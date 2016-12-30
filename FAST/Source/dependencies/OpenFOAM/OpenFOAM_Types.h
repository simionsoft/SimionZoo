//!STARTOFREGISTRYGENERATEDFILE 'OpenFOAM_Types.h'
//!
//! WARNING This file is generated automatically by the FAST registry
//! Do not edit.  Your changes to this file will be lost.
//!

#ifndef _OpenFOAM_TYPES_H
#define _OpenFOAM_TYPES_H


#ifdef _WIN32 //define something for Windows (32-bit)
#  include "stdbool.h"
#  define CALL __declspec( dllexport )
#elif _WIN64 //define something for Windows (64-bit)
#  include "stdbool.h"
#  define CALL __declspec( dllexport ) 
#else
#  include <stdbool.h>
#  define CALL 
#endif


  typedef struct OpFM_InitInputType {
    void * object ;
    int NumSCin ;
    int NumSCout ;
  } OpFM_InitInputType_t ;
  typedef struct OpFM_InitOutputType {
    void * object ;
    char * WriteOutputHdr ;     int WriteOutputHdr_Len ;
    char * WriteOutputUnt ;     int WriteOutputUnt_Len ;

  } OpFM_InitOutputType_t ;
  typedef struct OpFM_MiscVarType {
    void * object ;




  } OpFM_MiscVarType_t ;
  typedef struct OpFM_ParameterType {
    void * object ;
    float AirDens ;
    int Nnodes ;
  } OpFM_ParameterType_t ;
  typedef struct OpFM_InputType {
    void * object ;
    float * px ;     int px_Len ;
    float * py ;     int py_Len ;
    float * pz ;     int pz_Len ;
    float * fx ;     int fx_Len ;
    float * fy ;     int fy_Len ;
    float * fz ;     int fz_Len ;
    float * SuperController ;     int SuperController_Len ;
  } OpFM_InputType_t ;
  typedef struct OpFM_OutputType {
    void * object ;
    float * u ;     int u_Len ;
    float * v ;     int v_Len ;
    float * w ;     int w_Len ;
    float * SuperController ;     int SuperController_Len ;
    float * WriteOutput ;     int WriteOutput_Len ;
  } OpFM_OutputType_t ;
  typedef struct OpFM_UserData {
    OpFM_InitInputType_t           OpFM_InitInput ;
    OpFM_InitOutputType_t          OpFM_InitOutput ;
    OpFM_ParameterType_t           OpFM_Param ;
    OpFM_InputType_t               OpFM_Input ;
    OpFM_OutputType_t              OpFM_Output ;
  } OpFM_t ;

#endif // _OpenFOAM_TYPES_H


//!ENDOFREGISTRYGENERATEDFILE
