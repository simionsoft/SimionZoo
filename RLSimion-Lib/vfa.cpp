#include "stdafx.h"
#include "vfa.h"
#include "featuremap.h"
#include "features.h"
#include "parameters.h"
#include "globals.h"
#include "logger.h"


//LINEAR VFA. Common functionalities: getValue (CFeatureList*), saturate, save, load, ....
CLinearVFA::CLinearVFA()
{
	m_pWeights = 0;

}
CLinearVFA::~CLinearVFA()
{
	if (m_pWeights) delete[] m_pWeights;
}

double CLinearVFA::getValue(const CFeatureList *pFeatures)
{
	double value = 0.0;
	assert(pFeatures);
	for (unsigned int i = 0; i<pFeatures->m_numFeatures; i++)
	{
		assert(pFeatures->m_pFeatures[i].m_index<m_numWeights);
		assert(pFeatures->m_pFeatures[i].m_factor >= 0.0 && pFeatures->m_pFeatures[i].m_factor <= 1.0);

		value += m_pWeights[pFeatures->m_pFeatures[i].m_index] * pFeatures->m_pFeatures[i].m_factor;
	}
	return value;
}

void CLinearVFA::saturateOutput(double min, double max)
{
	m_bSaturateOutput = true;
	m_minOutput = min;
	m_maxOutput = max;
}

bool CLinearVFA::saveWeights(const char* pFilename)
{
	FILE* pFile;
	assert(m_pWeights && m_numWeights >= 0);

	fopen_s(&pFile, pFilename, "wb");
	if (pFile)
	{
		fwrite(&m_numWeights, sizeof(unsigned int), 1, (FILE*)pFile);
		fwrite(m_pWeights, sizeof(double), m_numWeights, (FILE*)pFile);
		fclose(pFile);
		return true;
	}
	return false;
}

bool CLinearVFA::loadWeights(const char* pFilename)
{
	FILE* pFile;
	unsigned int numWeightsRead;

	fopen_s(&pFile, pFilename, "rb");
	if (pFile)
	{
		fread_s(&numWeightsRead, sizeof(unsigned int), sizeof(unsigned int), 1, (FILE*)pFile);

		assert(m_numWeights == numWeightsRead);

		fread_s(m_pWeights, numWeightsRead*sizeof(double), sizeof(double), numWeightsRead, (FILE*)pFile);

		fclose(pFile);
		return true;
	}
	return false;
}
//
//void CLinearStateVFA::load(const char* pFilename)
//{
//	CParameters* pFeatureMapParameters;
//	char msg[128];
//	char binFile[512];
//	char xmlDescFile[512];
//	FILE* pXMLFile;
//	CParameterFile parameterFile;
//	CParameters* pReadParameters;
//
//	if (pFilename == 0 || pFilename[0] == 0) return;
//
//	sprintf_s(msg, 128, "Loading Policy (\"%s\" (.bin/.xml)...", pFilename);
//	CLogger::logMessage(Info, msg);
//
//	sprintf_s(xmlDescFile, 512, "%s.feature-map.xml", pFilename);
//
//	pReadParameters = parameterFile.loadFile(xmlDescFile);
//	pFeatureMapParameters = m_pStateFeatureMap->setParameters(pReadParameters);
//
//		//if (pFeatureMapParameters)
//		//	pFeatureMapParameters->saveFile(pXMLFile);
//
//		//fclose(pXMLFile);
//		//CLogger::logMessage(Info, "OK\n");
//	//	return;
//	//}
//
//	sprintf_s(binFile, 512, "%s", pFilename);
//	if (loadWeights(binFile))
//		sprintf_s(msg, 128, "OK", binFile);
//	else
//		sprintf_s(msg, 128, "FAILED", binFile);
//	CLogger::logMessage(Warning, msg);
//}
//
void CLinearStateVFA::save(const char* pFilename)
{
	CParameters* pFeatureMapParameters;
	char msg[128];
	char binFile[512];
	char xmlDescFile[512];
	FILE* pXMLFile;

	if ( pFilename == 0 || pFilename[0] == 0) return;

	sprintf_s(msg, 128, "Saving Policy (\"%s\" (.bin/.xml)...", pFilename);
	CLogger::logMessage(Info, msg);

	sprintf_s(binFile, 512, "%s.weights.bin", pFilename);
	saveWeights(binFile);

	sprintf_s(xmlDescFile, 512, "%s.feature-map.xml", pFilename);

	fopen_s(&pXMLFile, xmlDescFile, "w");
	if (pXMLFile)
	{
		pFeatureMapParameters= m_pStateFeatureMap->getParameters();

		if (pFeatureMapParameters)
			pFeatureMapParameters->saveFile(pXMLFile);

		fclose(pXMLFile);
		CLogger::logMessage(Info, "OK\n");
		return;
	}

	sprintf_s(msg, 128, "FAILED", binFile);
	CLogger::logMessage(Warning, msg);
}



//STATE VFA: V(s), pi(s), .../////////////////////////////////////////////////////////////////////

CLASS_CONSTRUCTOR(CLinearStateVFA) : CParamObject(pParameters)
{
	CHILD_CLASS_FACTORY(m_pStateFeatureMap,"State-Feature-Map","The feature map fuction (state->features)","",CFeatureMap);

	m_numWeights = m_pStateFeatureMap->getTotalNumFeatures();
	m_pWeights = new double[m_numWeights];

	m_pAux = new CFeatureList("LinearStateVFA/aux");

	double initValue;
	CONST_DOUBLE_VALUE(initValue, "Init-Value", 0.0,"The initial value given to the VFA's weights on initialization");
	for (unsigned int i = 0; i < m_numWeights; i++)
		m_pWeights[i] = initValue;


	m_bSaturateOutput = false;
	m_minOutput = 0.0;
	m_maxOutput = 0.0;
	END_CLASS();
}
CLinearStateVFA::CLinearStateVFA() : CParamObject(0)
{}

CLASS_CONSTRUCTOR(CLinearStateVFAFromFile) :CLinearStateVFA()
{
	FILE_PATH_VALUE(m_loadFilename, "Load", "../config/data", "The VFA will be loaded from this file");
	CParameterFile mapFeatureParameterFile;
	CParameters* mapFeatureParameters= mapFeatureParameterFile.loadFile(m_loadFilename);
	m_pStateFeatureMap = CFeatureMap::getInstance(mapFeatureParameters);

	m_numWeights = m_pStateFeatureMap->getTotalNumFeatures();
	m_pWeights = new double[m_numWeights];
	loadWeights(m_loadFilename);

	m_bSaturateOutput = false;
	m_minOutput = 0.0;
	m_maxOutput = 0.0;
	END_CLASS();
}

CLASS_FACTORY(CLinearStateVFA)
{
	CHOICE("Parameterization", "Do we want to define the vfa explicitly or load it from file?");
	CHOICE_ELEMENT("Explicit", CLinearStateVFA,"The parameterization is explicitly given and no weights loaded.");
	CHOICE_ELEMENT("From-File", CLinearStateVFAFromFile, "The parameterization and weights are read from a file.");
	END_CHOICE();
	END_CLASS();
}

CLinearStateVFA::~CLinearStateVFA()
{
	/*if (m_saveFilename)
		save(m_saveFilename);*/

	delete m_pStateFeatureMap;
	delete m_pAux;
}



void CLinearStateVFA::getFeatures(const CState* s, CFeatureList* outFeatures)
{
	assert (s);
	assert (outFeatures);
	m_pStateFeatureMap->getFeatures(s,0,outFeatures);
}

void CLinearStateVFA::getFeatureState(unsigned int feature, CState* s)
{
	assert(feature>=0 && feature<m_pStateFeatureMap->getTotalNumFeatures());
	m_pStateFeatureMap->getFeatureStateAction(feature,s,0);
}


void CLinearStateVFA::add(const CFeatureList* pFeatures, double alpha)
{
	assert(pFeatures);

	//replicating code because i think it will be more efficient avoiding the per-iteration if
	if (!m_bSaturateOutput)
		for (unsigned int i= 0; i<pFeatures->m_numFeatures; i++)
		{
			assert(pFeatures->m_pFeatures[i].m_index<m_numWeights);

			m_pWeights[pFeatures->m_pFeatures[i].m_index]+= alpha* pFeatures->m_pFeatures[i].m_factor;
		}
	else
		for (unsigned int i = 0; i<pFeatures->m_numFeatures; i++)
		{
			assert(pFeatures->m_pFeatures[i].m_index<m_numWeights);

			m_pWeights[pFeatures->m_pFeatures[i].m_index] = 
				std::min(m_maxOutput,
					std::max(m_minOutput
							, m_pWeights[pFeatures->m_pFeatures[i].m_index] + alpha* pFeatures->m_pFeatures[i].m_factor));
		}
}


double CLinearStateVFA::getValue(const CState *s)
{
	getFeatures(s, m_pAux);
	return CLinearVFA::getValue(m_pAux);
}




//STATE-ACTION VFA: Q(s,a), A(s,a), .../////////////////////////////////////////////////////////////////////

CLASS_CONSTRUCTOR(CLinearStateActionVFA) : CParamObject(pParameters)
{
	CHILD_CLASS_FACTORY(m_pStateFeatureMap,"State-Feature-Map","The state feature map","",CFeatureMap);
	CHILD_CLASS_FACTORY(m_pActionFeatureMap,"Action-Feature-Map","The action feature map","",CFeatureMap);

	m_numStateWeights = m_pStateFeatureMap->getTotalNumFeatures();
	m_numActionWeights = m_pActionFeatureMap->getTotalNumFeatures();
	m_numWeights = m_numStateWeights * m_numActionWeights;
	m_pWeights = new double[m_numWeights];

	m_pAux = new CFeatureList("LinearStateActionVFA/aux");

	double initValue;
	CONST_DOUBLE_VALUE(initValue, "Init-Value", 0.0,"The initial value given to the weights on initialization");
	for (unsigned int i = 0; i < m_numWeights; i++)
		m_pWeights[i] = initValue;
	//std::fill_n(m_pWeights, m_numWeights, initValue);

	m_bSaturateOutput = false;
	m_minOutput = 0.0;
	m_maxOutput = 0.0;
	END_CLASS();
}

CLinearStateActionVFA::~CLinearStateActionVFA()
{
	delete m_pStateFeatureMap;
	delete m_pActionFeatureMap;

	delete m_pAux;
}



void CLinearStateActionVFA::getFeatures(const CState* s, const CAction* a, CFeatureList* outFeatures)
{
	assert(s); assert(a);
	assert(outFeatures);
	m_pStateFeatureMap->getFeatures(s, 0, m_pAux);

	m_pActionFeatureMap->getFeatures(0, a, outFeatures);

	m_pAux->spawn(outFeatures, m_numStateWeights);
}

void CLinearStateActionVFA::getFeatureStateAction(unsigned int feature, CState* s, CAction* a)
{
	assert(feature >= 0 && feature<m_numWeights);

	if (s)
		m_pStateFeatureMap->getFeatureStateAction(feature % m_numStateWeights, s, 0);
	if (a)
		m_pActionFeatureMap->getFeatureStateAction(feature / m_numStateWeights, 0, a);
}


void CLinearStateActionVFA::add(const CFeatureList* pFeatures, double alpha)
{
	assert(pFeatures);

	//replicating code because i think it will be more efficient avoiding the per-iteration if
	if (!m_bSaturateOutput)
	for (unsigned int i = 0; i<pFeatures->m_numFeatures; i++)
	{
		assert(pFeatures->m_pFeatures[i].m_index<m_numWeights);

		m_pWeights[pFeatures->m_pFeatures[i].m_index] += alpha* pFeatures->m_pFeatures[i].m_factor;
	}
	else
	for (unsigned int i = 0; i<pFeatures->m_numFeatures; i++)
	{
		assert(pFeatures->m_pFeatures[i].m_index<m_numWeights);

		m_pWeights[pFeatures->m_pFeatures[i].m_index] =
			std::min(m_maxOutput,
			std::max(m_minOutput
			, m_pWeights[pFeatures->m_pFeatures[i].m_index] + alpha* pFeatures->m_pFeatures[i].m_factor));
	}
}




double CLinearStateActionVFA::getValue(const CState *s, const CAction* a)
{
	getFeatures(s, a, m_pAux);

	return CLinearVFA::getValue(m_pAux);
}


void CLinearStateActionVFA::argMax(const CState *s, CAction* a)
{
	//state features in aux list
	getFeatures(s, a, m_pAux);

	double value= 0.0;
	double maxValue = std::numeric_limits<double>::lowest();
	unsigned int arg = -1;

	//action-value maximization
	for (unsigned int i = 0; i < m_numActionWeights; i++)
	{
		m_pAux->offsetIndices(m_numStateWeights);

		value= getValue(m_pAux);
		if (value>maxValue)
		{
			maxValue = value;
			arg = i;
		}
	}

	//retrieve action
	getFeatureStateAction(arg,0,a);
}

