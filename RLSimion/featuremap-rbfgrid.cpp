#include "featuremap.h"
#include "named-var-set.h"
#include "worlds/world.h"
#include "features.h"
#include "config.h"
#include "single-dimension-grid.h"

#define ACTIVATION_THRESHOLD 0.0001


GaussianRBFGridFeatureMap::GaussianRBFGridFeatureMap(ConfigNode* pConfigNode): FeatureMap(pConfigNode)
{
	m_pVarFeatures = new FeatureList("RBFGrid/var");

	m_numFeaturesPerVariable = INT_PARAM(pConfigNode, "Num-Features-Per-Dimension", "Number of features per input variable", 20);

	//pre-calculate number of features
	m_totalNumFeatures = 1;
	m_maxNumActiveFeatures = 1;

	//state variables
	for (unsigned int i = 0; i < m_stateVariables.size(); i++)
	{
		m_totalNumFeatures *= m_numFeaturesPerVariable.get();
		m_maxNumActiveFeatures *= m_maxNumActiveFeaturesPerDimension;

		m_stateVariableNames.push_back(m_stateVariables[i]->getName());
		m_grids.push_back(new SingleDimensionGrid(m_numFeaturesPerVariable.get()
			, m_stateVariables[i]->getProperties().getMin(), m_stateVariables[i]->getProperties().getMax()
			, m_stateVariables[i]->getProperties().bIsCircular()));
	}
	//action variables
	for (unsigned int i = 0; i < m_actionVariables.size(); i++)
	{
		m_totalNumFeatures *= m_numFeaturesPerVariable.get();
		m_maxNumActiveFeatures *= m_maxNumActiveFeaturesPerDimension;

		m_actionVariableNames.push_back(m_actionVariables[i]->getName());
		m_grids.push_back(new SingleDimensionGrid(m_numFeaturesPerVariable.get()
			, m_actionVariables[i]->getProperties().getMin(), m_actionVariables[i]->getProperties().getMax()
			, m_actionVariables[i]->getProperties().bIsCircular()));
	}
}


GaussianRBFGridFeatureMap::~GaussianRBFGridFeatureMap()
{
	delete m_pVarFeatures;
}



void GaussianRBFGridFeatureMap::getFeatures(const State* s, const Action* a, FeatureList* outFeatures)
{
	unsigned int offset = 1;

	outFeatures->clear();
	if (m_grids.size() == 0) return;
	//features of the 0th variable in outFeatures
	getDimensionFeatures(0, s, a, outFeatures);

	for (unsigned int i = 1; i < m_grids.size(); i++)
	{
		offset *= m_numFeaturesPerVariable.get();

		//we calculate the features of i-th variable in m_pVarFeatures
		getDimensionFeatures(i, s, a, m_pVarFeatures);
		//spawn features in buffer with the i-th variable's features
		outFeatures->spawn(m_pVarFeatures, offset);
	}
	outFeatures->applyThreshold(ACTIVATION_THRESHOLD);
	outFeatures->normalize();
}


void GaussianRBFGridFeatureMap::getFeatureStateAction(size_t feature, State* s, Action* a)
{
	unsigned int dimFeature;

	for (unsigned int i = 0; i < m_grids.size(); i++)
	{
		dimFeature = feature % m_numFeaturesPerVariable.get();

		setInputVariableValue(i, m_grids[i]->getFeatureValue(dimFeature) , s, a);

		feature = feature / m_numFeaturesPerVariable.get();
	}
}

void GaussianRBFGridFeatureMap::getDimensionFeatures(size_t varIndex, const State* s, const Action* a, FeatureList* outFeatures) const
{
	double u;
	size_t i;

	if (varIndex<0 || varIndex>m_grids.size())
		throw exception("Invalid variable index give to GaussianRBFGridFeatureMap::getDimensionFeatures()");
	SingleDimensionGrid* pGrid = m_grids[varIndex];

	outFeatures->clear();

	size_t numCenters = pGrid->getValues().size();

	if (numCenters <= 2) return;

	double value = getInputVariableValue(varIndex, s, a);

	if (value <= pGrid->getValues()[1])
	{
		if (!pGrid->isCircular())
		{
			outFeatures->add(0, getFeatureFactor(varIndex, 0, value));
			outFeatures->add(1, getFeatureFactor(varIndex, 1, value));
			outFeatures->add(2, getFeatureFactor(varIndex, 2, value));
		}
		else
		{
			outFeatures->add(0, getFeatureFactor(varIndex, 0, value));
			outFeatures->add(1, getFeatureFactor(varIndex, 1, value));
			outFeatures->add(numCenters - 1, getFeatureFactor(varIndex, numCenters - 1, value + pGrid->getRangeWidth()));
		}
	}
	else if (value >= pGrid->getValues()[numCenters - 2])
	{
		if (!pGrid->isCircular())
		{
			outFeatures->add(numCenters - 3, getFeatureFactor(varIndex, numCenters - 3, value));
			outFeatures->add(numCenters - 2, getFeatureFactor(varIndex, numCenters - 2, value));
			outFeatures->add(numCenters - 1, getFeatureFactor(varIndex, numCenters - 1, value));
		}
		else
		{
			outFeatures->add(numCenters - 2, getFeatureFactor(varIndex, numCenters - 2, value));
			outFeatures->add(numCenters - 1, getFeatureFactor(varIndex, numCenters - 1, value));
			outFeatures->add(0, getFeatureFactor(varIndex, 0, value - pGrid->getRangeWidth()));
		}
	}
	else
	{
		i = 1;

		while (value > pGrid->getValues()[i + 1]) i++;

		u = (value - pGrid->getValues()[i]) / (pGrid->getValues()[i + 1] - pGrid->getValues()[i]);

		if (u < 0.5)
		{
			outFeatures->add(i, getFeatureFactor(varIndex, i, value));
			outFeatures->add(i + 1, getFeatureFactor(varIndex, i + 1, value));
		}
		else
		{
			outFeatures->add(i + 1, getFeatureFactor(varIndex, i + 1, value));
			outFeatures->add(i, getFeatureFactor(varIndex, i, value));
		}

		if (value - pGrid->getValues()[i - 1] < pGrid->getValues()[i + 2] - value)
			outFeatures->add(i - 1, getFeatureFactor(varIndex, i - 1, value));
		else
			outFeatures->add(i + 2, getFeatureFactor(varIndex, i + 2, value));
	}
	outFeatures->applyThreshold(ACTIVATION_THRESHOLD);
	outFeatures->normalize();
}

double GaussianRBFGridFeatureMap::getFeatureFactor(size_t varIndex, size_t feature, double value) const
{
	double range, dist;

	SingleDimensionGrid* pGrid = m_grids[varIndex];

	if (value > pGrid->getValues()[feature])
	{
		dist = value - pGrid->getValues()[feature];
		if (feature != pGrid->getValues().size() - 1)
			range = pGrid->getValues()[feature + 1] - pGrid->getValues()[feature];
		else
			range = pGrid->getValues()[feature] - pGrid->getValues()[feature - 1];
	}
	else
	{
		dist = pGrid->getValues()[feature] - value;
		if (feature != 0)
			range = pGrid->getValues()[feature] - pGrid->getValues()[feature - 1];
		else
			range = pGrid->getValues()[1] - pGrid->getValues()[0];
	}

	//f_gauss(x)= a*exp(-(x-b)^2 / 2c^2 )
	//instead of 2c^2, we use the distance to the next feature
	double f = 2 * dist / range;
	double factor = exp(-(f*f));
	return factor;
}