#include "stdafx.h"
#include "named-var-set.h"
#include "noise.h"
#include "policy-learner.h"
#include "vfa.h"
#include "policy.h"
#include "actor.h"
#include "config.h"
#include "logger.h"


CPolicyLearner::CPolicyLearner(CConfigNode* pConfigNode)
{
	m_pPolicy= CHILD_OBJECT_FACTORY<CPolicy>(pConfigNode, "Policy", "The policy to be learned");
}

CPolicyLearner::~CPolicyLearner()
{
}

std::shared_ptr<CPolicyLearner> CPolicyLearner::getInstance(CConfigNode* pConfigNode)
{
	return CHOICE<CPolicyLearner>(pConfigNode,"Policy-Learner", "The algorithm used to learn the policy",
	{
		{"CACLA",CHOICE_ELEMENT_NEW<CCACLALearner>},
		{"Regular-Gradient",CHOICE_ELEMENT_NEW<CRegularPolicyGradientLearner>}
	});
}




/*

//doesn't work, not sure if it should either

double CVFAActor::getProbability(CState* s, CAction* a)
{
	double actionProb = 1.0;
	double actionDist = 0.0;
	double mahalanobisDist = 0.0;
	double varProd = 1.0;
	double noiseWidth;
	double var_i; //action's i-th dimension's variance
	double output;

	if (CSimionApp::getSample()->pExperiment->isEvaluationEpisode())
		return 1.0;
	//http://en.wikipedia.org/wiki/Multivariate_normal_distribution

	//if there is no correlation:
	//f(x,mu)= exp(-0.5 * (x-mu)^T * (x-mu)/var(x))/ sqrt(2*pi^k* prod var(x))

	for (int i = 0; i<m_numOutputs; i++)
	{
		output = m_pPolicyLearners[i]->getPolicy()->getVFA()->getSample(s, a);

		var_i = std::max(0.000000001, m_pPolicyLearners[i]->getPolicy()->getExpNoise()->getSigma());

		noiseWidth = fabs((a->getSample(i) - output) 
			/ a->getRangeHalfWidth(m_pPolicyLearners[i]->getPolicy()->getOutputActionIndex()));

		if (noiseWidth != 0.0)
		{
			//actionDist= (m_pExpNoise[i]->getLastValue()/noiseWidth)*3.0; //because we are using sigma=1 /3 to map values into [-1,1]
			mahalanobisDist += noiseWidth*noiseWidth / (var_i*var_i);// Variance=1.0 , otherwise it should be  / (var_i*var_i);
			varProd = varProd* var_i*var_i;
		}
	}

	if (mahalanobisDist == 0.0) return 1.0;

	double root = sqrt(pow((2 * M_PI), (double)m_numOutputs)*varProd);
	double expo = exp(-0.5*mahalanobisDist);

	return std::min(1.0, expo / root);
}
*/
