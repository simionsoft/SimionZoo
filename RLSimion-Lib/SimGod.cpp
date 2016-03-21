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

CLASS_INIT(CSimGod)
{
	m_numSimions = pParameters->countChildren("Simion");
	m_pSimions = new CSimion*[m_numSimions];
	CParameters* pChild = pParameters->getChild("Simion");
	for (int i = 0; i < m_numSimions; i++)
	{
		MULTI_VALUED_FACTORY(m_pSimions[i], "Simions", "Simions: agents and controllers",CSimion, pChild);
		pChild = pChild->getNextChild("Simion");
	}
	
	//m_td = 0.0;
	//RLSimion::Logger.addVarToStats("Critic", "TD-error", &m_td);
	END_CLASS();
}

CSimGod::CSimGod()
{
	m_numSimions = 0;
	m_pSimions = 0;
}


CSimGod::~CSimGod()
{
	if (m_pSimions)
	{
		for (int i = 0; i < m_numSimions; i++)
		{
			if (m_pSimions[i]) delete m_pSimions[i];
		}
		delete[] m_pSimions;
	}
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
	if (RLSimion::Experiment.isEvaluationEpisode()) return;

	//update critic
	for (int i = 0; i < m_numSimions; i++)
		m_pSimions[i]->updateValue(s, a,s_p,r);
	/*if (m_pCritic && !RLSimion::Experiment.isEvaluationEpisode())
		m_td = m_pCritic->updateValue(s, a, s_p, r, m_rho);*/

	//update actor: might be the controller
	for (int i = 0; i < m_numSimions; i++)
		m_pSimions[i]->updatePolicy(s, a,s_p,r);

	//if( !RLSimion::Experiment.isEvaluationEpisode())
	//	m_pActor->updatePolicy(s, a, s_p, r, m_td);

}
