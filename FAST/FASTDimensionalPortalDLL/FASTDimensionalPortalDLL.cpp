// This is the main DLL file.

#include "stdafx.h"

#include "FASTDimensionalPortalDLL.h"
#include "FASTWorldPortal.h"
#include "../../tinyxml2/tinyxml2.h"
#if _DEBUG
	#pragma comment(lib,"../../Debug/tinyxml2.lib")
#else
	#pragma comment(lib,"../../Release/tinyxml2.lib")
#endif

#define NINT(a) ((a) >= 0.0 ? (int)((a)+0.5) : (int)((a)-0.5))

extern "C" {

	FASTWorldPortal g_FASTWorldPortal;

	void __declspec(dllexport) __cdecl DISCON(float *avrSWAP //inout
		, int *aviFAIL //inout
		, char *accINFILE //in
		, char* avcOUTNAME //in
		, char *avcMSG) //inout
	{
		int iStatus = NINT(avrSWAP[1 - 1]);
		
		*aviFAIL = 0;

		if (iStatus==0)
		{
			//Only in first call
			*aviFAIL = 1;
			strcpy_s(avcMSG, 512, "Dimensional portal between FAST and RLSimion opened\n");
			tinyxml2::XMLDocument configFile;
			const char* pipeName;

			printf("Loading Dimensional portal config file: %s\n", accINFILE);
			if (configFile.LoadFile(accINFILE) == tinyxml2::XML_NO_ERROR)
			{
				tinyxml2::XMLElement *pNode;
				pNode = configFile.FirstChildElement("FAST-DIMENSIONAL-PORTAL");
				pipeName= pNode->FirstChildElement("PIPE-NAME")->GetText();
				
				g_FASTWorldPortal.connectToNamedPipeServer(pipeName);
				printf("Connected to master process via pipe %s\n", pipeName);
			}
			else
			{
				printf("FASTDimensionalPortalDLL: Could not open configuration file\n");
				*aviFAIL = -1;
			}

		}
		if (iStatus>=0)
		{
			//Done in all calls but last
			g_FASTWorldPortal.retrieveStateVariables(avrSWAP,(bool)(iStatus==0));
			g_FASTWorldPortal.sendState();
			g_FASTWorldPortal.receiveAction();
			g_FASTWorldPortal.setActionVariables(avrSWAP);
			
		}
		if (iStatus < 0)
		{
			//Last call
			g_FASTWorldPortal.disconnectFromNamedPipeServer();
		}
	}
}