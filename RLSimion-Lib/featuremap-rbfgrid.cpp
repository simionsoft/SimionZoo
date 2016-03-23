#include "stdafx.h"
#include "featuremap.h"
#include "named-var-set.h"
#include "globals.h"
#include "world.h"
#include "features.h"
#include "parameters.h"
#include "single-dimension-grid.h"

#define ACTIVATION_THRESHOLD 0.0001


CGaussianRBFGridFeatureMap::CGaussianRBFGridFeatureMap(CParameters* pParameters) : CFeatureMap(pParameters)
{
	m_numDimensions = 0;
	m_pGrid = 0;
}

CLASS_CONSTRUCTOR_NEW_WINDOW(CGaussianRBFStateGridFeatureMap) : CGaussianRBFGridFeatureMap(pParameters)
{
	m_pVarFeatures= new CFeatureList("RBFGrid/var");

	m_numDimensions = pParameters->countChildren("RBF-Grid-Dimension");
	m_pGrid = new CSingleDimensionGrid*[m_numDimensions];

	CParameters* dimension = pParameters->getChild("RBF-Grid-Dimension");
	for (int i = 0; i < m_numDimensions; i++)
	{
		MULTI_VALUED(m_pGrid[i], "RBF-Grid-Dimension", "Parameters of the state-dimension's grid",CStateVariableGrid,dimension);

		dimension= dimension->getNextChild("RBF-Grid-Dimension");
	}

	//pre-calculate number of features
	m_totalNumFeatures= 1;

	for (int i = 0; i < m_numDimensions; i++)
		m_totalNumFeatures *= m_pGrid[i]->getNumCenters();// m_pNumCenters[i];

	m_maxNumActiveFeatures= 1;
	for (int i = 0; i<m_numDimensions; i++)
		m_maxNumActiveFeatures*= MAX_NUM_ACTIVE_FEATURES_PER_DIMENSION;
	END_CLASS();
}

CLASS_CONSTRUCTOR(CGaussianRBFActionGridFeatureMap) : CGaussianRBFGridFeatureMap(pParameters)
{
	m_pVarFeatures = new CFeatureList("RBFGrid/var");

	m_numDimensions = pParameters->countChildren("RBF-Grid-Dimension");
	m_pGrid = new CSingleDimensionGrid*[m_numDimensions];

	CParameters* dimension = pParameters->getChild("RBF-Grid-Dimension");
	for (int i = 0; i < m_numDimensions; i++)
	{
		MULTI_VALUED(m_pGrid[i], "RBF-Grid-Dimension", "Parameters of the action-dimension's grid", CActionVariableGrid, dimension);

		dimension = dimension->getNextChild("RBF-Grid-Dimension");
	}

	//pre-calculate number of features
	m_totalNumFeatures = 1;

	for (int i = 0; i < m_numDimensions; i++)
		m_totalNumFeatures *= m_pGrid[i]->getNumCenters();// m_pNumCenters[i];

	m_maxNumActiveFeatures = 1;
	for (int i = 0; i<m_numDimensions; i++)
		m_maxNumActiveFeatures *= MAX_NUM_ACTIVE_FEATURES_PER_DIMENSION;
	END_CLASS();
}


CGaussianRBFGridFeatureMap::~CGaussianRBFGridFeatureMap()
{
	for (int i = 0; i < m_numDimensions; i++)
		delete m_pGrid[i];
	delete [] m_pGrid;

	delete m_pVarFeatures;
}


void CGaussianRBFGridFeatureMap::getFeatures(const CState* s,const CAction* a,CFeatureList* outFeatures)
{
	unsigned int offset= 1;

	outFeatures->clear();
	if (m_numDimensions == 0) return;
	//features of the 0th variable in m_pBuffer
	m_pGrid[0]->getFeatures(s,a,outFeatures);
	
	for (int i= 1; i<m_numDimensions; i++)
	{
		offset *= m_pGrid[i - 1]->getNumCenters();

		//we calculate the features of i-th variable
		m_pGrid[i]->getFeatures(s,a,m_pVarFeatures);
		//spawn features in buffer with the i-th variable's features
		outFeatures->spawn(m_pVarFeatures,offset);
	}
	outFeatures->applyThreshold(ACTIVATION_THRESHOLD);
	outFeatures->normalize();
}

void CGaussianRBFGridFeatureMap::getFeatureStateAction(unsigned int feature, CState* s, CAction* a)
{
	unsigned int dimFeature;

	for (int i= 0; i<m_numDimensions; i++)
	{
		dimFeature= feature % m_pGrid[i]->getNumCenters();

		m_pGrid[i]->setFeatureStateAction(dimFeature,s, a);

		feature = feature / m_pGrid[i]->getNumCenters();
	}
}
