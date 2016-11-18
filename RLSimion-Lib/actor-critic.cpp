#include "stdafx.h"
#include "actor-critic.h"
#include "globals.h"
#include "config.h"
#include "actor.h"
#include "critic.h"
#include "app.h"

CActorCritic::CActorCritic(CConfigNode* pConfigNode)
{
	m_td = 0.0;
	m_pActor = CHILD_OBJECT<CActor>(pConfigNode, "Actor", "The actor");
	//CHILD_CLASS(m_pActor, "Actor", "The actor",false,CActor);
	m_pCritic = CHILD_OBJECT_FACTORY<CCritic>(pConfigNode, "Critic", "The critic");
	//CHILD_CLASS_FACTORY(m_pCritic, "Critic","The critic", false,CCritic);
	//END_CLASS();
	CSimionApp::get()->pLogger->addVarToStats("TD-error", "TD-error", &m_td);
}

void CActorCritic::selectAction(const CState *s, CAction *a)
{
	m_pActor->selectAction(s, a);
}

void CActorCritic::updateValue(const CState *s, const CAction *a, const CState *s_p, double r)
{
	m_td= m_pCritic->updateValue(s, a, s_p, r);
}


void CActorCritic::updatePolicy(const CState *s, const CAction *a, const CState *s_p, double r)
{
	m_pActor->updatePolicy(s, a, s_p, r, m_td);
}