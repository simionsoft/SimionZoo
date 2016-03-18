#include "stdafx.h"
#include "simion.h"
#include "globals.h"
#include "parameters.h"
#include "controller.h"
#include "actor-critic.h"

CLASS_FACTORY(CSimion)
{
	CHOICE("Simion","The Simion's class");
	CHOICE_ELEMENT_FACTORY("Controller", CController,"A controller");
	CHOICE_ELEMENT("Actor-Critic", CActorCritic,"An Actor-Critic");
	END_CHOICE();
	return 0;
	END_CLASS();
}