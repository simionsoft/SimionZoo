// ControllerToActorCritic.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../RLSimion-Lib/parameters.h"
#include "../RLSimion-Lib/world.h"
#include "../RLSimion-Lib/actor.h"
#include "../RLSimion-Lib/reward.h"
#include "../RLSimion-Lib/logger.h"
#include "../RLSimion-Lib/experiment.h"
#include "../RLSimion-Lib/states-and-actions.h"
#include "../RLSimion-Lib/vfa.h"


CParameters *g_pParameters;
CWorld *g_pWorld;
CExperiment *g_pExperiment= 0;

class CStats
{
	double min,max,total;
	int numValues;
public:
	CStats(){min= 99999999.9; max= 0.0; total= 0.0;numValues= 0;}
	~CStats(){}
	void addValue(double value)
	{
		value= fabs(value);
		numValues++;
		if (value>max)
			max= value;
		if (value<min)
			min= value;
		total+= value;
	}
	double getMax(){return max;}
	double getMin(){return min;}
	double getAvg(){return total/(double)numValues;}
};

int _tmain(int argc, _TCHAR* argv[])
{
	g_pParameters= new CParameters("CompareControllerAndActor-parameters.txt");
	
	//INITIALISE WORLD -> STATE PROPERTIES
	g_pWorld= new CWorld(g_pParameters->getStringPtr("WORLD_CONFIG_FILE"));

	//INITIALISE ACTOR
	CActor *pActor= CActor::getActorInstance(g_pParameters->getStringPtr("ACTOR_CONFIG_FILE"));

	//INTIALISE CONTROLLER: VIDAL, BOUKHEZZAR, ...
	CActor *pController= CActor::getActorInstance(g_pParameters->getStringPtr("CONTROLLER_CONFIG_FILE"));

	CState *s= g_pWorld->getStateInstance();
	CAction *a= g_pWorld->getActionInstance();
	CAction *a2= g_pWorld->getActionInstance();
#define NUM_SAMPLES 100000
#define NUM_POINTS_PER_DIM 100000
	double value;
	CStats* pStats= new CStats[a->getNumVars()];

	//((CCACLAActor*)pActor)->m_pPolicy[1]->getFeatureStateAction(10187705,s,a);
	//((CCACLAActor*)pActor)->m_pPolicy[1]->getFeatureStateAction(10187706,s,a);
	//((CCACLAActor*)pActor)->m_pPolicy[1]->getFeatureStateAction(10187631,s,a);
	//((CCACLAActor*)pActor)->m_pPolicy[1]->getFeatureStateAction(9765830,s,a);
	//((CCACLAActor*)pActor)->m_pPolicy[1]->getFeatureStateAction(10182080,s,a);
	//((CCACLAActor*)pActor)->m_pPolicy[1]->getFeatureStateAction(10187630,s,a);
	//((CCACLAActor*)pActor)->m_pPolicy[1]->getFeatureStateAction(9765831,s,a);
	//((CCACLAActor*)pActor)->m_pPolicy[1]->getFeatureStateAction(10182081,s,a);
	//((CCACLAActor*)pActor)->m_pPolicy[1]->getFeatureStateAction(9765756,s,a);
	
	for (int i= 0; i<NUM_SAMPLES; i++)
	{
		printf("Sampling controller and actor: %d/%d samples\r",i,NUM_SAMPLES);
		for (int j= 0; j<s->getNumVars(); j++)
		{
			value= (double)(rand()%NUM_POINTS_PER_DIM)/ (double)NUM_POINTS_PER_DIM;
			value= s->getMin(j) + value*(s->getMax(j)-s->getMin(j));
			s->setValue(j,value);
		}
		pController->selectAction(s,a);
		pActor->selectAction(s,a2);

		for (int j= 0; j<a->getNumVars(); j++)
		{
			pStats[j].addValue(a->getValue(j) - a2->getValue(j));
		}
	}
	for (int j= 0; j<a->getNumVars(); j++)
	{
		printf("Comparison stats for dim %d: Max=%.5f Min=%.5f Avg=%.5f\n"
			,j, pStats[j].getMax(),pStats[j].getMin(),pStats[j].getAvg());
	}
	printf("Press ENTER...");
	char c= getchar();

	delete [] pStats;


	//CLEAN-UP
	delete a;
	delete s;
	delete g_pWorld;
	delete pController;
	delete pActor;

	delete g_pParameters;
}

