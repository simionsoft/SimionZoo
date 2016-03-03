// RLSimion-Lib-test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "../RLSimion-Lib/world.h"
#include "../RLSimion-Lib/simgod.h"
#include "../RLSimion-Lib/reward.h"
#include "../RLSimion-Lib/named-var-set.h"
#include "../RLSimion-Lib/globals.h"
#include "../RLSimion-Lib/experiment.h"
#include "../RLSimion-Lib/xml-parameters.h"
#include "../RLSimion-Lib/logger.h"

int main(int argc, char* argv[])
{
	tinyxml2::XMLDocument xmlDoc;
	tinyxml2::XMLElement* pParameters;

	
	//CParameters *pParameters;
	if (argc > 1)
		xmlDoc.LoadFile(argv[1]);
		//pParameters = new CParameters(argv[1]);
	else
	{
		printf("ERROR: configuration file not provided as an argument");
		exit(-1);
	}
	printf("\n\n******************\nRLSimion\n******************\nConfig. file %s\n******************\n\n", argv[1]);

	pParameters = xmlDoc.FirstChildElement("RLSimion");
	if (xmlDoc.Error())
	{
		printf("Error loading configuration file: %s\n\n", xmlDoc.ErrorName());
		exit(-1);
	}

	RLSimion::init(pParameters);

	//CParameterScheduler* pParameterScheduler= new CParameterScheduler(pParameters->getChild("PARAMETER_SCHEDULER"));

	CState *s= RLSimion::g_pWorld->getStateInstance();
	CState *s_p= RLSimion::g_pWorld->getStateInstance();
	CAction *a= RLSimion::g_pWorld->getActionInstance();

	double r= 0.0;

	//episodes
	for (RLSimion::g_pExperiment->m_expProgress.setEpisode(1); RLSimion::g_pExperiment->m_expProgress.isValidEpisode(); RLSimion::g_pExperiment->m_expProgress.incEpisode())
	{
		RLSimion::g_pWorld->reset(s);

		//steps per episode
		for (RLSimion::g_pExperiment->m_expProgress.setStep(1); RLSimion::g_pExperiment->m_expProgress.isValidStep(); RLSimion::g_pExperiment->m_expProgress.incStep())
		{
			//a= pi(s)
			RLSimion::g_pSimGod->selectAction(s,a);

			//s_p= f(s,a); r= R(s');
			r= RLSimion::g_pWorld->executeAction(s,a,s_p);

			//update god's policy and value estimation
			RLSimion::g_pSimGod->update(s, a, s_p, r);

			//log tuple <s,a,s',r>
			RLSimion::g_pExperiment->timestep(s,a,s_p,RLSimion::g_pWorld->getReward()); //we need the complete reward vector for logging

			//s= s'
			s->copy(s_p);
		}
	}

	//pending task: rewrite using smart pointers
	RLSimion::shutdown();

	XMLUtils::freeHandlers();

	delete s;
	delete s_p;
	delete a;

	return 0;
}