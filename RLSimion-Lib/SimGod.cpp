#include "stdafx.h"
#include "SimGod.h"
#include "parameters.h"
#include "critic.h"
#include "actor.h"


CSimGod::CSimGod(CParameters* pParameters)
{
	char* configType = pParameters->getStringPtr("SIMGOD/TYPE");
	if (strcmp(configType, "ACTOR-CRITIC")==0)
	{
		m_pActor = CActor::getActorInstance(pParameters);
		m_pCritic = CCritic::getCriticInstance(pParameters);
	}
	else if (strcmp(configType, "ACTOR-ONLY") == 0)
	{
		m_pActor = CActor::getActorInstance(pParameters);
		m_pCritic = 0;
	}
	
	
}


CSimGod::~CSimGod()
{
	if (m_pCritic) delete m_pCritic;
	delete m_pActor;
}


double CSimGod::selectAction(CState* s, CAction* a)
{
	return m_pActor->selectAction(s, a);
}

double CSimGod::update(CState* s, CAction* a, CState* s_p, double r)
{
	double td= 0.0;

	//update critic
	if (m_pCritic)
		td = m_pCritic->updateValue(s, a, s_p, r, 1.0);// , rho);

	//update actor
	m_pActor->updatePolicy(s, a, s_p, r, td);

	return td;
}
