#include "vfa.h"
#include "featuremap.h"
#include "features.h"
#include "logger.h"
#include "app.h"
#include "SimGod.h"
#include "experiment.h"
#include <assert.h>
#include <algorithm>
#include "mem-manager.h"

//LINEAR VFA. Common functionalities: getSample (FeatureList*), saturate, save, load, ....
LinearVFA::LinearVFA(MemManager<SimionMemPool>* pMemManager)
{
	m_pMemManager = pMemManager;
	m_pPendingUpdates = new FeatureList("Pending-vfa-updates", OverwriteMode::AllowDuplicates);
}

LinearVFA::~LinearVFA()
{
	if (m_pPendingUpdates)
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

	if (!bUseFrozenWeights || !m_bCanBeFrozen || SimionApp::get()->pSimGod->getTargetFunctionUpdateFreq() == 0)
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

void LinearVFA::set(size_t feature, double value)
{
	(*m_pWeights)[feature] = value;
}


//STATE VFA: V(s), pi(s), .../////////////////////////////////////////////////////////////////////

LinearStateVFA::LinearStateVFA(ConfigNode* pConfigNode)
	:LinearVFA(m_pMemManager)
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
	m_pWeights = m_pMemManager->getMemBuffer(m_numWeights);
	m_pWeights->setInitValue(m_initValue.get());

	//frozen weights
	if (m_bCanBeFrozen)
	{
		m_pFrozenWeights = m_pMemManager->getMemBuffer(m_numWeights);
		m_pFrozenWeights->setInitValue(m_initValue.get());
	}
}

void LinearStateVFA::setInitValue(double initValue)
{
	m_initValue.set(initValue);
}

LinearStateVFA::LinearStateVFA(MemManager<SimionMemPool>* pMemManager, std::shared_ptr<StateFeatureMap> stateFeatureMap)
	:LinearVFA(pMemManager)
{
	m_pStateFeatureMap = stateFeatureMap;
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
	m_pStateFeatureMap->getFeatures(s, nullptr,outFeatures);
	outFeatures->offsetIndices(m_minIndex);
}

void LinearStateVFA::getFeatureState(size_t feature, State* s)
{
	if (feature>=m_minIndex && feature<m_maxIndex)
		m_pStateFeatureMap->getFeatureStateAction(feature, nullptr ,s);
}

double LinearStateVFA::get(const State *s)
{
	getFeatures(s, m_pAux);

	return LinearVFA::get(m_pAux);
}

unsigned int LinearStateVFA::getNumOutputs()
{
	return 1;
}

vector<double>& LinearStateVFA::evaluate(const State* s, const Action* a)
{
	m_output[0] = get(s);
	return m_output;
}
const vector<string>& LinearStateVFA::getInputStateVariables()
{
	return m_pStateFeatureMap->getInputStateVariables();
}
const vector<string>& LinearStateVFA::getInputActionVariables()
{
	return m_pStateFeatureMap->getInputActionVariables();
}




//STATE-ACTION VFA: Q(s,a), A(s,a), .../////////////////////////////////////////////////////////////////////

LinearStateActionVFA::LinearStateActionVFA(MemManager<SimionMemPool>* pMemManager, std::shared_ptr<StateFeatureMap> pStateFeatureMap, std::shared_ptr<ActionFeatureMap> pActionFeatureMap)
	:LinearVFA(pMemManager)
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
	:LinearStateActionVFA(SimionApp::get()->pMemManager, SimGod::getGlobalStateFeatureMap(),SimGod::getGlobalActionFeatureMap())
{
	m_initValue= DOUBLE_PARAM(pConfigNode, "Init-Value","The initial value given to the weights on initialization", 0.0);
}

LinearStateActionVFA::LinearStateActionVFA(LinearStateActionVFA* pSourceVFA)
	: LinearStateActionVFA(SimionApp::get()->pMemManager, SimGod::getGlobalStateFeatureMap(), SimGod::getGlobalActionFeatureMap())
{
	m_initValue = pSourceVFA->m_initValue;
}

LinearStateActionVFA::~LinearStateActionVFA()
{
	//SimGod owns the feature maps -> his responsability to free memory
	if (m_pAux) delete m_pAux;
	if (m_pAux2) delete m_pAux2;

	if (m_pArgMaxTies) delete [] m_pArgMaxTies;
}

void LinearStateActionVFA::setInitValue(double initValue)
{
	m_initValue.set(initValue);
}

void LinearStateActionVFA::deferredLoadStep()
{
	//weights
	m_pWeights= m_pMemManager->getMemBuffer(m_numWeights);
	m_pWeights->setInitValue(m_initValue.get());

	//frozen weights
	if (m_bCanBeFrozen)
	{
		m_pFrozenWeights = m_pMemManager->getMemBuffer(m_numWeights);
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
		m_pStateFeatureMap->getFeatures(s, nullptr, outFeatures);

		m_pActionFeatureMap->getFeatures(nullptr, a, m_pAux2);

		outFeatures->spawn(m_pAux2, (unsigned int) m_numStateWeights);

		outFeatures->offsetIndices((int) m_minIndex);
	}
	else if (s)
	{
		//if we want the state features only, no action values will be provided and the action should not be taken into account
		m_pStateFeatureMap->getFeatures(s, a, outFeatures);
	}
	else
		Logger::logMessage(MessageType::Error, "LinearStateActionVFA::getFeatures() called with neither a state nor an action");
}

void LinearStateActionVFA::getFeatureStateAction(size_t feature, State* s, Action* a)
{
	if (feature >= m_minIndex && feature < m_maxIndex)
	{
		if (s)
			m_pStateFeatureMap->getFeatureStateAction(feature % m_numStateWeights, s, nullptr);
		if (a)
			m_pActionFeatureMap->getFeatureStateAction(feature / (unsigned int) m_numStateWeights, nullptr, a);
	}
}



double LinearStateActionVFA::get(const State *s, const Action* a)
{
	getFeatures(s, a, m_pAux);

	return LinearVFA::get(m_pAux);
}



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
	m_pActionFeatureMap->getFeatureStateAction(arg, nullptr, a);
}

double LinearStateActionVFA::max(const State* s, bool bUseFrozenWeights)
{
	//state features in aux list
	m_pStateFeatureMap->getFeatures(s, nullptr, m_pAux);

	double value = 0.0;
	double maxValue = std::numeric_limits<double>::lowest();

	//action-value maximization
	for (unsigned int i = 0; i < m_numActionWeights; i++)
	{
		value = get(m_pAux, bUseFrozenWeights); //if the target is frozen, we use the frozen weights
		if (value>maxValue)
			maxValue = value;

		m_pAux->offsetIndices((int) m_numStateWeights);
	}

	return maxValue;
}

void LinearStateActionVFA::getActionValues(const State* s,double *outActionValues)
{
	if (!outActionValues)
		throw std::exception("LinearStateActionVFA::getAction Values(...) tried to get action values without providing a buffer");
	//state features in aux list
	m_pStateFeatureMap->getFeatures(s, nullptr, m_pAux);

	//action-value maximization
	for (unsigned int i = 0; i < m_numActionWeights; i++)
	{
		outActionValues[i] = get(m_pAux, true); //frozen weights

		m_pAux->offsetIndices((int) m_numStateWeights);
	}
}


unsigned int LinearStateActionVFA::getNumOutputs()
{
	return 1;
}

vector<double>& LinearStateActionVFA::evaluate(const State* s, const Action* a)
{
	m_output[0] = get(s, a);
	return m_output;
}

const vector<string>& LinearStateActionVFA::getInputStateVariables()
{
	return m_pStateFeatureMap->getInputStateVariables();
}

const vector<string>& LinearStateActionVFA::getInputActionVariables()
{
	return m_pStateFeatureMap->getInputActionVariables();
}