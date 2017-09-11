#include "stdafx.h"
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

NEURAL_NETWORK_PROBLEM_DESCRIPTION::NEURAL_NETWORK_PROBLEM_DESCRIPTION(CConfigNode * pConfigNode, const char * name, const char * comment)
{
	m_pProblem = CHILD_OBJECT<CProblem>(pConfigNode, name, comment);
}
