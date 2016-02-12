#include "stdafx.h"
#include "actor.h"
#include "noise.h"
#include "controller.h"
#include "vfa.h"
#include "vfa-actor.h"
#include "features.h"
#include "world.h"
#include "globals.h"
#include "states-and-actions.h"


CActor* CActor::getInstance(tinyxml2::XMLElement* pParameters)
{
	tinyxml2::XMLElement* child;
	if (!pParameters) return 0;

	child = pParameters->FirstChildElement();
	const char* type = child->Name();

	if (strcmp(type, "VFA-ACTOR") == 0)
		return new CVFAActor(child); 
	else if (strcmp(type, "VIDAL") == 0)
		return new CWindTurbineVidalController(child);
	else if (strcmp(type, "BOUKHEZZAR") == 0)
		return new CWindTurbineBoukhezzarController(child);
	else if (strcmp(type, "PID") == 0)
		return new CPIDController(child);
	else if (strcmp(type, "LQR") == 0)
		return new CLQRController(child);
	return 0;
}


