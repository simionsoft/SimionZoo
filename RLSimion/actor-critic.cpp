#include "actor-critic.h"
#include "config.h"
#include "actor.h"
#include "critic.h"
#include "app.h"
#include "app-rlsimion.h"
#include "simgod.h"
#include "logger.h"

#define MIN_PROBABILITY 0.00001

ActorCritic::ActorCritic(ConfigNode* pConfigNode)
{
	m_td = 0.0;
	SimionApp::get()->pLogger->addVarToStats<double>("TD-error", "TD-error", m_td);

	m_pActor = CHILD_OBJECT<Actor>(pConfigNode, "Actor", "The actor");
	m_pCritic = CHILD_OBJECT_FACTORY<ICritic>(pConfigNode, "Critic", "The critic");
}

double ActorCritic::selectAction(const State *s, Action *a)
{
	return m_pActor->selectAction(s, a);
}

double ActorCritic::update(const State *s, const Action *a, const State *s_p, double r, double behaviorProb)
{
	//sample importance weigthing: rho= pi(a_t|x_t) / mu(a_t|x_t)
	//where mu is the behavior from which the sample is drawn

	double sampleWeight = 1.0;
	double policyProb;
	if (SimionApp::get()->pSimGod->useSampleImportanceWeights())
	{
		policyProb = m_pActor->getActionProbability(s,a,false);
		sampleWeight = policyProb;// std::min(10.0, std::max(MIN_PROBABILITY, policyProb / behaviorProb));
	}

	m_td= m_pCritic->update(s, a, s_p, r,sampleWeight);

	m_pActor->update(s, a, s_p, r, m_td);

	return m_td;
}
