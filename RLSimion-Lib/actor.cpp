#include "stdafx.h"
#include "actor.h"
#include "parameters.h"
#include "parameter.h"
#include "noise.h"
#include "controller.h"
#include "vfa.h"
#include "vfa-actor.h"
#include "features.h"
#include "world.h"
#include "globals.h"
#include "states-and-actions.h"


CActor* CActor::getInstance(CParameters* pParameters)
{
	if (!pParameters) return 0;

	const char* type = pParameters->getChild(0)->getName();

	if (strcmp(type, "VFA-Actor") == 0)
		return new CVFAActor(pParameters);
	else if (strcmp(type, "VIDAL") == 0)
		return new CWindTurbineVidalController(pParameters);
	else if (strcmp(type, "BOUKHEZZAR") == 0)
		return new CWindTurbineBoukhezzarController(pParameters);
	else if (strcmp(type, "PID") == 0)
		return new CPIDController(pParameters);
	else if (strcmp(type, "LQR") == 0)
		return new CLQRController(pParameters);
	return 0;
}


