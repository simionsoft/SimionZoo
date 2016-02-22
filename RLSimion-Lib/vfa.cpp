#include "stdafx.h"
#include "vfa.h"
#include "featuremap.h"
#include "features.h"


CLinearVFA::CLinearVFA(tinyxml2::XMLElement* pParameters) : CParamObject(pParameters)
{
	m_pFeatureMap = CFeatureMap::getInstance(pParameters);

	m_numWeights = m_pFeatureMap->getTotalNumFeatures();
	m_pWeights = new double[m_numWeights];

	m_pAux = new CFeatureList();

	double initValue = 0.0;
	std::fill_n(m_pWeights, m_numWeights, initValue);

	m_bSaturateOutput = false;
	m_minOutput = 0.0;
	m_maxOutput = 0.0;
}

CLinearVFA::~CLinearVFA()
{
	delete m_pFeatureMap;
	delete m_pAux;

	delete[] m_pWeights;
}

void CLinearVFA::saturateOutput(double min, double max)
{
	m_bSaturateOutput = true;
	m_minOutput = min;
	m_maxOutput = max;
}

void CLinearVFA::getFeatures(CState* s, CAction* a, CFeatureList* outFeatures)
{
	assert (s);
	assert (outFeatures);
	m_pFeatureMap->getFeatures(s,a,outFeatures);
}

void CLinearVFA::getFeatureStateAction(unsigned int feature, CState* s, CAction* a)
{
	assert(feature>=0 && feature<m_pFeatureMap->getTotalNumFeatures());
	m_pFeatureMap->getFeatureStateAction(feature,s,a);
}


void CLinearVFA::add(CFeatureList* pFeatures, double alpha)
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

			assert(m_pWeights[pFeatures->m_pFeatures[i].m_index] + alpha* pFeatures->m_pFeatures[i].m_factor>=m_minOutput);
			assert(m_pWeights[pFeatures->m_pFeatures[i].m_index] + alpha* pFeatures->m_pFeatures[i].m_factor <= m_maxOutput);
			//saturate output by saturation of the weights
			m_pWeights[pFeatures->m_pFeatures[i].m_index] = 
				std::min(m_maxOutput,
					std::max(m_minOutput
							, m_pWeights[pFeatures->m_pFeatures[i].m_index] + alpha* pFeatures->m_pFeatures[i].m_factor));
		}
}

void CLinearVFA::save(void* pFile)
{
	assert(m_pWeights && m_numWeights>=0);

	fwrite(&m_numWeights,sizeof(unsigned int),1,(FILE*)pFile);
	fwrite(m_pWeights,sizeof(double),m_numWeights,(FILE*)pFile);
}

void CLinearVFA::load(void* pFile)
{
	unsigned int numWeightsRead;
	fread_s(&numWeightsRead,sizeof(unsigned int),sizeof(unsigned int),1,(FILE*)pFile);
	
	assert(m_numWeights==numWeightsRead);

	fread_s(m_pWeights,numWeightsRead*sizeof(double),sizeof(double),numWeightsRead,(FILE*)pFile);
}




double CLinearVFA::getValue(CFeatureList *pFeatures)
{
	double value= 0.0;
	assert(pFeatures);
	for (unsigned int i= 0; i<pFeatures->m_numFeatures; i++)
	{
		assert(pFeatures->m_pFeatures[i].m_index<m_numWeights);
		assert(pFeatures->m_pFeatures[i].m_factor>=0.0 && pFeatures->m_pFeatures[i].m_factor<=1.0);

		value+= m_pWeights[pFeatures->m_pFeatures[i].m_index] * pFeatures->m_pFeatures[i].m_factor;
	}
	return value;
}

double CLinearVFA::getValue(CState *s, CAction* a)
{
	getFeatures(s, a, m_pAux);
	return getValue(m_pAux);
}