#pragma once

//detection of memory leaks
#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#include <crtdbg.h>
#define _CRTDBG_MAP_ALLOC
#endif  // _DEBUG


#include <stdlib.h>
#include <stdio.h>
#include <exception>

#ifdef _DEBUG
#pragma comment (lib,"../Debug/RLSimion-Lib.lib")
#pragma comment (lib,"../Debug/tinyxml2.lib")
#pragma comment(lib,"../Debug/WindowsUtils.lib")
#else
#pragma comment (lib,"../bin/RLSimion-Lib.lib")
#pragma comment (lib,"../bin/tinyxml2.lib")
#pragma comment(lib,"../bin/WindowsUtils.lib")
#endif
