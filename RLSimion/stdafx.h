




#include "../tinyxml2/tinyxml2.h"
#include <stdlib.h>
#include <stdio.h>


#ifdef _DEBUG
#pragma comment (lib,"../Debug/RLSimion-Lib.lib")
#pragma comment (lib,"../tinyxml2/tinyxml2/bin/Win32-Debug-Lib/tinyxml2.lib")
#else
#pragma comment (lib,"../Release/RLSimion-Lib.lib")
#pragma comment (lib,"../tinyxml2/tinyxml2/bin/Win32-Release-Lib/tinyxml2.lib")
#endif