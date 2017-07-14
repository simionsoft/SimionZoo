#include "stdafx.h"
#include "controller-adaptive.h"
#include "vfa.h"
#include "policy.h"
#include "app-rlsimion.h"
#include "worlds/world.h"

//Extended version of the Vidal controller

CExtendedWindTurbineVidalController::CExtendedWindTurbineVidalController(CConfigNode* pConfigNode) :CWindTurbineVidalController(pConfigNode)
{
	CDynamicModel* pWorld = CSimionApp::get()->pWorld->getDynamicModel();
	CDescriptor& pActionDescriptor = pWorld->getActionDescriptor();
	m_pCritic = CHILD_OBJECT_FACTORY<ICritic>(pConfigNode, "Critic", "The critic used to learn");
	//A parameter
	m_pALearner = CHILD_OBJECT_FACTORY<CPolicyLearner>(pConfigNode, "A-adaptive-parameter", "The learner used for Vidal controller's parameter A");
	m_aActionId= pWorld->addActionVariable("A", "unitless", 0.0, 100.0);
	m_pALearner->getPolicy()->setOutputActionIndex(m_aActionId);
	m_pALearner->getPolicy()->getDetPolicyStateVFA()->setInitValue(m_pA.get());
	m_pALearner->getPolicy()->getDetPolicyStateVFA()->saturateOutput(
		pActionDescriptor[m_aActionId].getMin()
		, pActionDescriptor[m_aActionId].getMax());
	m_pALearner->getPolicy()->getDetPolicyStateVFA()->setCanUseDeferredUpdates(true);
	//KAlpha parameter
	m_pKAlphaLearner = CHILD_OBJECT_FACTORY<CPolicyLearner>(pConfigNode, "KAlpha-adaptive-parameter", "The learner used for Vidal controller's parameter K_alpha");
	m_KAlphaActionId = pWorld->addActionVariable("K_alpha", "unitless", 0.0, 10000000.0);
	m_pKAlphaLearner->getPolicy()->setOutputActionIndex(m_KAlphaActionId);
	m_pKAlphaLearner->getPolicy()->getDetPolicyStateVFA()->setInitValue(m_pK_alpha.get());
	m_pKAlphaLearner->getPolicy()->getDetPolicyStateVFA()->saturateOutput(
		pActionDescriptor[m_KAlphaActionId].getMin()
		, pActionDescriptor[m_KAlphaActionId].getMax());
	m_pKAlphaLearner->getPolicy()->getDetPolicyStateVFA()->setCanUseDeferredUpdates(true);
	//KP parameter
	m_pKPLearner = CHILD_OBJECT_FACTORY<CPolicyLearner>(pConfigNode, "KP-adaptive-parameter", "The learner used for Vidal controller's parameter KP");
	m_KPActionId = pWorld->addActionVariable("KP", "unitless", -100.0, 100.0);
	m_pKPLearner->getPolicy()->setOutputActionIndex(m_KPActionId);
	m_pKPLearner->getPolicy()->getDetPolicyStateVFA()->setInitValue(m_pKP.get());
	m_pKPLearner->getPolicy()->getDetPolicyStateVFA()->saturateOutput(
		pActionDescriptor[m_KPActionId].getMin()
		, pActionDescriptor[m_KPActionId].getMax());
	m_pKPLearner->getPolicy()->getDetPolicyStateVFA()->setCanUseDeferredUpdates(true);
}

CExtendedWindTurbineVidalController::~CExtendedWindTurbineVidalController()
{}

double CExtendedWindTurbineVidalController::selectAction(const CState *s, CAction *a)
{
	//A= f_a(v)
	//K_alpha= f_k_alpha(v)
	//KP= f_kp(v)
	m_pALearner->getPolicy()->selectAction(s, a);
	m_pA.set(a->get(m_pALearner->getPolicy()->getOutputActionIndex()));

	m_pKAlphaLearner->getPolicy()->selectAction(s, a);
	m_pK_alpha.set(a->get(m_pKAlphaLearner->getPolicy()->getOutputActionIndex()));

	m_pKPLearner->getPolicy()->selectAction(s, a);
	m_pKP.set(a->get(m_pKPLearner->getPolicy()->getOutputActionIndex()));

	CWindTurbineVidalController::selectAction(s, a);

	return 1.0;
}
double CExtendedWindTurbineVidalController::update(const CState *s, const CAction *a, const CState *s_p, double r, double behaviorProbability)
{
	double td= m_pCritic->update(s, a, s_p, r);

	m_pALearner->update(s, a, s_p, r, td);
	m_pKAlphaLearner->update(s, a, s_p, r, td);
	m_pKPLearner->update(s, a, s_p, r, td);

	return 1.0;
}


//Extended version of the Boukhezzar controller


CExtendedWindTurbineBoukhezzarController::CExtendedWindTurbineBoukhezzarController(CConfigNode* pConfigNode)
	:CWindTurbineBoukhezzarController(pConfigNode)
{
	CDynamicModel* pWorld = CSimionApp::get()->pWorld->getDynamicModel();
	CDescriptor& pActionDescriptor = pWorld->getActionDescriptor();
	m_pCritic = CHILD_OBJECT_FACTORY<ICritic>(pConfigNode, "Critic", "The critic used to learn");
	//A parameter
	m_pC0Learner = CHILD_OBJECT_FACTORY<CPolicyLearner>(pConfigNode, "C_0-learner", "The learner used for Boukhezzar controller's parameter C0");
	m_C0ActionId = pWorld->addActionVariable("C_0", "unitless", 0.0, 100.0);
	m_pC0Learner->getPolicy()->setOutputActionIndex(m_C0ActionId);
	m_pC0Learner->getPolicy()->getDetPolicyStateVFA()->setInitValue(m_pC_0.get());
	m_pC0Learner->getPolicy()->getDetPolicyStateVFA()->saturateOutput(
		pActionDescriptor[m_C0ActionId].getMin()
		, pActionDescriptor[m_C0ActionId].getMax());
	m_pC0Learner->getPolicy()->getDetPolicyStateVFA()->setCanUseDeferredUpdates(true);
	//KP parameter
	m_pKPLearner = CHILD_OBJECT_FACTORY<CPolicyLearner>(pConfigNode, "KP-learner", "The learner used for Boukhezzar controller's parameter KP");
	m_KPActionId = pWorld->addActionVariable("KP", "unitless", -100.0, 100.0);
	m_pKPLearner->getPolicy()->setOutputActionIndex(m_KPActionId);
	m_pKPLearner->getPolicy()->getDetPolicyStateVFA()->setInitValue(m_pKP.get());
	m_pKPLearner->getPolicy()->getDetPolicyStateVFA()->saturateOutput(
		pActionDescriptor[m_KPActionId].getMin()
		, pActionDescriptor[m_KPActionId].getMax());
	m_pKPLearner->getPolicy()->getDetPolicyStateVFA()->setCanUseDeferredUpdates(true);
}

CExtendedWindTurbineBoukhezzarController::~CExtendedWindTurbineBoukhezzarController()
{}

double CExtendedWindTurbineBoukhezzarController::selectAction(const CState *s, CAction *a)
{
	//A= f_a(v)
	//K_alpha= f_k_alpha(v)
	//KP= f_kp(v)
	m_pC0Learner->getPolicy()->selectAction(s, a);
	m_pC_0.set(a->get(m_pC0Learner->getPolicy()->getOutputActionIndex()));

	m_pKPLearner->getPolicy()->selectAction(s, a);
	m_pKP.set(a->get(m_pKPLearner->getPolicy()->getOutputActionIndex()));

	CWindTurbineBoukhezzarController::selectAction(s, a);

	return 1.0;
}

double CExtendedWindTurbineBoukhezzarController::update(const CState *s, const CAction *a, const CState *s_p, double r, double behaviorProbability)
{
	double td = m_pCritic->update(s, a, s_p, r);

	m_pC0Learner->update(s, a, s_p, r, td);
	m_pKPLearner->update(s, a, s_p, r, td);

	return 1.0;
}


