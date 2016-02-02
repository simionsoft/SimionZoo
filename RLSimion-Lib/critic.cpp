#include "stdafx.h"
#include "critic.h"
#include "parameters.h"
#include "parameter.h"
#include "vfa.h"
#include "vfa-critic.h"


CCritic *CCritic::getInstance(CParameters* pParameters)
{
	if (!pParameters) return 0;

	if (!strcmp(pParameters->getChild(0)->getName(), "VFA_CRITIC"))
		return CVFACritic::getInstance(pParameters->getChild(0));

	return 0;
}
