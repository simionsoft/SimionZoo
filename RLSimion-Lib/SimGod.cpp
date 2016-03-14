#include "stdafx.h"
#include "SimGod.h"
#include "critic.h"
#include "actor.h"
#include "globals.h"
#include "world.h"
#include "experiment.h"
#include "reward.h"
#include "named-var-set.h"
#include "parameters.h"
#include "simion.h"

CLASS_CONSTRUCTOR(CSimGod)(CParameters* pParameters)
{
	m_numSimions = pParameters->countChildren("Simion");
	m_pSimions = new CSimion*[m_numSimions];
	CParameters* pChild = pParameters->getChild("Simion");
	for (int i = 0; i < m_numSimions; i++)
	{
		MULTI_VALUED_FACTORY(m_pSimions[i], "Simion", CSimion, pChild);
		pChild = pChild->getNextChild("Simion");
	}
	
	//m_td = 0.0;
	//RLSimion::g_pLogger->addVarToStats("Critic", "TD-error", &m_td);
	END_CLASS();
}


CSimGod::~CSimGod()
{
	for (int i = 0; i < m_numSimions; i++)
	{
		delete m_pSimions[i];
	}
	delete[] m_pSimions;
}


void CSimGod::selectAction(CState* s, CAction* a)
{

	for (int i = 0; i < m_numSimions; i++)
		m_pSimions[i]->selectAction(s, a);

	
	//m_pController->selectAction(s, a);

	//if (m_pController == m_pActor)
	//	m_rho = 1.0;
	//else
	//{
	//	double controllerProb = m_pController->getProbability(s, a);
	//	double actorProb = m_pActor->getProbability(s, a);
	//	m_rho = actorProb / controllerProb;
	//}
	//	
	//return m_rho;
}

void CSimGod::update(CState* s, CAction* a, CState* s_p, double r)
{
	if (RLSimion::g_pExperiment->isEvaluationEpisode()) return;

	//update critic
	for (int i = 0; i < m_numSimions; i++)
		m_pSimions[i]->updateValue(s, a,s_p,r);
	/*if (m_pCritic && !RLSimion::g_pExperiment->isEvaluationEpisode())
		m_td = m_pCritic->updateValue(s, a, s_p, r, m_rho);*/

	//update actor: might be the controller
	for (int i = 0; i < m_numSimions; i++)
		m_pSimions[i]->updatePolicy(s, a,s_p,r);

	//if( !RLSimion::g_pExperiment->isEvaluationEpisode())
	//	m_pActor->updatePolicy(s, a, s_p, r, m_td);

}
