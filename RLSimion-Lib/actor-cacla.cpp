#include "stdafx.h"
#include "actor.h"
#include "vfa.h"
#include "globals.h"
#include "states-and-actions.h"
#include "features.h"
#include "experiment.h"
#include "parameter.h"

CCACLAActor::CCACLAActor(CParameters *pParameters) : CVFAGaussianNoiseActor(pParameters)
{

}

CCACLAActor::~CCACLAActor()
{

}

void CCACLAActor::updatePolicy(CState *s,CAction *a,CState *s_p,double r,double td)
{
	double lastNoise;
	/*double a_width;*/

	//CACLA (van Hasselt)
	//if delta>0: theta= theta + alpha*(lastNoise)*phi_pi(s)

	if (td>0 && !g_pExperiment->isEvaluationEpisode())
	{
		//MAYBE WE CAN CHECK WHETHER WE HAVE ALREADY CALCULATED THE OUTPUT OF THE CONTROLLER
		selectAction(s, m_pOutput);

		for (int i= 0; i<m_numOutputs; i++)
		{
			lastNoise = a->getValue(i) - m_pOutput->getValue(i);

			m_pPolicy[i]->getFeatures(s,0,m_pStateFeatures);

			if (m_pAlpha[i]->getDouble() != 0.0)
				m_pPolicy[i]->add(m_pStateFeatures,m_pAlpha[i]->getDouble()*lastNoise);
		}
	}
}

