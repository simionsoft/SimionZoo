#include "stdafx.h"
#include "vfa.h"
#include "featuremap.h"
#include "features.h"
#include "parameters.h"
#include "globals.h"
#include "logger.h"
#include "app.h"
#include "simgod.h"

//LINEAR VFA. Common functionalities: getValue (CFeatureList*), saturate, save, load, ....
CLinearVFA::CLinearVFA(CParameters* pParameters) : CParamObject(pParameters)
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

void CLinearVFA::setIndexOffset(unsigned int offset)
{
	m_minIndex = offset;
	m_maxIndex = offset + m_numWeights;
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
	else
		CLogger::logMessage(MessageType::Error,"Couldn't open binary file with VFA weights");
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
	else
		CLogger::logMessage(MessageType::Warning, "Couldn't load weights from file");
	return false;
}

void CLinearStateVFA::save(const char* pFilename)
{
	CParameters* pFeatureMapParameters;
	char msg[128];
	char binFile[512];
	char xmlDescFile[512];
	FILE* pXMLFile;

	if ( pFilename == 0 || pFilename[0] == 0) return;

	sprintf_s(msg, 128, "Saving Policy to \"%s\" (.fmap/.weights)...", pFilename);
	CLogger::logMessage(Info, msg);

	sprintf_s(binFile, 512, "%s.weights", pFilename);
	saveWeights(binFile);

	sprintf_s(xmlDescFile, 512, "%s.fmap", pFilename);

	fopen_s(&pXMLFile, xmlDescFile, "w");
	if (pXMLFile)
	{
		pFeatureMapParameters= m_pStateFeatureMap->getParameters();

		if (pFeatureMapParameters)
			pFeatureMapParameters->saveFile(pXMLFile);

		fclose(pXMLFile);
		CLogger::logMessage(Info, "OK");
		return;
	}

	sprintf_s(msg, 128, "Couldn't save the policy", binFile);
	CLogger::logMessage(Warning, msg);
}



//STATE VFA: V(s), pi(s), .../////////////////////////////////////////////////////////////////////

CLASS_CONSTRUCTOR(CLinearStateVFA) : CLinearVFA(pParameters), CDeferredLoad()
{
	CHILD_CLASS_FACTORY(m_pStateFeatureMap,"State-Feature-Map","The feature map fuction: state->features",false,CStateFeatureMap);

	m_numWeights = m_pStateFeatureMap->getTotalNumFeatures();
	m_pWeights = 0;
	m_minIndex = 0;
	m_maxIndex = m_numWeights;

	m_pAux = new CFeatureList("LinearStateVFA/aux");

	m_bSaturateOutput = false;
	m_minOutput = 0.0;
	m_maxOutput = 0.0;
	END_CLASS();
}
void CLinearStateVFA::deferredLoadStep()
{
	m_pWeights = new double[m_numWeights];
}
CLinearStateVFA::CLinearStateVFA() : CLinearVFA(0)
{}
//
//CLASS_CONSTRUCTOR(CLinearStateVFAFromFile) :CLinearStateVFA()
//{
//	//load the map feature description from an xml file
//	FILE_PATH_VALUE(m_loadFilename, "Load", "../data/*.fmap", "The VFA will be loaded from this file");
//	CApp::get()->SimGod.registerInputFile(m_loadFilename);
//
//	strcpy_s(m_weightFilename, 1024, m_loadFilename);
//	char* extension = strstr(m_weightFilename, "fmap");
//	if (extension)
//		strcpy_s(extension, 1024 - (extension - m_weightFilename + 1), "weights");
//	CApp::get()->SimGod.registerInputFile(m_weightFilename);
//
//
//	m_pAux = new CFeatureList("LinearStateVFA/aux");
//	m_mapFeatureParameterFile = 0;
//	m_mapFeatureParameters = 0;
//	m_minIndex= m_maxIndex= m_numWeights = 0;
//	m_pStateFeatureMap = 0;
//	m_bSaturateOutput = false;
//	m_minOutput = 0.0;
//	m_maxOutput = 0.0;
//	END_CLASS();
//}
//void CLinearStateVFAFromFile::deferredLoadStep()
//{
//	char message[1024];
//	sprintf_s(message, 1024, "Loaded %s file", m_loadFilename);
//	CLogger::logMessage(MessageType::Info, message);
//
//	m_mapFeatureParameterFile = new CParameterFile();
//	m_mapFeatureParameters = m_mapFeatureParameterFile->loadFile(m_loadFilename);
//	m_pStateFeatureMap = new CGaussianRBFStateGridFeatureMap(m_mapFeatureParameters);
//
//	m_numWeights = m_pStateFeatureMap->getTotalNumFeatures();
//	m_pWeights = new double[m_numWeights];
//	m_minIndex = 0;
//	m_maxIndex = m_numWeights;
//
//	//load the weigths from the binary file
//	loadWeights(m_weightFilename);
//}
//
//CLinearStateVFAFromFile::~CLinearStateVFAFromFile()
//{
//	delete m_mapFeatureParameterFile;
//}

CLASS_FACTORY(CLinearStateVFA)
{
	CHOICE("Parameterization", "Do we want to define the vfa explicitly or load it from file?");
	CHOICE_ELEMENT("Explicit", CLinearStateVFA,"The parameterization is explicitly given and no weights loaded.");
	//CHOICE_jELEMEN("From-File", CLinearStateVFAFromFile, "The parameterization and weights are read from a file.");
	END_CHOICE();
	END_CLASS();
}

CLinearStateVFA::~CLinearStateVFA()
{
	if (m_pStateFeatureMap) delete m_pStateFeatureMap;
	if (m_pAux) delete m_pAux;
}


void CLinearStateVFA::getFeatures(const CState* s, CFeatureList* outFeatures)
{
	assert (s);
	assert (outFeatures);
	m_pStateFeatureMap->getFeatures(s,outFeatures);
	outFeatures->offsetIndices(m_minIndex);
}

void CLinearStateVFA::getFeatureState(unsigned int feature, CState* s)
{
	if (feature>=m_minIndex && feature<m_maxIndex)
		m_pStateFeatureMap->getFeatureState(feature,s);
}


void CLinearStateVFA::add(const CFeatureList* pFeatures, double alpha)
{
	assert(pFeatures);


	//replicating code because i think it will be more efficient avoiding the per-iteration if
	if (!m_bSaturateOutput)
		for (unsigned int i= 0; i<pFeatures->m_numFeatures; i++)
		{
			//IF instead of assert because some features may not belong to this specific VFA (for example, in a VFAPolicy with 2 VFAs: StochasticPolicyGaussianNose)
			if (pFeatures->m_pFeatures[i].m_index >= m_minIndex && pFeatures->m_pFeatures[i].m_index<m_maxIndex)
				m_pWeights[pFeatures->m_pFeatures[i].m_index]+= alpha* pFeatures->m_pFeatures[i].m_factor;
		}
	else
		for (unsigned int i = 0; i<pFeatures->m_numFeatures; i++)
		{
			//IF instead of assert because some features may not belong to this specific VFA (for example, in a VFAPolicy with 2 VFAs: StochasticPolicyGaussianNose)
			if (pFeatures->m_pFeatures[i].m_index >= m_minIndex && pFeatures->m_pFeatures[i].m_index<m_maxIndex)
				m_pWeights[pFeatures->m_pFeatures[i].m_index] = 
					std::min(m_maxOutput,
						std::max(m_minOutput
								, m_pWeights[pFeatures->m_pFeatures[i].m_index] + alpha* pFeatures->m_pFeatures[i].m_factor));
		}
}


double CLinearStateVFA::getValue(const CState *s)
{
	getFeatures(s, m_pAux);
	m_pAux->offsetIndices(-(int)m_minIndex);
	return CLinearVFA::getValue(m_pAux);
}




//STATE-ACTION VFA: Q(s,a), A(s,a), .../////////////////////////////////////////////////////////////////////

CLASS_CONSTRUCTOR(CLinearStateActionVFA) : CLinearVFA(pParameters)
{
	CHILD_CLASS_FACTORY(m_pStateFeatureMap,"State-Feature-Map","The state feature map",false,CStateFeatureMap);
	CHILD_CLASS_FACTORY(m_pActionFeatureMap,"Action-Feature-Map","The action feature map",false,CActionFeatureMap);

	m_numStateWeights = m_pStateFeatureMap->getTotalNumFeatures();
	m_numActionWeights = m_pActionFeatureMap->getTotalNumFeatures();
	m_numWeights = m_numStateWeights * m_numActionWeights;
	m_pWeights = new double[m_numWeights];
	m_minIndex = 0;
	m_maxIndex = m_numWeights;

	//this is used in "high-level" methods
	m_pAux = new CFeatureList("LinearStateActionVFA/aux");
	//this is used in "lower-level" methods
	m_pAux2 = new CFeatureList("LinearStateActionVFA/aux2");

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
	unsigned int oldindex;
	assert(outFeatures);

	if (a)
	{
		m_pStateFeatureMap->getFeatures(s, outFeatures);
		oldindex = outFeatures->m_pFeatures[0].m_index;

		m_pActionFeatureMap->getFeatures(a, m_pAux2);
		assert(outFeatures->m_pFeatures[0].m_index == oldindex);

		outFeatures->spawn(m_pAux2, m_numStateWeights);

		outFeatures->offsetIndices(m_minIndex);
	}
	else if (s)
	{
		//if we want the state features only, no action values will be provided and the action should not be taken into account
		m_pStateFeatureMap->getFeatures(s, outFeatures);
	}
	else
		CLogger::logMessage(MessageType::Error, "CLinearStateActionVFA::getFeatures() called with neither a state nor an action");
}

void CLinearStateActionVFA::getFeatureStateAction(unsigned int feature, CState* s, CAction* a)
{
	if (feature >= m_minIndex && feature < m_maxIndex)
	{
		if (s)
			m_pStateFeatureMap->getFeatureState(feature % m_numStateWeights, s);
		if (a)
			m_pActionFeatureMap->getFeatureAction(feature / m_numStateWeights, a);
	}
}



void CLinearStateActionVFA::add(const CFeatureList* pFeatures, double alpha)
{
	assert(pFeatures);

	for (unsigned int i = 0; i<pFeatures->m_numFeatures; i++)
	{
		if (pFeatures->m_pFeatures[i].m_index >= m_minIndex && pFeatures->m_pFeatures[i].m_index < m_maxIndex)
		{
			if (!m_bSaturateOutput)
			{
				m_pWeights[pFeatures->m_pFeatures[i].m_index] += alpha* pFeatures->m_pFeatures[i].m_factor;
			}
			else
			{
				m_pWeights[pFeatures->m_pFeatures[i].m_index] =
					std::min(m_maxOutput,
					std::max(m_minOutput
					, m_pWeights[pFeatures->m_pFeatures[i].m_index] + alpha* pFeatures->m_pFeatures[i].m_factor));
			}
		}
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
	getFeatures(s, 0, m_pAux);

	double value= 0.0;
	double maxValue = std::numeric_limits<double>::lowest();
	unsigned int arg = -1;

	//action-value maximization
	for (unsigned int i = 0; i < m_numActionWeights; i++)
	{
		value= getValue(m_pAux);
		if (value>maxValue)
		{
			maxValue = value;
			arg = i;
		}

		m_pAux->offsetIndices(m_numStateWeights);
	}

	//retrieve action
	m_pActionFeatureMap->getFeatureAction(arg,a);
}

double CLinearStateActionVFA::max(const CState* s)
{
	//state features in aux list
	m_pStateFeatureMap->getFeatures(s, m_pAux);

	double value = 0.0;
	double maxValue = std::numeric_limits<double>::lowest();

	//action-value maximization
	for (unsigned int i = 0; i < m_numActionWeights; i++)
	{
		value = getValue(m_pAux);
		if (value>maxValue)
			maxValue = value;
	
		m_pAux->offsetIndices(m_numStateWeights);
	}

	return maxValue;
}

void CLinearStateActionVFA::getActionValues(const CState* s,double *outActionValues)
{
	if (!outActionValues)
		throw std::exception("CLinearStateActionVFA::getAction Values(...) tried to get action values without providing a buffer");
	//state features in aux list
	m_pStateFeatureMap->getFeatures(s, m_pAux);

	//action-value maximization
	for (unsigned int i = 0; i < m_numActionWeights; i++)
	{
		outActionValues[i] = getValue(m_pAux);

		m_pAux->offsetIndices(m_numStateWeights);
	}
}