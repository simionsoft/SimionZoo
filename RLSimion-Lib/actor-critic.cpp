#include "stdafx.h"
#include "actor-critic.h"
#include "config.h"
#include "actor.h"
#include "critic.h"
#include "app.h"
#include "app-rlsimion.h"
#include "simgod.h"

#define MIN_PROBABILITY 0.00001

CActorCritic::CActorCritic(CConfigNode* pConfigNode)
{
	m_td = 0.0;
	CSimionApp::get()->pLogger->addVarToStats("TD-error", "TD-error", &m_td);

	m_pActor = CHILD_OBJECT<CActor>(pConfigNode, "Actor", "The actor");
	m_pCritic = CHILD_OBJECT_FACTORY<CCritic>(pConfigNode, "Critic", "The critic");
}

double CActorCritic::selectAction(const CState *s, CAction *a)
{
	return m_pActor->selectAction(s, a);
}

void CActorCritic::update(const CState *s, const CAction *a, const CState *s_p, double r, double behaviorProb)
{
	//sample importance weigthing: rho= pi(a_t|x_t) / mu(a_t|x_t)
	//where mu is the behavior from which the sample is drawn

	double sampleWeight = 1.0;
	double policyProb;
	if (CSimionApp::get()->pSimGod->useSampleImportanceWeights())
	{
		policyProb = m_pActor->getActionProbability(s,a,false);
		sampleWeight = std::min(10.0,std::max(MIN_PROBABILITY,policyProb / behaviorProb));
	}

	m_td= m_pCritic->update(s, a, s_p, r,sampleWeight);

	m_pActor->update(s, a, s_p, r, m_td);
}
