#include "stdafx.h"
#include "controller-adaptive.h"
#include "vfa.h"
#include "policy.h"
#include "app-rlsimion.h"

CExtendedWindTurbineVidalController::CExtendedWindTurbineVidalController(CConfigNode* pConfigNode) :CWindTurbineVidalController(pConfigNode)
{
	m_td = 0.0;
	m_pCritic = CHILD_OBJECT_FACTORY<CCritic>(pConfigNode, "Critic", "The critic used to learn");
	m_pAdpParamA = CHILD_OBJECT_FACTORY<CPolicyLearner>(pConfigNode, "A-adaptive-parameter", "The learner used for Vidal controller's parameter A");
	m_pAdpParamA->getPolicy()->getDetPolicyStateVFA()->setInitValue(m_pA.get());
	m_pAdpParamK_alpha = CHILD_OBJECT_FACTORY<CPolicyLearner>(pConfigNode, "KAlpha-adaptive-parameter", "The learner used for Vidal controller's parameter K_alpha");
	m_pAdpParamK_alpha->getPolicy()->getDetPolicyStateVFA()->setInitValue(m_pK_alpha.get());
	m_pAdpParamKP = CHILD_OBJECT_FACTORY<CPolicyLearner>(pConfigNode, "KP-adaptive-parameter", "The learner used for Vidal controller's parameter KP");
	m_pAdpParamKP->getPolicy()->getDetPolicyStateVFA()->setInitValue(m_pKP.get());
}

CExtendedWindTurbineVidalController::~CExtendedWindTurbineVidalController()
{}

double CExtendedWindTurbineVidalController::selectAction(const CState *s, CAction *a)
{
	//A= f_a(v)
	//K_alpha= f_k_alpha(v)
	//KP= f_kp(v)
	m_pA.set(m_pAdpParamA->getPolicy()->getOutput(s));
	m_pK_alpha.set(m_pAdpParamK_alpha->getPolicy()->getOutput(s));
	m_pKP.set(m_pAdpParamKP->getPolicy()->getOutput(s));

	CWindTurbineVidalController::selectAction(s, a);

	return 1.0;
}
void CExtendedWindTurbineVidalController::update(const CState *s, const CAction *a, const CState *s_p, double r)
{
	m_td= m_pCritic->update(s, a, s_p, r);

	m_pAdpParamA->update(s, a, s_p, r, m_td);
	m_pAdpParamK_alpha->update(s, a, s_p, r, m_td);
	m_pAdpParamKP->update(s, a, s_p, r, m_td);
}
