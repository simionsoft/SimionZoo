#include "stdafx.h"
#include "vfa.h"
#include "featuremap.h"
#include "features.h"

void CFeatureVFA::getFeatures(CState* s,CAction* a,CFeatureList* outFeatures)
{
	assert (s);
	assert (outFeatures);
	m_pFeatureMap->getFeatures(s,a,outFeatures);
}

void CFeatureVFA::getFeatureStateAction(unsigned int feature, CState* s, CAction* a)
{
	assert(feature>=0 && feature<m_pFeatureMap->getTotalNumFeatures());
	m_pFeatureMap->getFeatureStateAction(feature,s,a);
}


void CFeatureVFA::add(CFeatureList* pFeatures,double alpha)
{
	assert(pFeatures);
	//double sum= 0.0;
	for (unsigned int i= 0; i<pFeatures->m_numFeatures; i++)
	{
		assert(pFeatures->m_pFeatures[i].m_index<m_numWeights);
		//assert(pFeatures->m_pFeatures[i].m_factor>=0.0 && pFeatures->m_pFeatures[i].m_factor<=1.0);
		//sum+= alpha* pFeatures->m_pFeatures[i].m_factor;
		m_pWeights[pFeatures->m_pFeatures[i].m_index]+= alpha* pFeatures->m_pFeatures[i].m_factor;
	}
}

void CFeatureVFA::save(void* pFile)
{
	assert(m_pWeights && m_numWeights>=0);

	fwrite(&m_numWeights,sizeof(unsigned int),1,(FILE*)pFile);
	fwrite(m_pWeights,sizeof(double),m_numWeights,(FILE*)pFile);
}

void CFeatureVFA::load(void* pFile)
{
	unsigned int numWeightsRead;
	fread_s(&numWeightsRead,sizeof(unsigned int),sizeof(unsigned int),1,(FILE*)pFile);
	
	assert(m_numWeights==numWeightsRead);

	fread_s(m_pWeights,numWeightsRead*sizeof(double),sizeof(double),numWeightsRead,(FILE*)pFile);
}


CRBFFeatureGridVFA::CRBFFeatureGridVFA(const char* configString)
{
	m_pFeatureMap= new CGaussianRBFGridFeatureMap(configString);

	m_numWeights= m_pFeatureMap->getTotalNumFeatures();
	m_pWeights= new double[m_numWeights];
	memset(m_pWeights,0,sizeof(double)*m_numWeights);
}

CRBFFeatureGridVFA::~CRBFFeatureGridVFA()
{
	delete m_pFeatureMap;

	delete [] m_pWeights;
}

double CRBFFeatureGridVFA::getValue(CFeatureList *pFeatures)
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