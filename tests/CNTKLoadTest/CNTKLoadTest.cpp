// CNTKLoadTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>


#include "../../tools/CNTKWrapper/CNTKWrapper.h"
#include "../../tools/CNTKWrapper/Problem.h"
#include "../../3rd-party/tinyxml2/tinyxml2.h"

typedef IProblem* (__stdcall * getProblemInstanceDLL)(tinyxml2::XMLElement* pNode);

getProblemInstanceDLL getProblemInstance;


int main()
{
	HMODULE hLibrary;
#ifdef _DEBUG
	hLibrary= LoadLibrary(L".\\..\\..\\Debug\\x64\\CNTKWrapper.dll");
#else
	hLibrary = LoadLibrary(L".\\..\\..\\bin\\x64\\CNTKWrapper.dll");
#endif


	DWORD error= 0;
	if (hLibrary)
	{
		getProblemInstance = (getProblemInstanceDLL)GetProcAddress(hLibrary, "CNTKWrapper::getProblemInstance");

		tinyxml2::XMLDocument doc;
		doc.LoadFile("problem-config-sample.xml");

		if (doc.Error() == tinyxml2::XML_NO_ERROR)
		{
			tinyxml2::XMLElement *pRoot= doc.FirstChildElement(); //root is of type "Problem" in the test file
			tinyxml2::XMLElement *pConfigNode = pRoot->FirstChildElement();
			IProblem* pProblem = getProblemInstance(pRoot);
			pProblem->createNetwork();


			if (pProblem)
			{
				printf("successssssss");
			}
			else
				error = GetLastError();
		}
		FreeLibrary(hLibrary);
	}
	else
		error= GetLastError();
    return 0;
}

