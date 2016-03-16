#include "stdafx.h"
#include "actor-critic.h"
#include "globals.h"
#include "parameters.h"
#include "actor.h"
#include "critic.h"

CLASS_CONSTRUCTOR(CActorCritic)(CParameters* pParameters) : CParamObject(pParameters)
{
	CParameters* pChild = pParameters->getChild();
	const char* type = pChild->getName();
	m_td = 0.0;

	CHILD_CLASS(m_pActor, "Actor", CActor, pParameters->getChild("Actor"));
	CHILD_CLASS_FACTORY(m_pCritic, "Critic", CCritic, pParameters);
	END_CLASS();

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