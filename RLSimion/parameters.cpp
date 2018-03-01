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

//Overriden Copy-assignment to avoid destroying copied buffer
//Expected use: problem= NEURAL_NETWORK_PROBLEM_DESCRIPTION(...)
NEURAL_NETWORK_PROBLEM_DESCRIPTION&  NEURAL_NETWORK_PROBLEM_DESCRIPTION::operator=(NEURAL_NETWORK_PROBLEM_DESCRIPTION& copied)
{
	//we move pointers to the copy
	m_pProblem = copied.m_pProblem;
	m_pNetwork = copied.m_pNetwork;
	//we remove pointers in the copy so that the destructor doesn't call destructors
	copied.m_pProblem = 0;
	copied.m_pNetwork = 0;
	m_name = copied.m_name;
	m_comment = copied.m_comment;

	return *this;
}


NEURAL_NETWORK_PROBLEM_DESCRIPTION::~NEURAL_NETWORK_PROBLEM_DESCRIPTION()
{
	if (m_pProblem) m_pProblem->destroy();
	if (m_pNetwork) m_pNetwork->destroy();
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