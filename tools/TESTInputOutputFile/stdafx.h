// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once
#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#include <crtdbg.h>
#define _CRTDBG_MAP_ALLOC
#endif  // _DEBUG

#include <stdlib.h>
#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

#ifdef _DEBUG
#pragma comment (lib,"../../Debug/RLSimion-Lib.lib")
#pragma comment (lib,"../../Debug/tinyxml2.lib")
#else
#pragma comment (lib,"../../Release/RLSimion-Lib.lib")
#pragma comment (lib,"../../Release/tinyxml2.lib")
#endif



// TODO: reference additional headers your program requires here
