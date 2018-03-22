#include "parameters.h"
#include "worlds/world.h"

STATE_VARIABLE::STATE_VARIABLE(ConfigNode* pConfigNode, const char* name, const char* comment)
{
	m_hVariable = World::getDynamicModel()->getStateDescriptor().getVarIndex(pConfigNode->getConstString(name));
	m_name = name;
	m_comment = comment;
}

ACTION_VARIABLE::ACTION_VARIABLE(ConfigNode* pConfigNode, const char* name, const char* comment)
{
	m_hVariable = World::getDynamicModel()->getActionDescriptor().getVarIndex(pConfigNode->getConstString(name));
	m_name = name;
	m_comment = comment;
}
#ifdef _WIN64
#include "../tools/CNTKWrapper/CNTKWrapper.h"

//Overriden Copy-assignment to avoid destroying copied buffer
//Expected use: problem= NN_DEFINITION(...)
NN_DEFINITION&  NN_DEFINITION::operator=(NN_DEFINITION& copied)
{
	//we move pointers to the copy
	m_pProblem = copied.m_pProblem;

	copied.m_pProblem = nullptr;

	m_name = copied.m_name;
	m_comment = copied.m_comment;

	return *this;
}

#include "logger.h"

NN_DEFINITION::~NN_DEFINITION()
{
}

void NN_DEFINITION::destroy()
{
	Logger::logMessage(MessageType::Info, "Called NN_DEFINITION::destroy()");
	if (m_pProblem)
	{
		Logger::logMessage(MessageType::Info, "Called IProblem::destroy()");
		m_pProblem->destroy();
		Logger::logMessage(MessageType::Info, "Finished IProblem::destroy()");
		m_pProblem = nullptr;
	}
	Logger::logMessage(MessageType::Info, "Finished NN_DEFINITION::destroy()");
}

INetwork* NN_DEFINITION::buildNetwork()
{
	INetwork* pNetwork= m_pProblem->createNetwork();
	pNetwork->setParent(m_pProblem);
	pNetwork->buildNetworkFunctionPtr(m_pProblem->getOptimizerSetting());
	return pNetwork;
}


IProblem* NN_DEFINITION::get() { return m_pProblem; }

#endif