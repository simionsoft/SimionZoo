#include "stdafx.h"
#include "simion.h"
#include "globals.h"
#include "parameters.h"
#include "controller.h"
#include "actor-critic.h"

CLASS_FACTORY(CSimion)
{
	CHOICE("Simion");
	CHOICE_ELEMENT_FACTORY("Controller", CController);
	CHOICE_ELEMENT("Actor-Critic", CActorCritic);
	END_CHOICE();
	return 0;
	END_CLASS();
}