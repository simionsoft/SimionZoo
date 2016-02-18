#include "stdafx.h"
#include "critic.h"
#include "vfa-critic.h"


CCritic *CCritic::getInstance(tinyxml2::XMLElement* pParameters)
{
	if (!pParameters) return 0;

	if (!strcmp(pParameters->FirstChildElement()->Name(), "VFA-Critic"))
		return CVFACritic::getInstance(pParameters->FirstChildElement());

	return 0;
}
