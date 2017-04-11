#include "stdafx.h"
#include "vfa.h"
#include "featuremap.h"
#include "features.h"
#include "logger.h"
#include "app.h"
#include "SimGod.h"

//LINEAR VFA. Common functionalities: getSample (CFeatureList*), saturate, save, load, ....
CLinearVFA::CLinearVFA()
{
}

CLinearVFA::~CLinearVFA()
{
}

double CLinearVFA::get(const CFeatureList *pFeatures,bool bUseFrozenWeights)
{
	double value = 0.0;
	unsigned int localIndex;
	double pendingFeatureUpdate= 0.0;
	double *pWeights;

	if (!bUseFrozenWeights || !m_bCanBeFrozen)
		pWeights = m_pWeights.get();
	else
		pWeights = m_pFrozenWeights.get();

	for (unsigned int i = 0; i<pFeatures->m_numFeatures; i++)
	{
		if (m_minIndex <= pFeatures->m_pFeatures[i].m_index && m_maxIndex > pFeatures->m_pFeatures[i].m_index)
		{
			//offset
			localIndex = pFeatures->m_pFeatures[i].m_index - m_minIndex;

			value += pWeights[localIndex] * pFeatures->m_pFeatures[i].m_factor;
		}
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

bool CLinearVFA::saveWeights(const char* pFilename) const
{
	FILE* pFile;
	assert(m_pWeights && m_numWeights >= 0);

	fopen_s(&pFile, pFilename, "wb");
	if (pFile)
	{
		fwrite(&m_numWeights, sizeof(unsigned int), 1, (FILE*)pFile);
		fwrite(m_pWeights.get(), sizeof(double), m_numWeights, (FILE*)pFile);
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

		fread_s(m_pWeights.get(), numWeightsRead*sizeof(double), sizeof(double), numWeightsRead, (FILE*)pFile);

		fclose(pFile);
		return true;
	}
	else
		CLogger::logMessage(MessageType::Warning, "Couldn't load weights from file");
	return false;
}

void CLinearStateVFA::save(const char* pFilename) const
{
	CConfigNode* pFeatureMapParameters;
	char msg[128];
	char binFile[512];
	char xmlDescFile[512];
	FILE* pXMLFile;

	if (pFilename == 0 || pFilename[0] == 0) return;

	sprintf_s(msg, 128, "Saving Policy to \"%s\" (.fmap/.weights)...", pFilename);
	CLogger::logMessage(Info, msg);

	sprintf_s(binFile, 512, "%s.weights", pFilename);
	saveWeights(binFile);

	sprintf_s(xmlDescFile, 512, "%s.fmap", pFilename);

	fopen_s(&pXMLFile, xmlDescFile, "w");
	if (pXMLFile)
	{
		pFeatureMapParameters = m_pStateFeatureMap.get()->getParameters();

		if (pFeatureMapParameters)
			pFeatureMapParameters->saveFile(pXMLFile);

		fclose(pXMLFile);
		CLogger::logMessage(Info, "OK");
		return;
	}

	sprintf_s(msg, 128, "Couldn't save the policy in file %s", binFile);
	CLogger::logMessage(Warning, msg);
}


void CLinearVFA::add(const CFeatureList* pFeatures, double alpha)
{
	int vUpdateFreq = 0;
	int experimentStep = 0;

	//then we apply all the feature updates
	for (unsigned int i = 0; i < pFeatures->m_numFeatures; i++)
	{
		//IF instead of assert because some features may not belong to this specific VFA
		//and would still be a valid operation
		//(for example, in a VFAPolicy with 2 VFAs: StochasticPolicyGaussianNose)
		if (!m_bSaturateOutput)
		{
			if (pFeatures->m_pFeatures[i].m_index >= m_minIndex && pFeatures->m_pFeatures[i].m_index < m_maxIndex)
				m_pWeights.get()[pFeatures->m_pFeatures[i].m_index] += alpha*pFeatures->m_pFeatures[i].m_factor;	
		}
		else
		{
			if (pFeatures->m_pFeatures[i].m_index >= m_minIndex && pFeatures->m_pFeatures[i].m_index < m_maxIndex)
				m_pWeights.get()[pFeatures->m_pFeatures[i].m_index] =
				std::min(m_maxOutput,std::max(m_minOutput
					, m_pWeights.get()[pFeatures->m_pFeatures[i].m_index] + alpha * pFeatures->m_pFeatures[i].m_factor));
		}
	}
	//if we are deferring updates, check whether we have to update frozen weights
	if (m_bCanBeFrozen)
	{
		vUpdateFreq = CSimionApp::get()->pSimGod->getVFunctionUpdateFreq();
		experimentStep = CSimionApp::get()->pExperiment->getExperimentStep();

		if (vUpdateFreq == 0 || experimentStep % vUpdateFreq == 0)
		{
			memcpy(m_pFrozenWeights.get(), m_pWeights.get(), m_numWeights * sizeof(double));
		}
	}
}


//STATE VFA: V(s), pi(s), .../////////////////////////////////////////////////////////////////////

CLinearStateVFA::CLinearStateVFA(CConfigNode* pConfigNode)
{
	m_pStateFeatureMap = CSimGod::getGlobalStateFeatureMap();

	m_numWeights = m_pStateFeatureMap.get()->getTotalNumFeatures();
	m_pWeights = 0;
	m_minIndex = 0;
	m_maxIndex = m_numWeights;

	m_pAux = new CFeatureList("LinearStateVFA/aux");
	m_initValue= DOUBLE_PARAM(pConfigNode, "Init-Value", "The initial value given to the weights on initialization", 0.0);

	m_bSaturateOutput = false;
	m_minOutput = 0.0;
	m_maxOutput = 0.0;
}
void CLinearStateVFA::deferredLoadStep()
{
	//weights
	m_pWeights = std::shared_ptr<double>(new double[m_numWeights]);
	for (unsigned int i = 0; i < m_numWeights; i++)
		m_pWeights.get()[i] = m_initValue.get();

	//frozen weights
	if (m_bCanBeFrozen)
	{
		m_pFrozenWeights = std::shared_ptr<double>(new double[m_numWeights]);
		for (unsigned int i = 0; i < m_numWeights; i++)
			m_pFrozenWeights.get()[i] = m_initValue.get();
	}
}

void CLinearStateVFA::setInitValue(double initValue)
{
	m_initValue.set(initValue);
}

CLinearStateVFA::CLinearStateVFA()
{
}


CLinearStateVFA::~CLinearStateVFA()
{
	//now SimGod owns the feature map, his duty to free the memory
	//if (m_pStateFeatureMap) delete m_pStateFeatureMap;
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

double CLinearStateVFA::get(const CState *s)
{
	getFeatures(s, m_pAux);

	return CLinearVFA::get(m_pAux);
}




//STATE-ACTION VFA: Q(s,a), A(s,a), .../////////////////////////////////////////////////////////////////////

CLinearStateActionVFA::CLinearStateActionVFA(std::shared_ptr<CStateFeatureMap> pStateFeatureMap, std::shared_ptr<CActionFeatureMap> pActionFeatureMap)
{
	m_pStateFeatureMap = pStateFeatureMap;
	m_pActionFeatureMap = pActionFeatureMap;

	m_numStateWeights = m_pStateFeatureMap->getTotalNumFeatures();
	m_numActionWeights = m_pActionFeatureMap->getTotalNumFeatures();
	m_numWeights = m_numStateWeights * m_numActionWeights;
	m_pWeights = 0;
	m_minIndex = 0;
	m_maxIndex = m_numWeights;

	//this is used in "high-level" methods
	m_pAux = new CFeatureList("LinearStateActionVFA/aux");
	//this is used in "lower-level" methods
	m_pAux2 = new CFeatureList("LinearStateActionVFA/aux2");

	m_bSaturateOutput = false;
	m_minOutput = 0.0;
	m_maxOutput = 0.0;
}

CLinearStateActionVFA::CLinearStateActionVFA(CConfigNode* pConfigNode)
	:CLinearStateActionVFA(CSimGod::getGlobalStateFeatureMap(),CSimGod::getGlobalActionFeatureMap())
{
	m_initValue= DOUBLE_PARAM(pConfigNode, "Init-Value","The initial value given to the weights on initialization", 0.0);
}

CLinearStateActionVFA::CLinearStateActionVFA(CLinearStateActionVFA* pSourceVFA)
	: CLinearStateActionVFA(CSimGod::getGlobalStateFeatureMap(), CSimGod::getGlobalActionFeatureMap())
{
	m_initValue = pSourceVFA->m_initValue;
}

CLinearStateActionVFA::~CLinearStateActionVFA()
{
	//now SimGod owns the feature maps -> his responsability to free memory
	delete m_pAux;
	delete m_pAux2;
}

void CLinearStateActionVFA::setInitValue(double initValue)
{
	m_initValue.set(initValue);
}

void CLinearStateActionVFA::deferredLoadStep()
{
	//weights
	m_pWeights= std::unique_ptr<double>(new double[m_numWeights]);
	for (unsigned int i = 0; i < m_numWeights; i++)
		m_pWeights.get()[i] = m_initValue.get();

	//frozen weights
	if (m_bCanBeFrozen)
	{
		m_pFrozenWeights = std::shared_ptr<double>(new double[m_numWeights]);
		for (unsigned int i = 0; i < m_numWeights; i++)
			m_pFrozenWeights.get()[i] = m_initValue.get();
	}
}

void CLinearStateActionVFA::getFeatures(const CState* s, const CAction* a, CFeatureList* outFeatures)
{
	assert(outFeatures);

	if (a)
	{
		m_pStateFeatureMap->getFeatures(s, outFeatures);

		m_pActionFeatureMap->getFeatures(a, m_pAux2);

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



double CLinearStateActionVFA::get(const CState *s, const CAction* a)
{
	getFeatures(s, a, m_pAux);

	return CLinearVFA::get(m_pAux);
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
		value= get(m_pAux);
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
		value = get(m_pAux);
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
		outActionValues[i] = get(m_pAux);

		m_pAux->offsetIndices(m_numStateWeights);
	}
}