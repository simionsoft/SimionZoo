#include "stdafx.h"
#include "actor.h"
#include "noise.h"
#include "controller.h"
#include "actor.h"
#include "policy.h"
#include "globals.h"
#include "named-var-set.h"
#include "parameters.h"
#include "policy-learner.h"

#include "vfa.h"


CLASS_CONSTRUCTOR(CActor) : CParamObject(pParameters)
{
	m_numOutputs = pParameters->countChildren("Output");
	CParameters* pOutput = pParameters->getChild("Output");

	m_pPolicyLearners = new CPolicyLearner*[m_numOutputs];

	for (int i = 0; i<m_numOutputs; i++)
	{
		MULTI_VALUED_FACTORY(m_pPolicyLearners[i], "Output", "The outputs of the actor. One for each output dimension",CPolicyLearner, pOutput);

		pOutput = pOutput->getNextChild();
	}
	END_CLASS();
}



CActor::~CActor()
{

	for (int i = 0; i < m_numOutputs; i++)
	{
		delete m_pPolicyLearners[i];
	}

	delete[] m_pPolicyLearners;
}



void CActor::selectAction(const CState *s, CAction *a)
{
	for (int i = 0; i<m_numOutputs; i++)
	{
		//each uni-dimensional policy sets its own action's value
		m_pPolicyLearners[i]->getPolicy()->selectAction(s, a);
	}
}

void CActor::updatePolicy(const CState* s, const CAction* a, const CState* s_p, double r, double td)
{
	for (int i = 0; i<m_numOutputs; i++)
	{
		//each uni-dimensional policy sets its own action's value
		m_pPolicyLearners[i]->updatePolicy(s, a, s_p, r, td);
	}
}