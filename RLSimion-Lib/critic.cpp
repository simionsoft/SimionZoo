#include "stdafx.h"
#include "critic.h"
#include "vfa-critic.h"
#include "parameters.h"
#include "globals.h"
#include "logger.h"
#include "vfa.h"

CLASS_CONSTRUCTOR(CCritic) : CParamObject(pParameters)
{
	CHILD_CLASS_FACTORY(m_pVFunction, "V-Function","The parameterization of the V-Function to be learned","", CLinearStateVFA);

	END_CLASS();
}

CLASS_FACTORY(CCritic)
{
	CHOICE("Critic","Critic type");
	CHOICE_ELEMENT("TD-Lambda", CTDLambdaCritic,"TD-Lambda algorithm");
	CHOICE_ELEMENT("True-Online-TD-Lambda", CTrueOnlineTDLambdaCritic,"True-online TD-Lambda algorithm");
	CHOICE_ELEMENT("TDC-Lambda", CTDCLambdaCritic,"TDC-Lambda algorithm");
	END_CHOICE();

	END_CLASS();

	return 0;
}

CCritic::~CCritic()
{

	delete m_pVFunction;
}
