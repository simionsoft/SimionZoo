#include <stdafx.h>
#include "actor.h"
#include "parameters.h"
#include "controller.h"

CActor *CActor::getInstance(char* configFile)
{
	CActor* pActor= 0;

	CParameters *pParameters= new CParameters(configFile);

	/*if (strcmp(pParameters->getStringPtr("ALGORITHM"),"REGULAR_GRADIENT")==0)
		pActor= new CRegularGradientActor(pParameters);
	else*/ if (strcmp(pParameters->getStringPtr("ALGORITHM"),"CACLA")==0)
		pActor= new CCACLAActor(pParameters);
	else if (strcmp(pParameters->getStringPtr("ALGORITHM"),"VIDAL")==0)
		pActor= new CWindTurbineVidalController(pParameters);
	else if (strcmp(pParameters->getStringPtr("ALGORITHM"),"BOUKHEZZAR")==0)
		pActor= new CWindTurbineBoukhezzarController(pParameters);
	else if (strcmp(pParameters->getStringPtr("ALGORITHM"),"PID")==0)
		pActor= new CPIDController(pParameters);
	else if (strcmp(pParameters->getStringPtr("ALGORITHM"),"LQR")==0)
		pActor= new CLQRController(pParameters);	
	delete pParameters;

	return pActor;
}
