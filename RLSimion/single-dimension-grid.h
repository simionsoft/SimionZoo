#pragma once
class NamedVarSet;
typedef NamedVarSet State;
typedef NamedVarSet Action;
class FeatureList;
class ConfigNode;

#include "named-var-set.h"
#include <string>
#include <vector>
using namespace std;

class SingleDimensionGrid
{
protected:
	vector<double> m_values;

	double m_min, m_max, m_rangeWidth;
	bool m_bCircular;

	SingleDimensionGrid();

public:
	SingleDimensionGrid(size_t numValues, double min, double max, bool circular = false);
	virtual ~SingleDimensionGrid();

	void initCenterPoints();

	vector<double>& getValues() { return m_values; }

	double getMin() const { return m_min; }
	double getMax() const { return m_max; }
	bool isCircular() const { return m_bCircular; }
	double getRangeWidth() const { return m_rangeWidth; }

	size_t getClosestFeature (double value) const;
	double getFeatureValue (size_t feature) const;
};
