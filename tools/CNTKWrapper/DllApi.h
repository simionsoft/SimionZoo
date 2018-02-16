#pragma once

#ifdef __BUILD_CNTK_WRAPPER__
#define DLL_API __declspec(dllexport)
#else
#define DLL_API 
//__declspec(dllimport)
#endif