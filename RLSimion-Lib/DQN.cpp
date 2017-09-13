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
	for (unsigned int i = 0; i < m_policies.size(); i++)
	{
		m_policies[i]->selectAction(s, a);
	}
	
	return 0.0;
}

double CDQN::update(const CState * s, const CAction * a, const CState * s_p, double r, double behaviorProb)
{
	//train now
	//m_QNetwork.getNetwork()->getTrainer()


	//every C time steps clone the q-network to set the new target network
	int step = (int)(CSimionApp::get()->pWorld->getTotalSimTime() / CSimionApp::get()->pWorld->getDT());
	if (step == CSimionApp::get()->pSimGod->getTargetFunctionUpdateFreq())
	{
		//m_pTargetNetwork = m_QNetwork.getNetwork()->getNetworkFunctionPtr()->Clone(CNTK::ParameterCloningMethod::Freeze);
	}

	for (unsigned int i = 0; i < m_policies.size(); i++)
	{
		m_policies[i]->updateNetwork(s, a, s_p, r);
	}

	return 0.0;
}
