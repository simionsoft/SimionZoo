/*
	SimionZoo: A framework for online model-free Reinforcement Learning on continuous
	control problems

	Copyright (c) 2016 SimionSoft. https://github.com/simionsoft

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

#include "features.h"
#include "experiment.h"
#include "logger.h"
#include "app.h"
#include "../../tools/System/CrossPlatform.h"

#include <cmath>

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


/// <summary>
/// Resizes the feature list, allocating more memory
/// </summary>
/// <param name="newSize">New size</param>
/// <param name="bKeepFeatures">true if we want to preserve the features on the list</param>
void FeatureList::resize(size_t newSize, bool bKeepFeatures)
{
	//make the newSize a multiple of the block size
	if (newSize%FEATURE_BLOCK_SIZE != 0)
		newSize += FEATURE_BLOCK_SIZE - (newSize%FEATURE_BLOCK_SIZE);

	Feature* pNewFeatures = new Feature[newSize];

	if (bKeepFeatures)
		CrossPlatform::Memcpy_s(pNewFeatures, sizeof(Feature)*newSize, m_pFeatures, sizeof(Feature)*m_numFeatures);
	else m_numFeatures = 0;

	delete[] m_pFeatures;
	m_pFeatures = pNewFeatures;
	m_numAllocFeatures = newSize;
}

/// <summary>
/// Multiplies all the features by a factor
/// </summary>
/// <param name="factor">The factor value</param>
void FeatureList::mult(double factor)
{
	for (size_t i = 0; i < m_numFeatures; i++)
	{
		m_pFeatures[i].m_factor *= factor;
	}
}

/// <summary>
/// Returns the factor of a given feature on the list
/// </summary>
/// <param name="index">The index of the feature</param>
/// <returns>The factor of the feature</returns>
double FeatureList::getFactor(size_t index) const
{
	double factor = 0.0;
	for (size_t i = 0; i < m_numFeatures; i++)
	{
		if (m_pFeatures[i].m_index == index)
		{
			if (m_overwriteMode == AllowDuplicates)
				factor += m_pFeatures[i].m_factor;
			else
				return m_pFeatures[i].m_factor;
		}
	}
	return factor;
}

/// <summary>
/// Implements an inner-product operation
/// </summary>
/// <param name="inList">Second operand of the multiply operation</param>
/// <returns>The result of the inner-product</returns>
double FeatureList::innerProduct(const FeatureList *inList)
{
	double innerprod = 0.0;
	for (size_t i = 0; i < m_numFeatures; i++)
	{
		innerprod += m_pFeatures[i].m_factor* (inList->getFactor(m_pFeatures[i].m_index));
	}
	return innerprod;
}

/// <summary>
/// Copies a list multiplied by a factor on this feature list. Resizes the list if needed
/// </summary>
/// <param name="factor">The factor to multiply by</param>
/// <param name="inList">The feature list to copy</param>
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

/// <summary>
/// Adds feature to this list, multiplied by a factor
/// </summary>
/// <param name="inList">Feature list to be added</param>
/// <param name="factor">Factor used to multiply</param>
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


/// <summary>
/// Adds a single feature
/// </summary>
/// <param name="index">The index of the feature</param>
/// <param name="value">The value of the feature</param>
void FeatureList::add(size_t index, double value)
{
	bool bCheckIfExists = m_overwriteMode != OverwriteMode::AllowDuplicates;
	long long pos;
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

/// <summary>
/// All features (indices and values) are spawned by those in inList. This means that this list contains 2 features
/// a 5-feature space, and inList contains 3 features a 6-feature space, after this operation, this list will contain
/// 2*3= 6 features from a 5*6= 30 feature space
/// </summary>
/// <param name="inList">Second list used as an operand</param>
/// <param name="indexOffset">Feature-index offset used for the second list</param>
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

/// <summary>
/// Removes any feature with an activation factor under the threshold
/// </summary>
/// <param name="threshold">Threshold value</param>
void FeatureList::applyThreshold(double threshold)
{
	size_t oldNumFeatures = m_numFeatures;
	long long firstUnderThreshold = -1;

	for (size_t i = 0; i < oldNumFeatures; i++)
	{
		if (abs(m_pFeatures[i].m_factor) < abs(threshold))
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

/// <summary>
/// Normalizes features so that the sum of all the activation factors are 1
/// </summary>
void FeatureList::normalize()
{
	double sum = 0.0;
	for (size_t i = 0; i < m_numFeatures; i++)
		sum += m_pFeatures[i].m_factor;

	sum = 1. / sum;

	for (size_t i = 0; i < m_numFeatures; i++)
		m_pFeatures[i].m_factor *= sum;
}


/// <summary>
/// Copies in this list the given one
/// </summary>
/// <param name="inList">Source feature list to copy</param>
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

/// <summary>
/// Adds an offset to all the feature indices
/// </summary>
/// <param name="offset">Offset value</param>
void FeatureList::offsetIndices(size_t offset)
{
	if (offset == 0) return;
	for (size_t i = 0; i < m_numFeatures; i++)
		m_pFeatures[i].m_index += offset;
}


/// <summary>
/// Splits this feature list in two lists: features with an index below splitOffset go to the first output list,
/// and those above go to the second output list
/// </summary>
/// <param name="outList1">Output list 1</param>
/// <param name="outList2">Output list 2</param>
/// <param name="splitOffset">Index used to split the feature list</param>
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

/// <summary>
/// Multiplies all the feature indices by mult
/// </summary>
/// <param name="mult">Value used to multiply</param>
void FeatureList::multIndices(int mult)
{
	if (mult <= 1) return;
	for (size_t i = 0; i < m_numFeatures; i++)
		m_pFeatures[i].m_index *= mult;
}