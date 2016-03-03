#include "stdafx.h"
#include "SimGod.h"
#include "critic.h"
#include "actor.h"
#include "globals.h"
#include "world.h"
#include "experiment.h"
#include "logger.h"
#include "reward.h"
#include "named-var-set.h"
#include "parameters.h"

CSimGod::CSimGod(CParameters* pParameters)
{
	/*CParameters* child;/* = pParameters->getChild("ACTOR/CRITIC");

	if (child)
	{
		m_pController = CActorCritic::getInstance(child->getChild("ACTOR/CRITIC"));
		m_pActor = m_pController;
		m_pCritic = (CCritic*) m_pController;
	}
	else*/
	{
		if (pParameters->getChild("Controller"))
		{
			m_pController = CActor::getInstance(pParameters->getChild("Controller"));
			m_pActor = CActor::getInstance(pParameters->getChild("Actor"));
		}
		else
		{
			m_pActor = CActor::getInstance(pParameters->getChild("Actor"));
			m_pController = m_pActor;
		}

		if (pParameters->getChild("Critic"))
			m_pCritic = CCritic::getInstance(pParameters->getChild("Critic"));
		else m_pCritic = 0;
	}
	
	m_rho = 0.0;
	m_td = 0.0;
	RLSimion::g_pExperiment->m_pLogger->addVarToStats("Critic", "TD-error", &m_td);

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
	//update critic
	if (m_pCritic)
		m_td = m_pCritic->updateValue(s, a, s_p, r, m_rho);

	//update actor: might be the controller
	m_pActor->updatePolicy(s, a, s_p, r, m_td);

	return m_td;
}
