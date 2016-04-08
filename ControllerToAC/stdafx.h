// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdlib.h>
#include <stdio.h>
#include <tchar.h>
#include <algorithm>

#ifdef _DEBUG
	#pragma comment(lib,"../Debug/RLSimion-Lib.lib")
	#pragma comment (lib,"../Debug/tinyxml2.lib")
#else
	#pragma comment(lib,"../Release/RLSimion-Lib.lib")
	#pragma comment (lib,"../Release/tinyxml2.lib")
#endif
