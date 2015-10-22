#include "stdafx.h"
#include "critic.h"
#include "parameters.h"



CCritic *CCritic::getInstance(char* configFile)
{
	CCritic* pCritic= 0;

	if (!configFile) return (CCritic*)0;


	CParameters *pParameters= new CParameters(configFile);

	if (strcmp(pParameters->getStringPtr("ALGORITHM"),"TD-Lambda")==0)
		pCritic= new CTDLambdaCritic(pParameters);
	else if (strcmp(pParameters->getStringPtr("ALGORITHM"),"True-Online-TD-Lambda")==0)
		pCritic= new CTrueOnlineTDLambdaCritic(pParameters);
	else if (strcmp(pParameters->getStringPtr("ALGORITHM"), "TDC-Lambda") == 0)
		pCritic = new CTDCLambdaCritic(pParameters);

	delete pParameters;

	return pCritic;
}

