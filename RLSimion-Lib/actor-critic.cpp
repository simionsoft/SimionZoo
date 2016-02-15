#include "stdafx.h"
#include "actor-critic.h"
#include "vfa-actor-critic.h"
#include "parameters.h"

CActorCritic* CActorCritic::getInstance(tinyxml2::XMLElement* pParameters)
{
	if (!pParameters) return 0;

	if (!strcmp(pParameters->FirstChildElement()->Name(), "VFA-Actor-Critic"))
		return CVFAActorCritic::getInstance(pParameters->FirstChildElement());

	return 0;
}