// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#include <stdlib.h>
#include <stdio.h>
#include <tchar.h>


#ifdef _DEBUG
	#pragma comment(lib,"../Debug/RLSimion-Lib.lib")
#else
	#pragma comment(lib,"../Release/RLSimion-Lib.lib")
#endif
