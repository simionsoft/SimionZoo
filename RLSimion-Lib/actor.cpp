#include "stdafx.h"
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

CActor::CActor(CConfigNode* pConfigNode): CDeferredLoad(10)
{
	m_policyLearners= MULTI_VALUE_FACTORY<CPolicyLearner>(pConfigNode, "Output", "The outputs of the actor. One for each output dimension");
	m_pInitController= CHILD_OBJECT_FACTORY<CController>(pConfigNode, "Base-Controller", "The base controller used to initialize the weights of the actor", true);
}

CActor::~CActor() {}

void CActor::deferredLoadStep()
{
	unsigned int controllerActionIndex, actorActionIndex;
	unsigned int numWeights;
	double *pWeights;
	CState* s= CSimionApp::get()->pWorld->getDynamicModel()->getStateInstance();
	CAction* a= CSimionApp::get()->pWorld->getDynamicModel()->getActionInstance();
	
	if (m_pInitController.shared_ptr())
	{
		int numActionDims = std::min(m_pInitController->getNumOutputs(), (int)m_policyLearners.size());
		CLogger::logMessage(MessageType::Info, "Initializing the policy weights using the base controller");
		//initialize the weights using the controller's output at each center point in state space
		for (int actionIndex = 0; actionIndex < numActionDims; actionIndex++)
		{
			controllerActionIndex = m_pInitController->getOutputActionIndex(actionIndex);
			for (unsigned int actorActionIndex = 0; actorActionIndex < m_policyLearners.size(); actorActionIndex++)
			{
				if (controllerActionIndex == m_policyLearners[actorActionIndex]->getPolicy()->getOutputActionIndex())
				{
					//controller's output action index and actor's match, so we use it to initialize
					numWeights = m_policyLearners[actorActionIndex]->getPolicy()->getDetPolicyStateVFA()->getNumWeights();
					pWeights = m_policyLearners[actorActionIndex]->getPolicy()->getDetPolicyStateVFA()->getWeightPtr();
					for (unsigned int i = 0; i < numWeights; i++)
					{
						m_policyLearners[actorActionIndex]->getPolicy()->getDetPolicyStateVFA()->getStateFeatureMap()->getFeatureState(i, s);
						m_pInitController->selectAction(s, a);
						pWeights[i] = a->get(controllerActionIndex);//m_pPolicyLearners[actorActionIndex]->getPolicy()->getDetPolicyStateVFA()->getSample(s);
					}
				}
			}
		}
		CLogger::logMessage(MessageType::Info, "Initialization done");
	}
	else
	{
		CLogger::logMessage(MessageType::Info, "Initializing policy weights with null values");
		for (actorActionIndex = 0; actorActionIndex < m_policyLearners.size(); actorActionIndex++)
		{
			numWeights = m_policyLearners[actorActionIndex]->getPolicy()->getDetPolicyStateVFA()->getNumWeights();
			pWeights = m_policyLearners[actorActionIndex]->getPolicy()->getDetPolicyStateVFA()->getWeightPtr();
			assert(pWeights);

			for (unsigned int i = 0; i < numWeights; i++)
			{
				pWeights[i] = 0.0;
			}
		}
		CLogger::logMessage(MessageType::Info, "Initialization done");
	}
	delete s;
	delete a;
}

double CActor::selectAction(const CState *s, CAction *a)
{
	double prob = 1.0;
	for (unsigned int i = 0; i<m_policyLearners.size(); i++)
	{
		//each uni-dimensional policy sets its own action's value
		prob*= m_policyLearners[i]->getPolicy()->selectAction(s, a);
	}
	return prob;
}

double CActor::getActionProbability(const CState *s, const CAction *a, bool bStochastic)
{
	double prob = 1.0;
	double ret;

	for (unsigned int i = 0; i<m_policyLearners.size(); i++)
	{
		//each uni-dimensional policy sets its own action's value
		ret = m_policyLearners[i]->getPolicy()->getProbability(s, a, bStochastic);

		if (CSimionApp::get()->pSimGod->useSampleImportanceWeights())
			prob *= ret;
	}
	return prob;
}

void CActor::update(const CState* s, const CAction* a, const CState* s_p, double r, double td)
{
	for (unsigned int i = 0; i<m_policyLearners.size(); i++)
	{
		//each uni-dimensional policy sets its own action's value
		m_policyLearners[i]->update(s, a, s_p, r, td);
	}
}