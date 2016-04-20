#include "stdafx.h"
#include "app-compare-controller-vfa.h"

#include "../RLSimion-Lib/globals.h"
#include "../RLSimion-Lib/parameters.h"
#include "../RLSimion-Lib//world.h"
#include "../RLSimion-Lib/controller.h"
#include "../RLSimion-Lib/named-var-set.h"
#include "../RLSimion-Lib/vfa.h"
#include "../RLSimion-Lib/utils.h"
#include "../RLSimion-Lib/logger.h"
#include "../RLSimion-Lib/simgod.h"

class CSimpleStats
{
	double min, max, total, sqrtotal;
	int numValues;
public:
	CSimpleStats(){ reset(); }
	~CSimpleStats(){}
	void addValue(double value)
	{
		value = fabs(value);
		numValues++;
		if (value>max)
			max = value;
		if (value<min)
			min = value;
		total += value;
		sqrtotal += value*value;
	}
	double getMax(){ return max; }
	double getMin(){ return min; }
	double getAvg(){ return total / (double)numValues; }
	double getRMSE(){ return sqrt(sqrtotal / (double)numValues); }
	void reset(){ min = 99999999.9; max = 0.0; total = 0.0; numValues = 0; sqrtotal = 0.0; }
};

void CompareControllerVFAApp::getInputFiles(CFilePathList& filepathList)
{
	SimGod.getInputFiles(filepathList);
}

void CompareControllerVFAApp::getOutputFiles(CFilePathList& filepathList)
{
	SimGod.getOutputFiles(filepathList);
}

APP_CLASS (CompareControllerVFAApp)
{
	CParameters* pParameters = m_pConfigDoc->loadFile(argv[1], "CompareControllerVFA");
	if (!pParameters) throw std::exception("Wrong experiment configuration file");
	pParameters = pParameters->getChild("CompareControllerVFA");
	if (!pParameters) throw std::exception("Wrong experiment configuration file");

	//First the world
	CDynamicModel *pWorld;
	CHILD_CLASS_FACTORY(pWorld, "Dynamic-Model", "The dynamic model's definition", false, CDynamicModel);
	//This short-cut avoids having to define all the rest of parameters of the world not used in this app (setpoints, ...)
	World.setDynamicModel(pWorld);

	//The controller
	CHILD_CLASS_FACTORY(m_pController, "Controller", "The controller to be approximated", false, CController);

	//The VFA policy
	m_numVFAs = pParameters->countChildren("Policy");
	CParameters* pPolicyParameters = pParameters->getChild("Policy");
	m_pVFAs = new CLinearStateVFA*[m_numVFAs];
	for (int i = 0; i < m_numVFAs; i++)
	{
		MULTI_VALUED(m_pVFAs[i], "Policy", "The VFAs used to approximate the controller", CLinearStateVFAFromFile, pPolicyParameters);
		pPolicyParameters = pPolicyParameters->getNextChild("Policy");
	}
	CHILD_CLASS(m_pOutputDirFile, "Output-DirFile", "The output directory and file", false, CDirFileOutput);

	sprintf_s(m_outputFilePath, 1024, "%s/%s.txt", m_pOutputDirFile->getOutputDir(), m_pOutputDirFile->getFilePrefix());
	SimGod.registerOutputFile(m_outputFilePath);

	CONST_INTEGER_VALUE(m_numSamples, "Num-Samples", 100000, "The number of samples taken to compare the controller and the approximation");
	END_CLASS();
}

void CompareControllerVFAApp::run()
{
	CState *s = World.getDynamicModel()->getStateDescriptor()->getInstance();
	CAction *a = World.getDynamicModel()->getActionDescriptor()->getInstance();
	CAction *a2 = World.getDynamicModel()->getActionDescriptor()->getInstance();

#define NUM_POINTS_PER_DIM 100000
	double value;
	CSimpleStats* pStats = new CSimpleStats[a->getNumVars()];

	srand(1);
	double progress;
	int numOutputs;
	char msg[1024];
	for (int i = 0; i<m_numSamples; i++)
	{
		printf("Sampling controller and actor: %d/%d samples\r", i, m_numSamples);
		for (int j = 0; j<s->getNumVars(); j++)
		{
			value = (double)(rand() % NUM_POINTS_PER_DIM) / (double)NUM_POINTS_PER_DIM;
			value = s->getMin(j) + value*(s->getMax(j) - s->getMin(j));
			s->setValue(j, value);
		}

		m_pController->selectAction(s, a);
		numOutputs = std::min(m_pController->getNumOutputs(), m_numVFAs);
		for (int j = 0; j < numOutputs; j++)
		{
			m_pVFAs[j]->getValue(s);

			pStats[j].addValue(a->getValue(m_pController->getOutputActionIndex(j)) - m_pVFAs[j]->getValue(s));
		}
		progress = (((double)i) / numOutputs) + (1.0 / numOutputs) * ((double)i) / ((double)m_numSamples);
		progress *= 100.0;
		sprintf_s(msg, 512, "%f", progress);
		CLogger::logMessage(MessageType::Progress, msg);
	}


	char buffer[4000];

	FILE *pFile;
	fopen_s(&pFile,m_outputFilePath , "w");
	if (pFile)
	{
		for (int j = 0; j < a->getNumVars(); j++)
		{
			sprintf_s(buffer,4000, "Comparison stats for dim %d: Max=%.5e Min=%.5e Avg=%.5e RMSE= %.5e\n"
				, j, pStats[j].getMax(), pStats[j].getMin()
				, pStats[j].getAvg(), pStats[j].getRMSE());
			fprintf_s(pFile, "%s", buffer);
			CLogger::logMessage(MessageType::Info, buffer);
		}
		fclose(pFile);
	}
	CLogger::logMessage(MessageType::Progress, "100");

	delete[] pStats;


	//CLEAN-UP
	delete a;
	delete s;
}


CompareControllerVFAApp::~CompareControllerVFAApp()
{
	delete m_pController;
	delete m_pOutputDirFile;

	for (int i = 0; i < m_numVFAs; i++)
		if (m_pVFAs[i]) delete m_pVFAs[i];

	delete[] m_pVFAs;
}