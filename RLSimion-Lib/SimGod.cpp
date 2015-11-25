#include "stdafx.h"
#include "SimGod.h"
#include "parameters.h"
#include "parameter.h"
#include "critic.h"
#include "actor.h"


CSimGod::CSimGod(CParameters* pParameters)
{
	const char* configType = pParameters->getParameter("TYPE")->getStringPtr();

	if (strcmp(configType, "ACTOR-CRITIC")==0)
	{
		m_pController = CActor::getActorInstance(pParameters->getChild("ACTOR"));
		m_pActor = m_pController;
		m_pCritic = CCritic::getCriticInstance(pParameters->getChild("CRITIC"));
	}
	else if(strcmp(configType, "ACTOR-CRITIC-CONTROLLER") == 0)
	{
		m_pController = CActor::getControllerInstance(pParameters->getChild("CONTROLLER")); //CActor is a singleton
		m_pActor = CActor::getActorInstance(pParameters->getChild("ACTOR"));
		m_pCritic = CCritic::getCriticInstance(pParameters->getChild("CRITIC"));
	}
	else if (strcmp(configType, "ACTOR-ONLY") == 0)
	{
		m_pController = CActor::getActorInstance(pParameters->getChild("ACTOR"));
		m_pActor = m_pController;
		m_pCritic = 0;
	}
	
	
}


CSimGod::~CSimGod()
{
	if (m_pCritic) delete m_pCritic;
	if (m_pController && m_pController != m_pActor) delete m_pController;
	if (m_pActor) delete m_pActor;
}


double CSimGod::selectAction(CState* s, CAction* a)
{
	//the controller selects the action: might be the actor
	return m_pController->selectAction(s, a);
}

double CSimGod::update(CState* s, CAction* a, CState* s_p, double r)
{
	double td= 0.0;

	//update critic
	if (m_pCritic)
		td = m_pCritic->updateValue(s, a, s_p, r, 1.0);// , rho);

	//update actor: might be the controller
	m_pActor->updatePolicy(s, a, s_p, r, td);

	return td;
}
