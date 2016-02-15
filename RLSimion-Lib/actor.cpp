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

	if (strcmp(type, "VFA-Actor") == 0)
		return new CVFAActor(child);
	else if (!strcmp(type, "Multi-Controller"))
		return new CMultiController(child);

	return 0;
}


