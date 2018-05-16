#include "actor.h"
#include "noise.h"
#include "controller.h"
#include "actor.h"
#include "policy.h"
#include "named-var-set.h"
#include "config.h"
#include "policy-learner.h"
#include "app.h"
#include "vfa.h"
#include "featuremap.h"
#include "simgod.h"
#include "worlds\world.h"
#include "logger.h"
#include <algorithm>

Actor::Actor(ConfigNode* pConfigNode): DeferredLoad(10)
{
	m_policyLearners= MULTI_VALUE_FACTORY<PolicyLearner>(pConfigNode, "Output", "The outputs of the actor. One for each output dimension");
	m_pInitController= CHILD_OBJECT_FACTORY<Controller>(pConfigNode, "Base-Controller", "The base controller used to initialize the weights of the actor", true);
}

Actor::~Actor() {}

void Actor::deferredLoadStep()
{
	size_t controllerActionIndex, actorActionIndex;
	size_t numWeights;
	IMemBuffer *pWeights;
	State* s= SimionApp::get()->pWorld->getDynamicModel()->getStateInstance();
	Action* a= SimionApp::get()->pWorld->getDynamicModel()->getActionInstance();
	
	if (m_pInitController.shared_ptr())
	{
		size_t numActionDims = std::min(m_pInitController->getNumOutputs(), m_policyLearners.size());
		Logger::logMessage(MessageType::Info, "Initializing the policy weights using the base controller");
		//initialize the weights using the controller's output at each center point in state space
		for (size_t actionIndex = 0; actionIndex < numActionDims; actionIndex++)
		{
			controllerActionIndex = m_pInitController->getOutputActionIndex(actionIndex);
			for (size_t actorActionIndex = 0; actorActionIndex < m_policyLearners.size(); actorActionIndex++)
			{
				if (controllerActionIndex == m_policyLearners[actorActionIndex]->getPolicy()->getOutputActionIndex())
				{
					//controller's output action index and actor's match, so we use it to initialize
					numWeights = m_policyLearners[actorActionIndex]->getPolicy()->getDetPolicyStateVFA()->getNumWeights();
					pWeights = m_policyLearners[actorActionIndex]->getPolicy()->getDetPolicyStateVFA()->getWeights();
					for (size_t i = 0; i < numWeights; i++)
					{
						m_policyLearners[actorActionIndex]->getPolicy()->getDetPolicyStateVFA()->getStateFeatureMap()->getFeatureStateAction(i, s, a);
						m_pInitController->selectAction(s, a);
						(*pWeights)[i] = a->get(controllerActionIndex);
					}
				}
			}
		}
		Logger::logMessage(MessageType::Info, "Initialization done");
	}
	else
	{
		Logger::logMessage(MessageType::Info, "Initializing policy weights with null values");
		for (actorActionIndex = 0; actorActionIndex < m_policyLearners.size(); actorActionIndex++)
		{
			m_policyLearners[actorActionIndex]->getPolicy()->getDetPolicyStateVFA()->getWeights()->setInitValue(0.0);
			m_policyLearners[actorActionIndex]->getPolicy()->getDetPolicyStateVFA()->getWeights()->setInitValue(0.0);
		}
		Logger::logMessage(MessageType::Info, "Initialization done");
	}
	delete s;
	delete a;
}

double Actor::selectAction(const State *s, Action *a)
{
	double prob = 1.0;
	for (unsigned int i = 0; i<m_policyLearners.size(); i++)
	{
		//each uni-dimensional policy sets its own action's value
		prob*= m_policyLearners[i]->getPolicy()->selectAction(s, a);
	}
	return prob;
}

double Actor::getActionProbability(const State *s, const Action *a, bool bStochastic)
{
	double prob = 1.0;
	double ret;

	for (unsigned int i = 0; i<m_policyLearners.size(); i++)
	{
		//each uni-dimensional policy sets its own action's value
		ret = m_policyLearners[i]->getPolicy()->getProbability(s, a, bStochastic);

		if (SimionApp::get()->pSimGod->useSampleImportanceWeights())
			prob *= ret;
	}
	return prob;
}

void Actor::update(const State* s, const Action* a, const State* s_p, double r, double td)
{
	for (unsigned int i = 0; i<m_policyLearners.size(); i++)
	{
		//each uni-dimensional policy sets its own action's value
		m_policyLearners[i]->update(s, a, s_p, r, td);
	}
}