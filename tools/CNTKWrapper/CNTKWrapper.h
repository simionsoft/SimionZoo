#pragma once
#ifdef _WIN64

#ifdef BUILD_CNTK_WRAPPER
//we are building the CNTK Wrapper
#define DLL_API __declspec(dllexport)
#else
#define DLL_API
//#define CNTK_HEADERONLY_DEFINITIONS
#endif


#include "CNTKLibrary.h"



class IProblem;

namespace tinyxml2 { class XMLElement; }


namespace CNTKWrapper
{
	DLL_API IProblem* getProblemInstance(tinyxml2::XMLElement* pNode);
}

#endif