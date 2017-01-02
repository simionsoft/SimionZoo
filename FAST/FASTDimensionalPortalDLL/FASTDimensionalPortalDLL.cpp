// This is the main DLL file.

#include "stdafx.h"

#include "FASTDimensionalPortalDLL.h"
#include "FASTWorldPortal.h"

#define NINT(a) ((a) >= 0.0 ? (int)((a)+0.5) : (int)((a)-0.5))

extern "C" {

	FASTWorldPortal g_FASTWorld;


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
			//Ony in first call
			*aviFAIL = 1;
			strcpy_s(avcMSG, 512, "Dimensional portal between FAST and RLSimion opened\n");

			g_FASTWorld.connectToNamedPipeServer();
		}
		if (iStatus>=0)
		{
			//Done in all calls but last
			bool bControlStep= g_FASTWorld.retrieveStateVariables(avrSWAP,(bool)(iStatus==0));
			if (bControlStep)
			{
				g_FASTWorld.sendState();
				g_FASTWorld.receiveAction();
				g_FASTWorld.setActionVariables(avrSWAP);
			}
		}
		if (iStatus < 0)
		{
			//Last call
			g_FASTWorld.disconnectFromNamedPipeServer();
		}
	}
}