#include "stdafx.h"
#include "actor.h"
#include "noise.h"
#include "controller.h"
#include "vfa.h"
#include "vfa-actor.h"
#include "features.h"
#include "world.h"
#include "globals.h"
#include "named-var-set.h"
#include "parameters.h"

CActor* CActor::getInstance(CParameters* pParameters)
{
	CParameters* child;
	if (!pParameters) return 0;

	child = pParameters->getChild();
	const char* type = child->getName();

	if (strcmp(type, "VFA-Learner") == 0)
		return new CVFAActor(child);
	else if (!strcmp(type, "Multi-Controller"))
		return new CMultiController(child);

	return 0;
}


