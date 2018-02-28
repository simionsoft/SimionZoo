#include "critic.h"
#include "vfa-critic.h"
#include "config.h"
#include "logger.h"
#include "vfa.h"
#include "q-learners.h"

std::shared_ptr<ICritic> ICritic::getInstance(CConfigNode* pConfigNode)
{
	return CHOICE<ICritic>(pConfigNode, "Critic", "Critic type",
	{
		{ "TD-Lambda",CHOICE_ELEMENT_NEW<CTDLambdaCritic> },
		{ "True-Online-TD-Lambda",CHOICE_ELEMENT_NEW<CTrueOnlineTDLambdaCritic> },
		{ "TDC-Lambda",CHOICE_ELEMENT_NEW<CTDCLambdaCritic> },
		{ "Q-Learning",CHOICE_ELEMENT_NEW<CQLearningCritic> }
	});
}

CVLearnerCritic::CVLearnerCritic(CConfigNode* pConfigNode)
{
	m_pVFunction = CHILD_OBJECT<CLinearStateVFA>(pConfigNode, "V-Function", "The V-function to be learned");
	m_pVFunction->setCanUseDeferredUpdates(true);
}



CVLearnerCritic::~CVLearnerCritic()
{
}
