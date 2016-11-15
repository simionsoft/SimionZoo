#include "stdafx.h"
#include "vfa.h"
#include "featuremap.h"
#include "features.h"
#include "config.h"
#include "globals.h"
#include "logger.h"
#include "app.h"
#include "simgod.h"

//LINEAR VFA. Common functionalities: getValue (CFeatureList*), saturate, save, load, ....


double CLinearVFA::getValue(const CFeatureList *pFeatures)
{
	double value = 0.0;
	unsigned int localIndex;
	assert(pFeatures);
	for (unsigned int i = 0; i<pFeatures->m_numFeatures; i++)
	{
		if (m_minIndex <= pFeatures->m_pFeatures[i].m_index && m_maxIndex > pFeatures->m_pFeatures[i].m_index)
		{
			//offset
			localIndex = pFeatures->m_pFeatures[i].m_index - m_minIndex;

			value += m_pWeights.get()[localIndex] * pFeatures->m_pFeatures[i].m_factor;
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


//STATE VFA: V(s), pi(s), .../////////////////////////////////////////////////////////////////////

CLASS_CONSTRUCTOR(CLinearStateVFA): CDeferredLoad()
{
	m_pStateFeatureMap = CApp::get()->SimGod.getGlobalStateFeatureMap();
	//CHOICE_INLINE("Feature-Map", "Choose from an explicit feature map or the global feature map");
	//	CHOICE_ELEMENT_INLINE("Global", ,"Use the global state feature map",m_pStateFeatureMap=CApp::get()->SimGod.getGlobalStateFeatureMap());
	//	CHOICE_ELEMENT_INLINE_FACTORY("Explicit",CStateFeatureMap,"Define an explicit feature map",m_pStateFeatureMap=CStateFeatureMap::getInstance(pChild->getChild("Explicit")););
	//END_CHOICE_INLINE();
	//default initialization
	//if (m_pStateFeatureMap == 0)
	//	m_pStateFeatureMap = CApp::get()->SimGod.getGlobalStateFeatureMap();

	m_numWeights = m_pStateFeatureMap.get()->getTotalNumFeatures();
	m_pWeights = 0;
	m_minIndex = 0;
	m_maxIndex = m_numWeights;

	m_pAux = new CFeatureList("LinearStateVFA/aux");
	CONST_DOUBLE_VALUE(m_initValue, "Init-Value", 0.0, "The initial value given to the weights on initialization");

	m_bSaturateOutput = false;
	m_minOutput = 0.0;
	m_maxOutput = 0.0;
	END_CLASS();
}
void CLinearStateVFA::deferredLoadStep()
{
	m_pWeights = std::unique_ptr<double>(new double[m_numWeights]);
	for (unsigned int i = 0; i < m_numWeights; i++)
		m_pWeights.get()[i] = m_initValue;
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


void CLinearStateVFA::add(const CFeatureList* pFeatures, double alpha)
{
	assert(pFeatures);


	//replicating code because i think it will be more efficient avoiding the per-iteration if
	if (!m_bSaturateOutput)
		for (unsigned int i= 0; i<pFeatures->m_numFeatures; i++)
		{
			//IF instead of assert because some features may not belong to this specific VFA (for example, in a VFAPolicy with 2 VFAs: StochasticPolicyGaussianNose)
			if (pFeatures->m_pFeatures[i].m_index >= m_minIndex && pFeatures->m_pFeatures[i].m_index<m_maxIndex)
				m_pWeights.get()[pFeatures->m_pFeatures[i].m_index]+= alpha* pFeatures->m_pFeatures[i].m_factor;
		}
	else
		for (unsigned int i = 0; i<pFeatures->m_numFeatures; i++)
		{
			//IF instead of assert because some features may not belong to this specific VFA (for example, in a VFAPolicy with 2 VFAs: StochasticPolicyGaussianNose)
			if (pFeatures->m_pFeatures[i].m_index >= m_minIndex && pFeatures->m_pFeatures[i].m_index<m_maxIndex)
				m_pWeights.get()[pFeatures->m_pFeatures[i].m_index] = 
					std::min(m_maxOutput,
						std::max(m_minOutput
								, m_pWeights.get()[pFeatures->m_pFeatures[i].m_index] + alpha* pFeatures->m_pFeatures[i].m_factor));
		}
}


double CLinearStateVFA::getValue(const CState *s)
{
	getFeatures(s, m_pAux);

	return CLinearVFA::getValue(m_pAux);
}




//STATE-ACTION VFA: Q(s,a), A(s,a), .../////////////////////////////////////////////////////////////////////

CLASS_CONSTRUCTOR(CLinearStateActionVFA)
{
	m_pStateFeatureMap = CApp::get()->SimGod.getGlobalStateFeatureMap();
	//m_pStateFeatureMap = 0;
	//CHOICE_INLINE("State-Feature-Map", "Choose from an explicit feature map or the global feature map");
	//CHOICE_ELEMENT_INLINE("Global", , "Use the global state feature map", m_pStateFeatureMap= CApp::get()->SimGod.getGlobalStateFeatureMap());
	//CHOICE_ELEMENT_INLINE_FACTORY("Explicit", CStateFeatureMap, "Define an explicit feature map", CStateFeatureMap::getInstance(pChild->getChild("Explicit")););
	//END_CHOICE_INLINE();
	////default initialization
	//if (m_pStateFeatureMap == 0)
	//	m_pStateFeatureMap = CApp::get()->SimGod.getGlobalStateFeatureMap();

	m_pActionFeatureMap = CApp::get()->SimGod.getGlobalActionFeatureMap();
	//m_pActionFeatureMap = 0;
	//CHOICE_INLINE("Action-Feature-Map", "Choose from an explicit feature map or the global feature map");
	//CHOICE_ELEMENT_INLINE("Global", , "Use the global action feature map", m_pActionFeatureMap= CApp::get()->SimGod.getGlobalActionFeatureMap());
	//CHOICE_ELEMENT_INLINE_FACTORY("Explicit", CActionFeatureMap, "Define an explicit feature map", CActionFeatureMap::getInstance(pChild->getChild("Explicit")););
	//END_CHOICE_INLINE();
	////default initialization
	//if (m_pActionFeatureMap == 0)
	//	m_pActionFeatureMap = CApp::get()->SimGod.getGlobalActionFeatureMap();

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

	CONST_DOUBLE_VALUE(m_initValue, "Init-Value", 0.0,"The initial value given to the weights on initialization");

	m_bSaturateOutput = false;
	m_minOutput = 0.0;
	m_maxOutput = 0.0;
	END_CLASS();
}

CLinearStateActionVFA::CLinearStateActionVFA(CLinearStateActionVFA* pSourceVFA)
{
	m_pStateFeatureMap = CApp::get()->SimGod.getGlobalStateFeatureMap();
	m_pActionFeatureMap = CApp::get()->SimGod.getGlobalActionFeatureMap();

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

	m_initValue = pSourceVFA->m_initValue;

	m_bSaturateOutput = false;
	m_minOutput = 0.0;
	m_maxOutput = 0.0;
}

CLinearStateActionVFA::~CLinearStateActionVFA()
{
	//now SimGod owns the feature maps -> his responsability to free memory
	//delete m_pStateFeatureMap;
	//delete m_pActionFeatureMap;

	delete m_pAux;
	delete m_pAux2;
}

void CLinearStateActionVFA::deferredLoadStep()
{
	m_pWeights= std::unique_ptr<double>(new double[m_numWeights]);
	for (unsigned int i = 0; i < m_numWeights; i++)
		m_pWeights.get()[i] = m_initValue;
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
				m_pWeights.get()[pFeatures->m_pFeatures[i].m_index] += alpha* pFeatures->m_pFeatures[i].m_factor;
			}
			else
			{
				m_pWeights.get()[pFeatures->m_pFeatures[i].m_index] =
					std::min(m_maxOutput,
					std::max(m_minOutput
					, m_pWeights.get()[pFeatures->m_pFeatures[i].m_index] + alpha* pFeatures->m_pFeatures[i].m_factor));
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