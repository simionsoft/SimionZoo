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

CLASS_CONSTRUCTOR(CSimGod)(CParameters* pParameters)
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
		CHILD_CLASS_FACTORY(m_pController, "Controller", CActor, pParameters->getChild("Controller"));
		CHILD_CLASS_FACTORY(m_pActor, "Actor", CActor, pParameters->getChild("Actor"));
		CHILD_CLASS_FACTORY(m_pCritic, "Critic", CCritic, pParameters->getChild("Critic"));
		if (!m_pController) m_pController = m_pActor;
	}
	
	m_rho = 0.0;
	m_td = 0.0;
	RLSimion::g_pLogger->addVarToStats("Critic", "TD-error", &m_td);
	END_CLASS();
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
	if (m_pCritic && !RLSimion::g_pExperiment->isEvaluationEpisode())
		m_td = m_pCritic->updateValue(s, a, s_p, r, m_rho);

	//update actor: might be the controller
	if( !RLSimion::g_pExperiment->isEvaluationEpisode())
		m_pActor->updatePolicy(s, a, s_p, r, m_td);

	return m_td;
}
