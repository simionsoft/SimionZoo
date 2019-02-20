#include "critic.h"
#include "vfa-critic.h"
#include "config.h"
#include "logger.h"
#include "vfa.h"
#include "q-learners.h"
#include "app.h"

std::shared_ptr<ICritic> ICritic::getInstance(ConfigNode* pConfigNode)
{
	return CHOICE<ICritic>(pConfigNode, "Critic", "Critic type",
	{
		{ "TD-Lambda",CHOICE_ELEMENT_NEW<TDLambdaCritic> },
		{ "True-Online-TD-Lambda",CHOICE_ELEMENT_NEW<TrueOnlineTDLambdaCritic> },
		{ "TDC-Lambda",CHOICE_ELEMENT_NEW<TDCLambdaCritic> },
		{ "Q-Learning",CHOICE_ELEMENT_NEW<QLearningCritic> }
	});
}

VLearnerCritic::VLearnerCritic(ConfigNode* pConfigNode)
{
	m_pVFunction = CHILD_OBJECT<LinearStateVFA>(pConfigNode, "V-Function", "The V-function to be learned");
	SimionApp::get()->registerStateActionFunction("V", m_pVFunction.ptr());
	m_pVFunction->setCanUseDeferredUpdates(true);
}



VLearnerCritic::~VLearnerCritic()
{
}
