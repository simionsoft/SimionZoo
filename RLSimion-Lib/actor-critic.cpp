#include "stdafx.h"
#include "actor-critic.h"
#include "vfa-actor-critic.h"
#include "parameters.h"

CActorCritic* CActorCritic::getInstance(CParameters* pParameters)
{
	if (!pParameters) return 0;

	if (!strcmp(pParameters->getName(), "VFA-Actor-Critic"))
		return CVFAActorCritic::getInstance(pParameters->getChild(0));

	return 0;
}