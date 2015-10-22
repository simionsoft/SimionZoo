// ControllerToActorCritic.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../RLSimion-Lib/parameters.h"
#include "../RLSimion-Lib/world.h"
#include "../RLSimion-Lib/actor.h"
#include "../RLSimion-Lib/reward.h"
#include "../RLSimion-Lib/logger.h"
#include "../RLSimion-Lib/states-and-actions.h"
#include "../RLSimion-Lib/vfa.h"
#include "../RLSimion-Lib/features.h"
#include "../RLSimion-Lib/globals.h"

/*
void SaveControllerAsPolicy(const char *pFilenamePolicy)
{
	int numActions= GetCAction()->getNumDimensions();
	
	CStateModifier *pStateModifier;
	CStateProperties *pStateProperties= g_pModel->getStateProperties();

	std::list<CFeatureCalculator *> *pModifierList;
	char paramName[512];
	char variableList[1024];
	CStateCollectionImpl* pStateInterface= new CStateCollectionImpl(pStateProperties);


	pModifierList = new std::list<CFeatureCalculator *>();
	for (int i= 0; i<numActions; i++)
	{
		sprintf_s(paramName,512,"ACTOR_VARIABLES_DIM_%d",i);


		pStateModifier= GetContinuousStateModifier(g_pModel
						,g_pParameters->getParameter("STATE_MODEL")
						,variableList);

		pModifierList->push_back((CFeatureCalculator*)pStateModifier);
		pStateInterface->addStateModifier(pStateModifier);
	}

	CContinuousActionFeaturePolicy *pPolicy= 
		new CContinuousActionFeaturePolicy(GetCAction(),pStateProperties,pModifierList);
	double *pPolicyWeights= pPolicy->getWeights();

	int numFeatures;
	double minV,maxV,state_value,value;

	CState* pState= pStateInterface->getState(pStateProperties);

	CState* pFeatures;
	CContinuousActionData* pActionData= GetCAction()->getContinuousActionData();

	std::list<CFeatureCalculator *>::iterator it;
	int actionWeightOffset= 0;

	int actionDim= 0;
	g_pModel->doResetModel();
	g_pModel->getState(pState);
	for (it= pModifierList->begin(); it != pModifierList->end(); it++)
	{
		printf("Action dimension: %d\n",actionDim);

		int numActionFeatures= ((CFeatureCalculator*)*it)->getNumFeatures();

		int index,var_index,point_index;
		
		for (int j=0; j<numActionFeatures; j++)
		{
			printf("Feature: %d (/%d)\r",j,numActionFeatures);

			index= j;


			std::list<CStateModifier *> *modifiers=
				((CFeatureOperatorAnd*)*it)->getStateModifiers();

			int i= 0;
			double *pCenters;
			for (std::list<CStateModifier*>::iterator it2= modifiers->begin()
				;it2!=modifiers->end(); it2++)
			//for (int i= 0; i<numVars; i++)
			{
				var_index= ((CSingleStateRBFFeatureCalculator*)*it2)->getDimension();
				//var_index= pStateProperties->getVarIndexForLearning(i);
				numFeatures= ((CFeatureOperatorAnd*)*it)->getModifierNumOfFeatures(i);
				point_index= index % numFeatures;

				minV= pStateProperties->getMinValue(var_index);
				maxV= pStateProperties->getMaxValue(var_index);
				pCenters= ((CSingleStateRBFFeatureCalculator*)*it2)->getCenters();
				state_value= pCenters[point_index];
					//minV + (((double)point_index)/(numFeatures-1))*(maxV-minV);
				pState->setContinuousState(var_index,state_value);

				index= index / numFeatures;
				i++;
			}

	//POLICY
			g_pController->getNextContinuousAction(pState, pActionData);
			
			value = pActionData->getActionValue(actionDim);

			pPolicyWeights[j + actionWeightOffset]= value;

			//pPolicy->getNextContinuousAction(pState, pActionData);
		}

		printf("\n");
		actionDim++;
		actionWeightOffset+= numActionFeatures;
	}
	
//#ifdef __DEBUG
	printf("\n\nSANITY CHECK\n\n");
#define NUM_STATE_VALUES 14
#define NUM_TEST_VALUES 2
	double testValues[NUM_TEST_VALUES][NUM_STATE_VALUES];
	//testValues[0]= {600000.00000 599950.26176 477682.85966 20.26000 109008.74590 4.38187 
	//-0.07565 0.46509 -0.01544 136916.16422 2243.51745 -49.73824 -0.01617 
	//2.07640 -0.01544 2243.51745} //log boukhezzar-0-1.txt

	testValues[0][0]= 600000.0; testValues[0][1]= 599950.26176; testValues[0][2]= 477682.85966;
	testValues[0][3]= 20.26000; testValues[0][4]= 109008.74590; testValues[0][5]= 4.38187;
	testValues[0][6]= -0.07565; testValues[0][7]= 0.46509; testValues[0][8]= -0.01544;
	testValues[0][9]= 136916.16422; testValues[0][10]= 2243.51745; testValues[0][11]= -49.73824;
	testValues[0][12]= -0.01617; testValues[0][13]= 2.07640;
	//testValues[1]= {600000.00000 599999.75976 613983.00722 20.70000 135606.27009 4.52770
	//0.00326 0.45347 0.13851 132513.31396 -1828.37350 -0.24024 0.12946 0.09301
	//0.13851 -1828.37350} //log vidal-0-1.txt
	testValues[1][0]= 600000.0; testValues[1][1]= 599999.75976; testValues[1][2]= 613983.00722;
	testValues[1][3]= 20.70000; testValues[1][4]= 135606.27009; testValues[1][5]= 4.52770;
	testValues[1][6]= 0.00326; testValues[1][7]= 0.45347; testValues[1][8]= 0.13851;
	testValues[1][9]= 132513.31396; testValues[1][10]= -1828.37350; testValues[1][11]= -0.24024;
	testValues[1][12]= 0.12946; testValues[1][13]= 0.09301;

	actionWeightOffset= 0;



	for (int i= 0; i<NUM_TEST_VALUES; i++)
	{

		g_pModel->doResetModel();
		g_pModel->getState(pState);

		for (int j=0; j<NUM_STATE_VALUES; j++)
			pState->setContinuousState(j,testValues[i][j]);


		((CContinuousActionController*)g_pController)->getNextContinuousAction(pState, pActionData);

		printf("Controller outputs: %.4f, %.4f\n", pActionData->getActionValue(0), pActionData->getActionValue(1));

		//pPolicy->getNextContinuousAction(pState,pActionData);
		printf("Actor outputs: ");//%.4f, %.4f\n", pActionData->getActionValue(0), pActionData->getActionValue(1));
		int dim= 0;
		actionWeightOffset= 0;
		for (it= pModifierList->begin(); it != pModifierList->end(); it++)
		{	
			pFeatures= pStateInterface->getState((CFeatureCalculator*)(*it));
			(*it)->getModifiedState(pStateInterface,pFeatures);

			int feature_index;
			double factor,value;
			value= 0.0;
			printf("DIMENSION: %d\n",dim);
			printf("Features(Factor)=value:\n");
			for (int feature= 0; min(4,feature<pFeatures->getNumActiveDiscreteStates()); feature++)
			{
				feature_index= pFeatures->getDiscreteState(feature);
				factor= pFeatures->getContinuousState(feature);
				printf("%d(%.4f)=%.5f\n",feature_index,factor,pPolicyWeights[feature_index + actionWeightOffset]);
				value+= factor* pPolicyWeights[feature_index + actionWeightOffset];

			}
			printf("\nValue: %f\n", value);
			actionWeightOffset+= ((CFeatureCalculator*)*it)->getNumFeatures();
			dim++;
		}
		printf("\n");
	
	}

//#endif

	FILE *pPolicyFile,*pValueFile;
	fopen_s(&pPolicyFile,pFilenamePolicy,"wb");
	if (pPolicyFile)
	{
		pPolicy->saveData(pPolicyFile);
		fclose(pPolicyFile);
	}
	
	delete pPolicy;
}*/
CParameters *g_pParameters;
CWorld *g_pWorld;
CExperiment *g_pExperiment;

int _tmain(int argc, _TCHAR* argv[])
{

	CFeatureVFA **m_pVFA;
	CActor *pController;

	g_pParameters= new CParameters("ControllerToAC-parameters.txt");
	
	//INITIALISE WORLD -> STATE PROPERTIES
	g_pWorld= new CWorld(g_pParameters->getStringPtr("WORLD_CONFIG_FILE"));
	CParameters *pFeatureVFAParameters= new CParameters(g_pParameters->getStringPtr("ACTOR_CONFIG_FILE"));

	//INTIALISE CONTROLLER: VIDAL, BOUKHEZZAR, ...
	pController= CActor::getInstance(g_pParameters->getStringPtr("CONTROLLER_CONFIG_FILE"));

	char parameterName[MAX_PARAMETER_NAME_SIZE];

	int numOutputs= (int) pFeatureVFAParameters->getDouble("NUM_OUTPUTS");

	//INTIALISE VFA
	m_pVFA= new CFeatureVFA* [numOutputs];

	for (int i= 0; i<numOutputs; i++)
	{
		sprintf_s(parameterName,MAX_PARAMETER_NAME_SIZE,"POLICY_RBF_VARIABLES_%d",i);
		m_pVFA[i]= new CRBFFeatureGridVFA(pFeatureVFAParameters->getStringPtr(parameterName));
	}

	CState *s= g_pWorld->getStateInstance();
	CAction *a= g_pWorld->getActionInstance();


	//CONTROLLER -> ACTOR
	printf("\nSaving the weights of a VFA that approximates the controller...\n");
	FILE* pFile;
	unsigned int numWeights;
	unsigned int feature;
	double output;
	fopen_s(&pFile,g_pParameters->getStringPtr("OUTPUT_FILE"),"wb");
	if (pFile)
	{
		for (int i= 0; i<numOutputs; i++)
		{
			numWeights= m_pVFA[i]->getNumWeights();
			fwrite(&numWeights,sizeof(unsigned int),1,pFile);
			
			for (feature= 0; feature<numWeights; feature++)
			{
				printf("Output dim: %d/%d      Progress: %2.2f%%\r",i+1,numOutputs,100.0*((double)feature)/((double)numWeights));
				m_pVFA[i]->getFeatureStateAction(feature,s,a);
				pController->selectAction(s,a);
				output= a->getValue(i);
				fwrite(&output,sizeof(double),1,pFile);
			}
		}
		printf("\nDone\n");
		fclose(pFile);
	}
	else printf("ERROR: could not open output file\n");

	//CLEAN-UP
	delete a;
	delete s;
	delete g_pWorld;
	delete pController;

	for (int i= 0; i<numOutputs; i++) delete m_pVFA [i];
	delete [] m_pVFA;

	delete g_pParameters;
	delete pFeatureVFAParameters;
}

