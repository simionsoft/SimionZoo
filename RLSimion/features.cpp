#include "features.h"
#include "experiment.h"
#include "logger.h"
#include "app.h"

#define FEATURE_BLOCK_SIZE 1024
#define DEFAULT_FEATURE_THRESHOLD 0.000001


CFeatureList::CFeatureList(const char* pName, OverwriteMode overwriteMode)
{
	m_name = pName;
	m_numAllocFeatures = FEATURE_BLOCK_SIZE;
	m_pFeatures = new CFeature[m_numAllocFeatures];
	m_numFeatures = 0;
	m_overwriteMode = overwriteMode;
}

CFeatureList::~CFeatureList()
{
	delete[] m_pFeatures;
}

//we only add the number of features of named feature lists (e-traces most probably)
void CFeatureList::setName(const char* name)
{
	m_name = name;
	CSimionApp::get()->pLogger->addVarToStats<unsigned int>("Features", m_name, m_numFeatures);
}
const char* CFeatureList::getName()
{
	return m_name;
}

void CFeatureList::clear()
{
	m_numFeatures = 0;
}

void CFeatureList::resize(unsigned int newSize, bool bKeepFeatures)
{
	//make the newSize a multiple of the block size
	if (newSize%FEATURE_BLOCK_SIZE != 0)
		newSize += FEATURE_BLOCK_SIZE - (newSize%FEATURE_BLOCK_SIZE);

	CFeature* pNewFeatures = new CFeature[newSize];

	unsigned int oldsize = sizeof(CFeature)*m_numFeatures;
	unsigned int newsize = sizeof(CFeature)*newSize;
	if (bKeepFeatures)
		memcpy_s(pNewFeatures, sizeof(CFeature)*newSize, m_pFeatures, sizeof(CFeature)*m_numFeatures);
	else m_numFeatures = 0;

	delete[] m_pFeatures;
	m_pFeatures = pNewFeatures;
	m_numAllocFeatures = newSize;
}

void CFeatureList::mult(double factor)
{
	for (unsigned int i = 0; i < m_numFeatures; i++)
	{
		m_pFeatures[i].m_factor *= factor;
	}
}
//
//void CFeatureList::swapFeatureLists(CFeatureList** pList1,CFeatureList** pList2)
//{
//	CFeatureList *auxList= *pList1;
//	*pList1= *pList2;
//	*pList2= auxList;
//}

double CFeatureList::getFactor(unsigned int index) const
{
	double factor = 0.0;
	for (unsigned int i = 0; i < m_numFeatures; i++)
	{
		if (m_pFeatures[i].m_index == index)
			if (m_overwriteMode == AllowDuplicates)
				factor += m_pFeatures[i].m_factor;
			else
				return m_pFeatures[i].m_factor;
	}
	return factor;
}

double CFeatureList::innerProduct(const CFeatureList *inList)
{
	double innerprod = 0.0;
	for (unsigned int i = 0; i < m_numFeatures; i++)
	{
		innerprod += m_pFeatures[i].m_factor* (inList->getFactor(m_pFeatures[i].m_index));
	}
	return innerprod;
}

void CFeatureList::copyMult(double factor, const CFeatureList *inList)
{
	if (m_numAllocFeatures < inList->m_numFeatures)
		resize(inList->m_numFeatures);

	m_numFeatures = inList->m_numFeatures;
	for (unsigned int i = 0; i < m_numFeatures; i++)
	{
		m_pFeatures[i].m_factor = inList->m_pFeatures[i].m_factor * factor;
		m_pFeatures[i].m_index = inList->m_pFeatures[i].m_index;
	}
}

void CFeatureList::addFeatureList(const CFeatureList *inList, double factor)
{
	//if (m_numFeatures+inList->m_numFeatures >m_numAllocFeatures)
	//	resize (m_numFeatures+inList->m_numFeatures);

	for (unsigned int i = 0; i < inList->m_numFeatures; i++)
	{
		add(inList->m_pFeatures[i].m_index, inList->m_pFeatures[i].m_factor*factor);
	}
}

int CFeatureList::getFeaturePos(unsigned int index)
{
	for (unsigned int i = 0; i < m_numFeatures; i++)
	{
		if (m_pFeatures[i].m_index == index) return i;
	}
	return -1;
}

void CFeatureList::add(unsigned int index, double value)
{
	bool bCheckIfExists = m_overwriteMode != OverwriteMode::AllowDuplicates;
	int pos;
	if (bCheckIfExists)
	{
		pos = getFeaturePos(index);

		if (pos >= 0)
		{
			if (m_overwriteMode == OverwriteMode::Add) m_pFeatures[pos].m_factor += value;
			else if (m_overwriteMode == OverwriteMode::Replace) m_pFeatures[pos].m_factor = value;
			return;
		}
	}
	//either we didn't find the index or we didn't look for it
	//in any case, we have to add the new feature;

	if (m_numFeatures >= m_numAllocFeatures)
		resize(m_numAllocFeatures + FEATURE_BLOCK_SIZE);

	m_pFeatures[m_numFeatures].m_factor = value;
	m_pFeatures[m_numFeatures].m_index = index;
	m_numFeatures++;
}

//spawn: all features (indices and values) are spawned by those in inList
void CFeatureList::spawn(const CFeatureList *inList, unsigned int indexOffset)
{
	unsigned int newNumFeatures = inList->m_numFeatures * m_numFeatures;

	if (m_numAllocFeatures < newNumFeatures)
		resize(newNumFeatures);

	int i = 0, pos = newNumFeatures - 1;
	int j = 0;

	for (i = m_numFeatures - 1; i >= 0; i--)
	{
		for (j = inList->m_numFeatures - 1; j >= 0; j--)
		{
			m_pFeatures[pos].m_factor = m_pFeatures[i].m_factor * inList->m_pFeatures[j].m_factor;
			m_pFeatures[pos].m_index = m_pFeatures[i].m_index + inList->m_pFeatures[j].m_index*indexOffset;
			pos--;
		}
	}
	m_numFeatures = newNumFeatures;
}

void CFeatureList::applyThreshold(double threshold)
{
	unsigned int oldNumFeatures = m_numFeatures;
	int firstUnderThreshold = -1;

	for (unsigned int i = 0; i < oldNumFeatures; i++)
	{
		if (abs(m_pFeatures[i].m_factor) < threshold)
		{
			if (firstUnderThreshold < 0) firstUnderThreshold = i;

			m_numFeatures--;
		}
		else if (firstUnderThreshold >= 0)
		{
			//move the i-th feature to the first "free" position
			m_pFeatures[firstUnderThreshold].m_factor = m_pFeatures[i].m_factor;
			m_pFeatures[firstUnderThreshold].m_index = m_pFeatures[i].m_index;
			firstUnderThreshold++;
		}
	}
}

void CFeatureList::normalize()
{
	double sum = 0.0;
	for (unsigned int i = 0; i < m_numFeatures; i++)
		sum += m_pFeatures[i].m_factor;

	sum = 1. / sum;

	for (unsigned int i = 0; i < m_numFeatures; i++)
		m_pFeatures[i].m_factor *= sum;
}

void CFeatureList::copy(const CFeatureList* inList)
{
	if (m_numAllocFeatures < inList->m_numFeatures)
		resize(inList->m_numFeatures, false);

	m_numFeatures = inList->m_numFeatures;

	for (unsigned int i = 0; i < m_numFeatures; i++)
	{
		m_pFeatures[i].m_index = inList->m_pFeatures[i].m_index;
		m_pFeatures[i].m_factor = inList->m_pFeatures[i].m_factor;
	}
}

void CFeatureList::offsetIndices(int offset)
{
	if (offset == 0) return;
	for (unsigned int i = 0; i < m_numFeatures; i++)
		m_pFeatures[i].m_index += offset;
}

void CFeatureList::split(CFeatureList *outList1, CFeatureList *outList2, unsigned int splitOffset) const
{
	for (unsigned int i = 0; i < m_numFeatures; i++)
	{
		if (m_pFeatures[i].m_index < splitOffset)
			outList1->add(m_pFeatures[i].m_index, m_pFeatures[i].m_factor);
		else
			outList2->add(m_pFeatures[i].m_index - splitOffset, m_pFeatures[i].m_factor);
	}
}


void CFeatureList::multIndices(int mult)
{
	if (mult <= 1) return;
	for (unsigned int i = 0; i < m_numFeatures; i++)
		m_pFeatures[i].m_index *= mult;
}