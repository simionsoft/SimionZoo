#include "stdafx.h"
#include "SimGod.h"
#include "critic.h"
#include "actor.h"
//#include "actor-critic.h"

CSimGod::CSimGod(tinyxml2::XMLElement* pParameters)
{
	/*tinyxml2::XMLElement* child;/* = pParameters->FirstChildElement("ACTOR/CRITIC");

	if (child)
	{
		m_pController = CActorCritic::getInstance(child->FirstChildElement("ACTOR/CRITIC"));
		m_pActor = m_pController;
		m_pCritic = (CCritic*) m_pController;
	}
	else*/
	{
		if (pParameters->FirstChildElement("CONTROLLER"))
		{
			m_pController = CActor::getInstance(pParameters->FirstChildElement("CONTROLLER"));
			m_pActor = CActor::getInstance(pParameters->FirstChildElement("ACTOR"));
		}
		else
		{
			m_pActor = CActor::getInstance(pParameters->FirstChildElement("ACTOR"));
			m_pController = m_pActor;
		}

		if (pParameters->FirstChildElement("CRITIC"))
			m_pCritic = CCritic::getInstance(pParameters->FirstChildElement("CRITIC"));
		else m_pCritic = 0;
	}
	
	m_rho = 0.0;
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
	m_pController->selectAction(s, a);

	if (m_pController == m_pActor)
		m_rho = 1.0;
	else
	{
		double controllerProb = m_pController->getProbability(s, a);
		double actorProb = m_pActor->getProbability(s, a);
		m_rho = actorProb / controllerProb;
	}
		
	return m_rho;
}

double CSimGod::update(CState* s, CAction* a, CState* s_p, double r)
{
	double td= 0.0;

	//update critic
	if (m_pCritic)
		td = m_pCritic->updateValue(s, a, s_p, r, m_rho);

	//update actor: might be the controller
	m_pActor->updatePolicy(s, a, s_p, r, td);

	return td;
}
