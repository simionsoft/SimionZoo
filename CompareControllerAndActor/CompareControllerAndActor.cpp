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




class CStats
{
	double min,max,total, sqrtotal;
	int numValues;
public:
	CStats(){ min = 99999999.9; max = 0.0; total = 0.0; numValues = 0; sqrtotal = 0.0; }
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
		sqrtotal += value*value;
	}
	double getMax(){return max;}
	double getMin(){return min;}
	double getAvg(){return total/(double)numValues;}
	double getRMSE(){ return sqrt(sqrtotal / (double)numValues); }
};


CWorld *g_pWorld;
CExperiment *g_pExperiment = 0;

int main(int argc, char* argv[])
{
	if (argc<2)
	{
		printf("ERROR: a configuration file should be provided as an argument.");
		exit(-1);
	}

	CParameters* pAppParameters= new CParameters(argv[1]);
	
	//INITIALISE WORLD -> STATE PROPERTIES
	CParameters* pActorParameters = new CParameters(pAppParameters->getStringPtr("ACTOR_CONFIG_FILE"));
	//INITIALISE ACTOR

	g_pWorld = new CWorld(pActorParameters);
	CActor *pActor = CActor::getActorInstance(pActorParameters);

	//INTIALISE CONTROLLER: VIDAL, BOUKHEZZAR, ...
	CParameters* pControllerParameters = new CParameters(pAppParameters->getStringPtr("CONTROLLER_CONFIG_FILE"));
	CActor *pController = CActor::getActorInstance(pControllerParameters);

	CState *s= g_pWorld->getStateInstance();
	CAction *a= g_pWorld->getActionInstance();
	CAction *a2= g_pWorld->getActionInstance();

	int numSamples = (int) pAppParameters->getDouble("COMPARE_AC_AND_ACTOR/NUM_SAMPLES");
	
//#define NUM_SAMPLES 100000
#define NUM_POINTS_PER_DIM 100000
	double value;
	CStats* pStats= new CStats[a->getNumVars()];

	
	for (int i= 0; i<numSamples; i++)
	{
		printf("Sampling controller and actor: %d/%d samples\r",i,numSamples);
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

	FILE *pFile;
	fopen_s(&pFile, pAppParameters->getStringPtr("COMPARE_AC_AND_ACTOR/OUTPUT_FILE"), "w");
	if (pFile)
	{
		for (int j = 0; j < a->getNumVars(); j++)
		{
			fprintf_s(pFile,"Comparison stats for dim %d: Max=%.5f Min=%.5f Avg=%.5f\n"
				, j, pStats[j].getMax(), pStats[j].getMin(), pStats[j].getAvg(), pStats[j].getRMSE());
		}
		fclose(pFile);
	}

	delete [] pStats;


	//CLEAN-UP
	delete a;
	delete s;
	delete g_pWorld;
	delete pController;
	delete pActor;
	delete pAppParameters;
	delete pActorParameters;
	delete pControllerParameters;
}

