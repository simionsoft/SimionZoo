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

CActor* CLASS_FACTORY(CActor)::getInstance(CParameters* pParameters)
{
	CParameters* child;
	if (!pParameters) return 0;

	child = pParameters->getChild();
	const char* type = child->getName();

	CHOICE("Actor-Type");
	CHOICE_ELEMENT(type, "VFA-Learner", CVFAActor, child);
	CHOICE_ELEMENT(type, "Multi-Controller", CMultiController, child);
	END_CHOICE();

	END_CLASS();
	return 0;
}


