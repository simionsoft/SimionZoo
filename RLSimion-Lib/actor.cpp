#include "stdafx.h"
#include "actor.h"
#include "noise.h"
#include "controller.h"
#include "actor.h"
#include "policy.h"
#include "globals.h"
#include "named-var-set.h"
#include "parameters.h"
#include "policy-learner.h"
#include "app.h"
#include "vfa.h"
#include "featuremap.h"


CLASS_CONSTRUCTOR(CActor) : CParamObject(pParameters), CDeferredLoad(10)//if initialized, it has to be done after allocating memory for the weights
{
	MULTI_VALUED_FACTORY(m_policyLearners,"Output","The outputs of the actor. One for each output dimension",CPolicyLearner);
	CHILD_CLASS_FACTORY(m_pInitController, "Base-Controller", "The base controller used to initialize the weights of the actor", true, CController);
	END_CLASS();
}



CActor::~CActor()
{
	delete m_pInitController;
	for (unsigned int i = 0; i < m_policyLearners.size(); i++)
	{
		delete m_policyLearners[i];
	}
}

void CActor::deferredLoadStep()
{
	unsigned int controllerActionIndex, actorActionIndex;
	unsigned int numWeights;
	double *pWeights;
	CState* s= CApp::get()->pWorld->getDynamicModel()->getStateInstance();
	CAction* a= CApp::get()->pWorld->getDynamicModel()->getActionInstance();
	
	if (m_pInitController)
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
						pWeights[i] = a->getValue(controllerActionIndex);//m_pPolicyLearners[actorActionIndex]->getPolicy()->getDetPolicyStateVFA()->getValue(s);
					}
				}
			}
		}
		CLogger::logMessage(MessageType::Info, "Initialization finished");
	}
	else
	{
		CLogger::logMessage(MessageType::Info, "Initializing policy weights with null values");
		for (actorActionIndex = 0; actorActionIndex < m_policyLearners.size(); actorActionIndex++)
		{
			numWeights = m_policyLearners[actorActionIndex]->getPolicy()->getDetPolicyStateVFA()->getNumWeights();
			pWeights = m_policyLearners[actorActionIndex]->getPolicy()->getDetPolicyStateVFA()->getWeightPtr();
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

void CActor::selectAction(const CState *s, CAction *a)
{
	for (unsigned int i = 0; i<m_policyLearners.size(); i++)
	{
		//each uni-dimensional policy sets its own action's value
		m_policyLearners[i]->getPolicy()->selectAction(s, a);
	}
}

void CActor::updatePolicy(const CState* s, const CAction* a, const CState* s_p, double r, double td)
{
	for (unsigned int i = 0; i<m_policyLearners.size(); i++)
	{
		//each uni-dimensional policy sets its own action's value
		m_policyLearners[i]->updatePolicy(s, a, s_p, r, td);
	}
}