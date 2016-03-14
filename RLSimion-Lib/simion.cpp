#include "stdafx.h"
#include "simion.h"
#include "globals.h"
#include "parameters.h"
#include "controller.h"
#include "actor-critic.h"

CSimion* CLASS_FACTORY(CSimion)(CParameters* pParameters)
{
	CParameters* pChild = pParameters->getChild();
	const char* type = pChild->getName();

	CHOICE("Type");
	CHOICE_ELEMENT_FACTORY(type, "Controller", CController, pChild);
	CHOICE_ELEMENT(type, "Actor-Critic", CActorCritic, pChild);
	return 0;
	END_CLASS();
}