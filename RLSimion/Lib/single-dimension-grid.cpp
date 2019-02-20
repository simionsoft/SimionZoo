#include "single-dimension-grid.h"
#include "features.h"
#include "../Common/named-var-set.h"
#include <algorithm>

SingleDimensionGrid::SingleDimensionGrid(size_t numValues, double min, double max, bool circular)
{
	m_min = min;
	m_max = max;
	m_rangeWidth = max - min;
	m_bCircular = circular;

	m_values = vector<double>(numValues);

	if (!circular)
	{
		for (int i = 0; i < (int) numValues; i++)
			m_values[i] = m_min + (((double)i) / (double)(numValues - 1))*(m_rangeWidth);
	}
	else
	{
		//if the variable is circular, the distance between the n points is calculated as if they were
		//n+1 points, so that m_values[0] != m_values[numValues-1]
		for (int i = 0; i < (int) numValues; i++)
			m_values[i] = m_min + (((double)i) / (double)(numValues))*(m_rangeWidth);
	}
}

SingleDimensionGrid::~SingleDimensionGrid()
{
}


size_t SingleDimensionGrid::getClosestFeature(double value) const
{
	unsigned int nearestIndex = 0;
	double dist, minDist;

	if (!m_bCircular)
	{
		minDist = abs(value - m_values[0]);
		for (unsigned int i = 1; i < m_values.size(); i++)
		{
			dist = abs(value - m_values[i]);
			if ( dist < minDist )
			{
				nearestIndex = i;
				minDist = dist;
			}
		}
	}
	else
	{
		minDist = std::min(abs(value - m_values[0]), abs(m_rangeWidth + m_values[0] - value));
		for (unsigned int i = 1; i < m_values.size(); i++)
		{
			dist = abs(value - m_values[i]);
			if (dist < minDist)
			{
				nearestIndex = i;
				minDist = dist;
			}
		}
	}

	return nearestIndex;
}

double SingleDimensionGrid::getFeatureValue(size_t feature) const
{
	return m_values[feature];
}