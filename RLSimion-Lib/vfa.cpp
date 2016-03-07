#include "stdafx.h"
#include "vfa.h"
#include "featuremap.h"
#include "features.h"
#include "parameters.h"


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

void CLinearVFA::save(void* pFile)
{
	assert(m_pWeights && m_numWeights >= 0);

	fwrite(&m_numWeights, sizeof(unsigned int), 1, (FILE*)pFile);
	fwrite(m_pWeights, sizeof(double), m_numWeights, (FILE*)pFile);
}

void CLinearVFA::load(void* pFile)
{
	unsigned int numWeightsRead;
	fread_s(&numWeightsRead, sizeof(unsigned int), sizeof(unsigned int), 1, (FILE*)pFile);

	assert(m_numWeights == numWeightsRead);

	fread_s(m_pWeights, numWeightsRead*sizeof(double), sizeof(double), numWeightsRead, (FILE*)pFile);
}


//STATE VFA: V(s), pi(s), .../////////////////////////////////////////////////////////////////////

CLinearStateVFA::CLinearStateVFA(CParameters* pParameters) : CParamObject(pParameters), CLinearVFA()
{
	m_pStateFeatureMap = CFeatureMap::getInstance(pParameters->getChild("State-Feature-Map"));

	m_numWeights = m_pStateFeatureMap->getTotalNumFeatures();
	m_pWeights = new double[m_numWeights];

	m_pAux = new CFeatureList("LinearStateVFA\\aux");

	double initValue = 0.0;
	for (unsigned int i = 0; i < m_numWeights; i++)
		m_pWeights[i] = initValue;
	//std::fill_n(m_pWeights, m_numWeights, initValue);

	m_bSaturateOutput = false;
	m_minOutput = 0.0;
	m_maxOutput = 0.0;
}

CLinearStateVFA::~CLinearStateVFA()
{
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

CLinearStateActionVFA::CLinearStateActionVFA(CParameters* pParameters) : CParamObject(pParameters)
{
	m_pStateFeatureMap = CFeatureMap::getInstance(pParameters->getChild("State-Feature-Map"));
	m_pActionFeatureMap = CFeatureMap::getInstance(pParameters->getChild("Action-Feature-Map"));

	m_numStateWeights = m_pStateFeatureMap->getTotalNumFeatures();
	m_numActionWeights = m_pActionFeatureMap->getTotalNumFeatures();
	m_numWeights = m_numStateWeights * m_numActionWeights;
	m_pWeights = new double[m_numWeights];

	m_pAux = new CFeatureList("LinearStateActionVFA\\aux");

	double initValue = 0.0;
	for (unsigned int i = 0; i < m_numWeights; i++)
		m_pWeights[i] = initValue;
	//std::fill_n(m_pWeights, m_numWeights, initValue);

	m_bSaturateOutput = false;
	m_minOutput = 0.0;
	m_maxOutput = 0.0;
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