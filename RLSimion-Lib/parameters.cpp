#include "parameters.h"
#include "worlds/world.h"

STATE_VARIABLE::STATE_VARIABLE(CConfigNode* pConfigNode, const char* name, const char* comment)
{
	m_hVariable = CWorld::getDynamicModel()->getStateDescriptor().getVarIndex(pConfigNode->getConstString(name));
	m_name = name;
	m_comment = comment;
}

ACTION_VARIABLE::ACTION_VARIABLE(CConfigNode* pConfigNode, const char* name, const char* comment)
{
	m_hVariable = CWorld::getDynamicModel()->getActionDescriptor().getVarIndex(pConfigNode->getConstString(name));
	m_name = name;
	m_comment = comment;
}
#ifdef _WIN64
#include "../tools/CNTKWrapper/CNTKWrapper.h"

NEURAL_NETWORK_PROBLEM_DESCRIPTION::~NEURAL_NETWORK_PROBLEM_DESCRIPTION()
{
	m_pProblem->destroy();
	m_pNetwork->destroy();
}

void NEURAL_NETWORK_PROBLEM_DESCRIPTION::buildNetwork()
{
	if (m_pNetwork)
		return;

	m_pNetwork = m_pProblem->createNetwork();
	m_pNetwork->buildNetworkFunctionPtr(m_pProblem->getOptimizerSetting());
}

INetwork* NEURAL_NETWORK_PROBLEM_DESCRIPTION::getNetwork()
{
	buildNetwork();
	return m_pNetwork;
}
IProblem* NEURAL_NETWORK_PROBLEM_DESCRIPTION::getProblem() { return m_pProblem; }

#endif