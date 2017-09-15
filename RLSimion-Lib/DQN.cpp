#ifdef _WIN64

#include "DQN.h"
#include "SimGod.h"
#include "worlds\world.h"
#include "app.h"
#include "featuremap.h"
#include "features.h"
#include "noise.h"
#include "parameters-numeric.h"

CDQN::~CDQN()
{
}

CDQN::CDQN(CConfigNode* pConfigNode)
{
	m_policies = MULTI_VALUE_FACTORY<CDeepPolicy>(pConfigNode, "Policy", "The policy");
}

double CDQN::selectAction(const CState * s, CAction * a)
{
	double prob = 1.0;
	for (unsigned int i = 0; i < m_policies.size(); i++)
	{
		prob *= m_policies[i]->selectAction(s, a);
	}
	
	return prob;
}

double CDQN::update(const CState * s, const CAction * a, const CState * s_p, double r, double behaviorProb)
{
	double prob = 1.0;
	for (unsigned int i = 0; i < m_policies.size(); i++)
	{
		prob *= m_policies[i]->updateNetwork(s, a, s_p, r);
	}

	return prob;
}

#endif