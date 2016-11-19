#include "stdafx.h"
#include "critic.h"
#include "vfa-critic.h"
#include "config.h"
#include "globals.h"
#include "logger.h"
#include "vfa.h"

CCritic::CCritic(CConfigNode* pConfigNode)
{
	m_pVFunction = CHILD_OBJECT<CCritic>(pConfigNode, "V-Function", "The V-function to be learned");
	//CHILD_CLASS(m_pVFunction, "V-Function","The parameterization of the V-Function to be learned",false, CLinearStateVFA);

	//END_CLASS();
}

std::shared_ptr<CCritic> CCritic::getInstance(CConfigNode* pConfigNode)
{
	return CHOICE<CCritic>(pConfigNode, "Critic", "Critic type",
	{
		CHOICE_ELEMENT_NEW(pConfigNode, CTDLambdaCritic,"TD-Lambda","TD-Lambda algorithm",""),
		CHOICE_ELEMENT_NEW(pConfigNode, CTrueOnlineTDLambdaCritic,"True-Online-TD-Lambda","True-online TD-Lambda algorithm",""),
		CHOICE_ELEMENT_NEW(pConfigNode,CTDCLambdaCritic,"TDC-Lambda", "TDC-Lambda algorithm","")
	});
	//CHOICE("Critic","Critic type");
	//CHOICE_ELEMENT("TD-Lambda", CTDLambdaCritic,"TD-Lambda algorithm");
	//CHOICE_ELEMENT("True-Online-TD-Lambda", CTrueOnlineTDLambdaCritic,"True-online TD-Lambda algorithm");
	//CHOICE_ELEMENT("TDC-Lambda", CTDCLambdaCritic,"TDC-Lambda algorithm");
	//END_CHOICE();

	//END_CLASS();

	//return 0;
}

CCritic::~CCritic()
{

	delete m_pVFunction;
}
