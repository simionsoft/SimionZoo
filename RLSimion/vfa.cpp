#include "vfa.h"
#include "featuremap.h"
#include "features.h"
#include "logger.h"
#include "app.h"
#include "SimGod.h"
#include "experiment.h"
#include <assert.h>
#include <algorithm>

//LINEAR VFA. Common functionalities: getSample (FeatureList*), saturate, save, load, ....
LinearVFA::LinearVFA()
{
	m_pPendingUpdates = new FeatureList("Pending-vfa-updates", OverwriteMode::AllowDuplicates);
}

LinearVFA::~LinearVFA()
{
	delete[] m_pPendingUpdates;
}

void LinearVFA::setCanUseDeferredUpdates(bool bCanUseDeferredUpdates)
{
	m_bCanBeFrozen = bCanUseDeferredUpdates;
}

double LinearVFA::get(const FeatureList *pFeatures,bool bUseFrozenWeights)
{
	double value = 0.0;
	size_t localIndex;

	IMemBuffer *pWeights;
	int updateFreq = SimionApp::get()->pSimGod->getTargetFunctionUpdateFreq();

	if (!bUseFrozenWeights || !m_bCanBeFrozen || updateFreq == 0)
		pWeights = m_pWeights;
	else
		pWeights = m_pFrozenWeights;

	for (size_t i = 0; i<pFeatures->m_numFeatures; i++)
	{
		if (m_minIndex <= pFeatures->m_pFeatures[i].m_index && m_maxIndex > pFeatures->m_pFeatures[i].m_index)
		{
			//offset
			localIndex = pFeatures->m_pFeatures[i].m_index - m_minIndex;

			value += (*pWeights)[localIndex] * pFeatures->m_pFeatures[i].m_factor;
		}
	}
	return value;
}

void LinearVFA::saturateOutput(double min, double max)
{
	m_bSaturateOutput = true;
	m_minOutput = min;
	m_maxOutput = max;
}

void LinearVFA::setIndexOffset(unsigned int offset)
{
	m_minIndex = offset;
	m_maxIndex = offset + m_numWeights;
}

bool LinearVFA::saveWeights(const char* pFilename) const
{
	FILE* pFile;
	assert(m_pWeights && m_numWeights >= 0);

	fopen_s(&pFile, pFilename, "wb");
	if (pFile)
	{
		fwrite(&m_numWeights, sizeof(unsigned int), 1, (FILE*)pFile);
		for (unsigned int i= 0; i<m_numWeights; ++i)
			fwrite(&(*m_pWeights)[i], sizeof(double), 1, (FILE*)pFile);
		fclose(pFile);
		return true;
	}
	else
		Logger::logMessage(MessageType::Error,"Couldn't open binary file with VFA weights");
	return false;
}

bool LinearVFA::loadWeights(const char* pFilename)
{
	FILE* pFile;
	unsigned int numWeightsRead;

	fopen_s(&pFile, pFilename, "rb");
	if (pFile)
	{
		fread_s(&numWeightsRead, sizeof(unsigned int), sizeof(unsigned int), 1, (FILE*)pFile);

		assert(m_numWeights == numWeightsRead);

		for(unsigned int i= 0; i<numWeightsRead; ++i)
			fread_s(&(*m_pWeights)[i], sizeof(double), sizeof(double), 1, (FILE*)pFile);

		fclose(pFile);
		return true;
	}
	else
		Logger::logMessage(MessageType::Warning, "Couldn't load weights from file");
	return false;
}

void LinearStateVFA::save(const char* pFilename) const
{
	ConfigNode* pFeatureMapParameters;
	char msg[128];
	char binFile[512];
	char xmlDescFile[512];
	FILE* pXMLFile;

	if (pFilename == 0 || pFilename[0] == 0) return;

	sprintf_s(msg, 128, "Saving Policy to \"%s\" (.fmap/.weights)...", pFilename);
	Logger::logMessage(Info, msg);

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
		Logger::logMessage(Info, "OK");
		return;
	}

	sprintf_s(msg, 128, "Couldn't save the policy in file %s", binFile);
	Logger::logMessage(Warning, msg);
}


void LinearVFA::add(const FeatureList* pFeatures, double alpha)
{
	int vUpdateFreq = 0;
	int experimentStep = 0;
	bool bFreezeTarget;

	//If we are deferring updates, check whether we have to update frozen weights
	//Note: vUpdate=0 if we are not deferring updates
	vUpdateFreq = SimionApp::get()->pSimGod->getTargetFunctionUpdateFreq();
	bFreezeTarget = (vUpdateFreq != 0) && m_bCanBeFrozen;

	//then we apply all the feature updates
	for (unsigned int i = 0; i < pFeatures->m_numFeatures; i++)
	{
		//index is too low, does not correspond to this map!
		if (pFeatures->m_pFeatures[i].m_index < m_minIndex)
			continue;
		//index is too high, does not correspond to this map, too!
		if (pFeatures->m_pFeatures[i].m_index - m_minIndex > m_maxIndex)
			continue;

		//IF instead of assert because some features may not belong to this specific VFA
		//and would still be a valid operation
		//(for example, in a VFAPolicy with 2 VFAs: StochasticPolicyGaussianNose)
		double inc;
		if (!m_bSaturateOutput)
			inc= alpha*pFeatures->m_pFeatures[i].m_factor;
		else
		{
			inc= std::min(m_maxOutput, std::max(m_minOutput, (*m_pWeights)[pFeatures->m_pFeatures[i].m_index - m_minIndex]
				+ alpha * pFeatures->m_pFeatures[i].m_factor)) - (*m_pWeights)[pFeatures->m_pFeatures[i].m_index - m_minIndex];
		}
		(*m_pWeights)[pFeatures->m_pFeatures[i].m_index - m_minIndex] += inc;
		if (bFreezeTarget)
			m_pPendingUpdates->add(pFeatures->m_pFeatures[i].m_index, inc);
	}

	if (bFreezeTarget && !SimionApp::get()->pSimGod->bReplayingExperience())
	{
		experimentStep = SimionApp::get()->pExperiment->getExperimentStep();

		if (experimentStep % vUpdateFreq == 0)
		{
			for (unsigned int i = 0; i < m_pPendingUpdates->m_numFeatures; ++i)
			{
				(*m_pFrozenWeights)[m_pPendingUpdates->m_pFeatures[i].m_index]
					+= m_pPendingUpdates->m_pFeatures[i].m_factor;
			}
			m_pPendingUpdates->clear();
		}
	}
}


//STATE VFA: V(s), pi(s), .../////////////////////////////////////////////////////////////////////

LinearStateVFA::LinearStateVFA(ConfigNode* pConfigNode)
{
	m_pStateFeatureMap = SimGod::getGlobalStateFeatureMap();

	m_numWeights = m_pStateFeatureMap.get()->getTotalNumFeatures();
	m_pWeights = 0;
	m_minIndex = 0;
	m_maxIndex = m_numWeights;

	m_pAux = new FeatureList("LinearStateVFA/aux");
	m_initValue= DOUBLE_PARAM(pConfigNode, "Init-Value", "The initial value given to the weights on initialization", 0.0);

	m_bSaturateOutput = false;
	m_minOutput = 0.0;
	m_maxOutput = 0.0;
}
void LinearStateVFA::deferredLoadStep()
{
	//weights
	m_pWeights = SimionApp::get()->pMemManager->getMemBuffer(m_numWeights);//std::shared_ptr<double>(new double[m_numWeights]);
	m_pWeights->setInitValue(m_initValue.get());

	//frozen weights
	if (m_bCanBeFrozen)
	{
		m_pFrozenWeights = SimionApp::get()->pMemManager->getMemBuffer(m_numWeights); //std::shared_ptr<double>(new double[m_numWeights]);
		m_pFrozenWeights->setInitValue(m_initValue.get());
	}
}

void LinearStateVFA::setInitValue(double initValue)
{
	m_initValue.set(initValue);
}

LinearStateVFA::LinearStateVFA()
{
}


LinearStateVFA::~LinearStateVFA()
{
	//now SimGod owns the feature map, his duty to free the memory
	//if (m_pStateFeatureMap) delete m_pStateFeatureMap;
	if (m_pAux) delete m_pAux;
}


void LinearStateVFA::getFeatures(const State* s, FeatureList* outFeatures)
{
	assert (s);
	assert (outFeatures);
	m_pStateFeatureMap->getFeatures(s,outFeatures);
	outFeatures->offsetIndices(m_minIndex);
}

void LinearStateVFA::getFeatureState(unsigned int feature, State* s)
{
	if (feature>=m_minIndex && feature<m_maxIndex)
		m_pStateFeatureMap->getFeatureState(feature,s);
}

double LinearStateVFA::get(const State *s)
{
	getFeatures(s, m_pAux);

	return LinearVFA::get(m_pAux);
}




//STATE-ACTION VFA: Q(s,a), A(s,a), .../////////////////////////////////////////////////////////////////////

LinearStateActionVFA::LinearStateActionVFA(std::shared_ptr<StateFeatureMap> pStateFeatureMap, std::shared_ptr<ActionFeatureMap> pActionFeatureMap)
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
	m_pAux = new FeatureList("LinearStateActionVFA/aux");
	//this is used in "lower-level" methods
	m_pAux2 = new FeatureList("LinearStateActionVFA/aux2");

	m_bSaturateOutput = false;
	m_minOutput = 0.0;
	m_maxOutput = 0.0;
}

LinearStateActionVFA::LinearStateActionVFA(ConfigNode* pConfigNode)
	:LinearStateActionVFA(SimGod::getGlobalStateFeatureMap(),SimGod::getGlobalActionFeatureMap())
{
	m_initValue= DOUBLE_PARAM(pConfigNode, "Init-Value","The initial value given to the weights on initialization", 0.0);
}

LinearStateActionVFA::LinearStateActionVFA(LinearStateActionVFA* pSourceVFA)
	: LinearStateActionVFA(SimGod::getGlobalStateFeatureMap(), SimGod::getGlobalActionFeatureMap())
{
	m_initValue = pSourceVFA->m_initValue;
}

LinearStateActionVFA::~LinearStateActionVFA()
{
	//now SimGod owns the feature maps -> his responsability to free memory
	delete m_pAux;
	delete m_pAux2;

	if (m_pArgMaxTies) delete [] m_pArgMaxTies;
}

void LinearStateActionVFA::setInitValue(double initValue)
{
	m_initValue.set(initValue);
}

void LinearStateActionVFA::deferredLoadStep()
{
	//weights
	m_pWeights= SimionApp::get()->pMemManager->getMemBuffer(m_numWeights);
	m_pWeights->setInitValue(m_initValue.get());

	//frozen weights
	if (m_bCanBeFrozen)
	{
		m_pFrozenWeights = SimionApp::get()->pMemManager->getMemBuffer(m_numWeights);
		m_pFrozenWeights->setInitValue(m_initValue.get());
	}

	//buffer to solve value ties in argMax()
	m_pArgMaxTies = new int[m_numActionWeights];
}

void LinearStateActionVFA::getFeatures(const State* s, const Action* a, FeatureList* outFeatures)
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
		Logger::logMessage(MessageType::Error, "LinearStateActionVFA::getFeatures() called with neither a state nor an action");
}

void LinearStateActionVFA::getFeatureStateAction(unsigned int feature, State* s, Action* a)
{
	if (feature >= m_minIndex && feature < m_maxIndex)
	{
		if (s)
			m_pStateFeatureMap->getFeatureState(feature % m_numStateWeights, s);
		if (a)
			m_pActionFeatureMap->getFeatureAction(feature / m_numStateWeights, a);
	}
}



double LinearStateActionVFA::get(const State *s, const Action* a)
{
	getFeatures(s, a, m_pAux);

	return LinearVFA::get(m_pAux);
}

//double LinearStateActionVFA::get(unsigned int sFeatureIndex, unsigned int aFeatureIndex) const
//{
//	return (*m_pWeights)[sFeatureIndex*m_pActionFeatureMap->getTotalNumFeatures() + aFeatureIndex];
//}

void LinearStateActionVFA::argMax(const State *s, Action* a)
{
	int numTies = 0;
	//state features in aux list
	getFeatures(s, 0, m_pAux);

	double value = 0.0;
	double maxValue = std::numeric_limits<double>::lowest();
	unsigned int arg = -1;

	//action-value maximization
	for (unsigned int i = 0; i < m_numActionWeights; i++)
	{
		value = get(m_pAux);
		if (value == maxValue)
		{
			m_pArgMaxTies[numTies++] = i;
		}
		if (value>maxValue)
		{
			maxValue = value;
			arg = i;
			m_pArgMaxTies[0] = i;
			numTies = 1;
		}

		m_pAux->offsetIndices(m_numStateWeights);
	}

	//any ties?
	if (numTies > 1)
		arg = m_pArgMaxTies[rand() % numTies]; //select one randomly

	//retrieve action
	m_pActionFeatureMap->getFeatureAction(arg, a);
}

double LinearStateActionVFA::max(const State* s, bool bUseFrozenWeights)
{
	//state features in aux list
	m_pStateFeatureMap->getFeatures(s, m_pAux);

	double value = 0.0;
	double maxValue = std::numeric_limits<double>::lowest();

	//action-value maximization
	for (unsigned int i = 0; i < m_numActionWeights; i++)
	{
		value = get(m_pAux, bUseFrozenWeights); //if the target is frozen, we use the frozen weights
		if (value>maxValue)
			maxValue = value;

		m_pAux->offsetIndices(m_numStateWeights);
	}

	return maxValue;
}

void LinearStateActionVFA::getActionValues(const State* s,double *outActionValues)
{
	if (!outActionValues)
		throw std::exception("LinearStateActionVFA::getAction Values(...) tried to get action values without providing a buffer");
	//state features in aux list
	m_pStateFeatureMap->getFeatures(s, m_pAux);

	//action-value maximization
	for (unsigned int i = 0; i < m_numActionWeights; i++)
	{
		outActionValues[i] = get(m_pAux, true); //frozen weights

		m_pAux->offsetIndices(m_numStateWeights);
	}
}