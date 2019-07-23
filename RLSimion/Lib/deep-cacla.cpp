#include "deep-cacla.h"

#if defined(__linux__) || defined(_WIN64)

#include "deep-network.h"
#include "deep-minibatch.h"
#include "deep-functions.h"
#include "simgod.h"
#include "app.h"
#include "noise.h"
#include <algorithm>
#include "app.h"
#include "simion.h"
#include "experiment.h"

DeepCACLA::DeepCACLA(ConfigNode* pConfigNode)
{
	m_actorPolicy = CHILD_OBJECT<DeepDeterministicPolicy>(pConfigNode, "Policy", "Neural Network used to represent the actors policy");
	m_noiseSignals = MULTI_VALUE_FACTORY<Noise>(pConfigNode, "Exploration-Noise", "Noise signals added to each of the outputs of the deterministic policy");
	m_criticVFunction= CHILD_OBJECT<DeepVFunction>(pConfigNode, "Value-Function", "Value function learned by the critic");
	CNTK::WrapperClient::SetRequirements();
}


DeepCACLA::~DeepCACLA()
{
	if (m_pActorMinibatch) delete m_pActorMinibatch;
	if (m_pActorNetwork) m_pActorNetwork->destroy();
	
	if (m_pCriticMinibatch) delete m_pCriticMinibatch;
	if (m_pCriticOnlineNetwork) m_pCriticOnlineNetwork->destroy();
	if (m_pCriticTargetNetwork) m_pCriticTargetNetwork->destroy();

	CNTK::WrapperClient::UnLoad();
}


void DeepCACLA::deferredLoadStep()
{
	CNTK::WrapperClient::Load();

	//Initialize the actor
	m_pActorMinibatch = m_actorPolicy->getMinibatch();
	m_pActorNetwork= m_actorPolicy->getNetworkInstance();
	SimionApp::get()->registerStateActionFunction("Policy", m_pActorNetwork);

	//Initialise the critic
	m_pCriticMinibatch = m_criticVFunction->getMinibatch();
	m_pCriticOnlineNetwork = m_criticVFunction->getNetworkInstance();
	m_pCriticTargetNetwork = (IVFunctionNetwork*) m_pCriticOnlineNetwork->clone();
	SimionApp::get()->registerStateActionFunction("V", m_pCriticOnlineNetwork);

	m_V_s_p = vector<double>(m_pCriticMinibatch->size());
}

double DeepCACLA::update(const State *s, const Action *a, const State *s_p, double r, double probability)
{
	double gamma = SimionApp::get()->pSimGod->getGamma();

	//Actor network
	if (!m_pActorMinibatch->isFull())
	{
		//add only tuples that produced a positive temporal difference
		double v_s= m_pCriticTargetNetwork->evaluate(s, a)[0];
		double v_s_p= m_pCriticTargetNetwork->evaluate(s_p, a)[0];

		double td = r + gamma * v_s_p - v_s;

		if (td > 0)
			m_pActorMinibatch->addTuple(s, a, s_p, r);
	}
	else
	{
		//update the actor: only actions with a positive TD are saved in the minibatch, so we can use them as target
		m_pActorNetwork->train(m_pActorMinibatch, m_pActorMinibatch->a(), m_actorPolicy->getLearningRate());
		m_pActorMinibatch->clear();
	}

	//Critic network
	if (!m_pCriticMinibatch->isFull())
		m_pCriticMinibatch->addTuple(s, a, s_p, r);
	else
	{
		//evaluate V(s')
		m_pCriticOnlineNetwork->evaluate(m_pCriticMinibatch->s_p(), m_V_s_p);

		//calculate the target of the critic: r + gamma * V(s) - V(s')
		for (size_t tuple = 0; tuple < m_pCriticMinibatch->size(); tuple++)
			m_pCriticMinibatch->target()[tuple] = m_pCriticMinibatch->r()[tuple] + gamma * m_V_s_p[tuple];

		//update the critic
		m_pCriticOnlineNetwork->train(m_pCriticMinibatch, m_pCriticMinibatch->target(), m_criticVFunction->getLearningRate());
		m_pCriticMinibatch->clear();
	}

	SimGod* pSimGod = SimionApp::get()->pSimGod.ptr();

	if (pSimGod->bUpdateFrozenWeightsNow())
	{
		if (m_pCriticTargetNetwork)
			m_pCriticTargetNetwork->destroy();
		m_pCriticTargetNetwork = (IVFunctionNetwork*)m_pCriticOnlineNetwork->clone();
	}
	return 0.0;
}

double DeepCACLA::selectAction(const State *s, Action *a)
{
	vector<double>& policyOutput = m_pActorNetwork->evaluate(s, a);

	if (SimionApp::get()->pExperiment->isEvaluationEpisode())
	{
		//just copy the output of the policy to the action
		m_actorPolicy->vectorToAction(policyOutput, 0, a);
		return 1.0;
	}

	size_t noiseSignalIndex;
	double noise;

	for (size_t i = 0; i < m_actorPolicy->getUsedActionVariables().size(); i++)
	{
		//if there are less noise signals than output action variables, just use the last one
		noiseSignalIndex = std::min(i, m_noiseSignals.size() - 1);

		noise = m_noiseSignals[i]->getSample();
		a->set(m_actorPolicy->getUsedActionVariables()[i].c_str()
			, policyOutput[i] + noise);
	}

	return 1.0;
}

#endif