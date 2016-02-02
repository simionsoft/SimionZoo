#include "stdafx.h"
#include "critic.h"
#include "parameters.h"
#include "parameter.h"
#include "vfa.h"



CCritic *CCritic::getInstance(CParameters* pParameters)
{
	if (!pParameters) return 0;


	const char* type = pParameters->getChild(0)->getName();
	if (!)
	if (strcmp(pAlgorithm, "TD-Lambda") == 0)
		return new CTDLambdaCritic(pParameters);
	else if (strcmp(pAlgorithm, "True-Online-TD-Lambda") == 0)
		return new CTrueOnlineTDLambdaCritic(pParameters);
	else if (strcmp(pAlgorithm, "TDC-Lambda") == 0)
		return new CTDCLambdaCritic(pParameters);

	return 0;
}
