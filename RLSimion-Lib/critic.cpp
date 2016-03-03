#include "stdafx.h"
#include "critic.h"
#include "vfa-critic.h"
#include "parameters.h"

CCritic *CCritic::getInstance(CParameters* pParameters)
{
	if (!pParameters) return 0;

	if (!strcmp(pParameters->getChild()->getName(), "VFA-Critic"))
		return CVFACritic::getInstance(pParameters->getChild());

	return 0;
}
