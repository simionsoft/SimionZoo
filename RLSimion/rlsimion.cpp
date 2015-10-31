// RLSimion-Lib-test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../RLSimion-Lib/parameters.h"
#include "../RLSimion-Lib/world.h"
#include "../RLSimion-Lib/simgod.h"
#include "../RLSimion-Lib/reward.h"
#include "../RLSimion-Lib/states-and-actions.h"
#include "../RLSimion-Lib/parameterscheduler.h"
#include "../RLSimion-Lib/experiment.h"

#ifdef _DEBUG
	#pragma comment (lib,"../Debug/RLSimion-Lib.lib")
#else
	#pragma comment (lib,"../Release/RLSimion-Lib.lib")
#endif

#include <stdlib.h>
#include <stdio.h>

CWorld* g_pWorld;
CParameterScheduler *g_pParameterScheduler;
CExperiment *g_pExperiment;

int main(int argc, char* argv[])
{
	CParameters *pParameters;
	if (argc > 1)
		pParameters = new CParameters(argv[1]);
	else
	{
		printf("ERROR: configuration file not provided as an argument");
		exit(-1);
	}
	printf("\n\n******************\nRLSimion\n******************\nConfig. file %s******************\n\n", argv[1]);

	g_pWorld= new CWorld(pParameters);
	CSimGod* pSimGod = new CSimGod(pParameters);
	g_pExperiment= new CExperiment(pParameters);

	g_pParameterScheduler= new CParameterScheduler(pParameters->getStringPtr("PARAMETER_SCHEDULER_CONFIG_FILE"), pParameters);


	CState *s= g_pWorld->getStateInstance();
	CState *s_p= g_pWorld->getStateInstance();
	CAction *a= g_pWorld->getActionInstance();

	double r= 0.0;
	double td= 0.0;

	//episodes
	for (g_pExperiment->m_episode= 1; g_pExperiment->m_episode<=g_pExperiment->m_numEpisodes; g_pExperiment->m_episode++)
	{
		g_pWorld->reset(s);

		//steps per episode
		for (g_pExperiment->m_step= 1; g_pExperiment->m_step<=g_pExperiment->m_numSteps; g_pExperiment->m_step++)
		{

			//Update parameters that have a schedule: learning gains, exploration, ...
			if (g_pParameterScheduler)
				g_pParameterScheduler->update();

			//a= pi(s)
			pSimGod->selectAction(s,a);

			//s_p= f(s,a); r= R(s');
			r= g_pWorld->executeAction(s,a,s_p);

			//update god's policy and value estimation
			pSimGod->update(s, a, s_p, r);

			//log tuple <s,a,s',r>
			g_pExperiment->logStep(s,a,s_p,r);

			//s= s'
			s->copy(s_p);
		}
	}

	delete pSimGod;


	delete s;
	delete s_p;
	delete a;

	delete g_pParameterScheduler;
	delete pParameters;
	delete g_pWorld;
	delete g_pExperiment;

	return 0;
}