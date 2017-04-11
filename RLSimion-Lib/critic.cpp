#include "stdafx.h"
#include "critic.h"
#include "vfa-critic.h"
#include "config.h"
#include "logger.h"
#include "vfa.h"

CCritic::CCritic(CConfigNode* pConfigNode)
{
	m_pVFunction = CHILD_OBJECT<CLinearStateVFA>(pConfigNode, "V-Function", "The V-function to be learned");
	m_pVFunction->setCanUseDeferredUpdates(true);
}

std::shared_ptr<CCritic> CCritic::getInstance(CConfigNode* pConfigNode)
{
	return CHOICE<CCritic>(pConfigNode, "Critic", "Critic type",
	{
		{"TD-Lambda",CHOICE_ELEMENT_NEW<CTDLambdaCritic>},
		{"True-Online-TD-Lambda",CHOICE_ELEMENT_NEW<CTrueOnlineTDLambdaCritic>},
		{"TDC-Lambda",CHOICE_ELEMENT_NEW<CTDCLambdaCritic>}
	});
}

CCritic::~CCritic()
{
}
