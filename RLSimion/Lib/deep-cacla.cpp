#include "deep-cacla.h"
#include "deep-network.h"
#include "deep-minibatch.h"
#include "simgod.h"
#include "app.h"

DeepCACLA::DeepCACLA(ConfigNode* pConfigNode)
{
	m_actorPolicy = CHILD_OBJECT<DeepDeterministicPolicy>(pConfigNode, "Policy", "Neural Network used to represent the actors policy");
	m_criticVFunction= CHILD_OBJECT<DeepVFunction>(pConfigNode, "Value-Function", "Value function learned by the critic");
	CNTK::WrapperClient::RegisterDependencies();
}


DeepCACLA::~DeepCACLA()
{
	if (m_pActorMinibatch) delete m_pActorMinibatch;
	if (m_pActorNetwork) m_pActorNetwork->destroy();
	
	if (m_pCriticMinibatch) delete m_pCriticMinibatch;
	if (m_pCriticNetwork) m_pCriticNetwork->destroy();

	CNTK::WrapperClient::UnLoad();
}


void DeepCACLA::deferredLoadStep()
{
	CNTK::WrapperClient::Load();

	m_pActorMinibatch = m_actorPolicy->getMinibatch();
	m_pActorNetwork= m_actorPolicy->getNetworkInstance();

	m_pCriticMinibatch = m_criticVFunction->getMinibatch();
	m_pCriticNetwork = m_criticVFunction->getNetworkInstance();
	m_V_s = vector<double>(m_pCriticMinibatch->size());
	m_V_s_p = vector<double>(m_pCriticMinibatch->size());
}

double DeepCACLA::update(const State *s, const Action *a, const State *s_p, double r, double probability)
{
	if (!m_pActorMinibatch->isFull() && !m_pCriticMinibatch->isFull())
	{
		m_pActorMinibatch->addTuple(s, a, s_p, r);
		m_pCriticMinibatch->addTuple(s, a, s_p, r);
	}
	else
	{
		double gamma = SimionApp::get()->pSimGod->getGamma();

		//evaluate V(s)
		m_pCriticNetwork->evaluate(m_pCriticMinibatch->s(),m_V_s);

		//evaluate V(s')
		m_pCriticNetwork->evaluate(m_pCriticMinibatch->s_p(), m_V_s_p);

		//evalute pi(s)
		m_pActorNetwork->evaluate(m_pActorMinibatch->s(), m_pActorMinibatch->target());

		//calculate the target of the critic: r + gamma * V(s) - V(s')
		for (size_t tuple = 0; tuple < m_pCriticMinibatch->size(); tuple++)
		{
			m_pCriticMinibatch->target()[tuple] = m_pCriticMinibatch->r()[tuple] + gamma * m_V_s_p[tuple] - m_V_s[tuple];

			//if delta_t > 0 shift the policy toward a_t, otherwise target the policy's output
			if (m_pCriticMinibatch->target()[tuple] > 0)
				m_pActorMinibatch->copyElement(m_pActorMinibatch->a(), m_pActorMinibatch->target(), tuple);
		}

		//update the critic
		m_pCriticNetwork->train(m_pCriticMinibatch, m_pCriticMinibatch->target());

		//update the actor
		m_pActorNetwork->train(m_pActorMinibatch, m_pActorMinibatch->target());
	}
	return 0.0;
}

double DeepCACLA::selectAction(const State *s, Action *a)
{
	//m_pActorNetwork->evaluate()
	return 1.0;
}