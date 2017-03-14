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

