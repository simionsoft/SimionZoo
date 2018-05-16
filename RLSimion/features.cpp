#include "features.h"
#include "experiment.h"
#include "logger.h"
#include "app.h"

#define FEATURE_BLOCK_SIZE 1024
#define DEFAULT_FEATURE_THRESHOLD 0.000001


FeatureList::FeatureList(const char* pName, OverwriteMode overwriteMode)
{
	m_name = pName;
	m_numAllocFeatures = FEATURE_BLOCK_SIZE;
	m_pFeatures = new Feature[m_numAllocFeatures];
	m_numFeatures = 0;
	m_overwriteMode = overwriteMode;
}

FeatureList::~FeatureList()
{
	delete[] m_pFeatures;
}

//we only add the number of features of named feature lists (e-traces most probably)
void FeatureList::setName(const char* name)
{
	m_name = name;
	SimionApp::get()->pLogger->addVarToStats<size_t>("Features", m_name, m_numFeatures);
}
const char* FeatureList::getName()
{
	return m_name;
}

void FeatureList::clear()
{
	m_numFeatures = 0;
}

void FeatureList::resize(size_t newSize, bool bKeepFeatures)
{
	//make the newSize a multiple of the block size
	if (newSize%FEATURE_BLOCK_SIZE != 0)
		newSize += FEATURE_BLOCK_SIZE - (newSize%FEATURE_BLOCK_SIZE);

	Feature* pNewFeatures = new Feature[newSize];

	size_t oldsize = sizeof(Feature)*m_numFeatures;
	size_t newsize = sizeof(Feature)*newSize;
	if (bKeepFeatures)
		memcpy_s(pNewFeatures, sizeof(Feature)*newSize, m_pFeatures, sizeof(Feature)*m_numFeatures);
	else m_numFeatures = 0;

	delete[] m_pFeatures;
	m_pFeatures = pNewFeatures;
	m_numAllocFeatures = newSize;
}

void FeatureList::mult(double factor)
{
	for (size_t i = 0; i < m_numFeatures; i++)
	{
		m_pFeatures[i].m_factor *= factor;
	}
}


double FeatureList::getFactor(size_t index) const
{
	double factor = 0.0;
	for (size_t i = 0; i < m_numFeatures; i++)
	{
		if (m_pFeatures[i].m_index == index)
			if (m_overwriteMode == AllowDuplicates)
				factor += m_pFeatures[i].m_factor;
			else
				return m_pFeatures[i].m_factor;
	}
	return factor;
}

double FeatureList::innerProduct(const FeatureList *inList)
{
	double innerprod = 0.0;
	for (size_t i = 0; i < m_numFeatures; i++)
	{
		innerprod += m_pFeatures[i].m_factor* (inList->getFactor(m_pFeatures[i].m_index));
	}
	return innerprod;
}

void FeatureList::copyMult(double factor, const FeatureList *inList)
{
	if (m_numAllocFeatures < inList->m_numFeatures)
		resize(inList->m_numFeatures);

	m_numFeatures = inList->m_numFeatures;
	for (size_t i = 0; i < m_numFeatures; i++)
	{
		m_pFeatures[i].m_factor = inList->m_pFeatures[i].m_factor * factor;
		m_pFeatures[i].m_index = inList->m_pFeatures[i].m_index;
	}
}

void FeatureList::addFeatureList(const FeatureList *inList, double factor)
{
	//if (m_numFeatures+inList->m_numFeatures >m_numAllocFeatures)
	//	resize (m_numFeatures+inList->m_numFeatures);

	for (size_t i = 0; i < inList->m_numFeatures; i++)
	{
		add(inList->m_pFeatures[i].m_index, inList->m_pFeatures[i].m_factor*factor);
	}
}

long long FeatureList::getFeaturePos(size_t index)
{
	for (size_t i = 0; i < m_numFeatures; i++)
	{
		if (m_pFeatures[i].m_index == index) return i;
	}
	return -1;
}

void FeatureList::add(size_t index, double value)
{
	bool bCheckIfExists = m_overwriteMode != OverwriteMode::AllowDuplicates;
	size_t pos;
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

long long FeatureList::maxFactorFeature()
{
	double maxFactor = std::numeric_limits<double>::lowest();
	long long maxFactorFeature = -1;
	for (size_t i = 0; i < m_numFeatures; i++)
	{
		if (m_pFeatures[i].m_factor > maxFactor)
		{
			maxFactorFeature = m_pFeatures[i].m_index;
			maxFactor = m_pFeatures[i].m_factor;
		}
	}
	return maxFactorFeature;
}

//spawn: all features (indices and values) are spawned by those in inList
void FeatureList::spawn(const FeatureList *inList, size_t indexOffset)
{
	size_t newNumFeatures = inList->m_numFeatures * m_numFeatures;

	if (m_numAllocFeatures < newNumFeatures)
		resize(newNumFeatures);

	long long i = 0, pos = newNumFeatures - 1;
	long long j = 0;

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

void FeatureList::applyThreshold(double threshold)
{
	size_t oldNumFeatures = m_numFeatures;
	long long firstUnderThreshold = -1;

	for (size_t i = 0; i < oldNumFeatures; i++)
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

void FeatureList::normalize()
{
	double sum = 0.0;
	for (size_t i = 0; i < m_numFeatures; i++)
		sum += m_pFeatures[i].m_factor;

	sum = 1. / sum;

	for (size_t i = 0; i < m_numFeatures; i++)
		m_pFeatures[i].m_factor *= sum;
}

void FeatureList::copy(const FeatureList* inList)
{
	if (m_numAllocFeatures < inList->m_numFeatures)
		resize(inList->m_numFeatures, false);

	m_numFeatures = inList->m_numFeatures;

	for (size_t i = 0; i < m_numFeatures; i++)
	{
		m_pFeatures[i].m_index = inList->m_pFeatures[i].m_index;
		m_pFeatures[i].m_factor = inList->m_pFeatures[i].m_factor;
	}
}

void FeatureList::offsetIndices(size_t offset)
{
	if (offset == 0) return;
	for (size_t i = 0; i < m_numFeatures; i++)
		m_pFeatures[i].m_index += offset;
}

void FeatureList::split(FeatureList *outList1, FeatureList *outList2, size_t splitOffset) const
{
	for (size_t i = 0; i < m_numFeatures; i++)
	{
		if (m_pFeatures[i].m_index < splitOffset)
			outList1->add(m_pFeatures[i].m_index, m_pFeatures[i].m_factor);
		else
			outList2->add(m_pFeatures[i].m_index - splitOffset, m_pFeatures[i].m_factor);
	}
}


void FeatureList::multIndices(int mult)
{
	if (mult <= 1) return;
	for (size_t i = 0; i < m_numFeatures; i++)
		m_pFeatures[i].m_index *= mult;
}