#include "stdafx.h"
#include "featuremap.h"
#include "named-var-set.h"

#include "globals.h"
#include "world.h"
#include "features.h"
#include "xml-parameters.h"


#define ACTIVATION_THRESHOLD 0.0001

void CGaussianRBFGridFeatureMap::initCenterPoints(int i,const char* varName,int numCenters
					  ,double minV, double maxV,const char* distribution)
{
	int index;
	CState *pSDesc= RLSimion::g_pWorld->getStateDescriptor();
	CAction *pADesc= RLSimion::g_pWorld->getActionDescriptor();

	m_pCenters[i]= new double[numCenters];
	m_pNumCenters[i]= numCenters;

	index= pSDesc->getVarIndex(varName);
	if (index>=0)
	{
		m_variableType[i]= STATE_VARIABLE;
		m_variableIndex[i]= index;
		if (minV==maxV)
		{
			minV= pSDesc->getMin(index);
			maxV= pSDesc->getMax(index);
		}
	}
	else
	{
		m_variableType[i]= ACTION_VARIABLE;
		m_variableIndex[i]= pADesc->getVarIndex(varName);
		assert(m_variableIndex[i]>=0);
		if (minV==maxV)
		{
			minV= pADesc->getMin(index);
			maxV= pADesc->getMax(index);
		}
	}

	if (!strcmp(distribution,"linear"))
	{
		for (int j= 0; j<numCenters; j++)
			m_pCenters[i][j]= minV + (((double)j)/(numCenters-1))*(maxV-minV);
	}
	else
	{
		double normalisedPos;
		double ncenters= (double)numCenters;
		for (int j= 0; j<numCenters; j++)
		{
			normalisedPos=((double)j-ncenters*.5)/(ncenters*.5);

			if (!strcmp(distribution ,"cubic"))
				normalisedPos= pow(normalisedPos,3.0);
			else if (!strcmp(distribution, "quadratic"))
				normalisedPos= pow(normalisedPos,2.0);
			else assert(0);

			m_pCenters[i][j]= minV + (normalisedPos+1.0)*0.5*(maxV-minV);
		}
	}


}

CGaussianRBFGridFeatureMap::CGaussianRBFGridFeatureMap(tinyxml2::XMLElement* pParameters): CParamObject(pParameters)
{
	char * token= 0, *nextToken= 0;
	//char copy[MAX_STRING_SIZE];

	m_numVariables = XMLUtils::countChildren(pParameters, "RBF-Grid-Dimension");
	
	m_variableType= new short [m_numVariables];
	m_variableIndex= new int [m_numVariables];
	
	m_pNumCenters= new int [m_numVariables];
	m_pCenters= new double* [m_numVariables];

//	m_pBuffer= new CFeatureList();
	m_pVarFeatures= new CFeatureList();



	int numFeatures= 0;
	int numVar= 0;
	const char *varName;
	double min,max;
	const char* distType;
	tinyxml2::XMLElement* dimension= pParameters->FirstChildElement("RBF-Grid-Dimension");

	for (int i = 0; i < m_numVariables; i++)
	{
		varName = dimension->FirstChildElement("Variable")->GetText();
		numFeatures = XMLUtils::getConstInteger(dimension->FirstChildElement("Num-Features"));

		if (dimension->FirstChildElement("Min"))
			min = XMLUtils::getConstDouble(dimension->FirstChildElement("Min"));
		else min = RLSimion::g_pWorld->getStateDescriptor()->getMin(varName);

		if (dimension->FirstChildElement("Max"))
			max = XMLUtils::getConstDouble(dimension->FirstChildElement("Max"));
		else max = RLSimion::g_pWorld->getStateDescriptor()->getMax(varName);

		distType = dimension->FirstChildElement("Distribution")->GetText();
		initCenterPoints(i, varName, numFeatures, min, max, distType);

		dimension = dimension->NextSiblingElement("RBF-Grid-Dimension");
	}


	//pre-calculate number of features
	m_totalNumFeatures= 1;

	for (int i= 0; i<m_numVariables; i++)
		m_totalNumFeatures*= m_pNumCenters[i];

	m_maxNumActiveFeatures= 1;
	for (int i= 0; i<m_numVariables; i++)
		m_maxNumActiveFeatures*= MAX_NUM_ACTIVE_FEATURES_PER_DIMENSION;

}


CGaussianRBFGridFeatureMap::~CGaussianRBFGridFeatureMap()
{
	delete [] m_variableType;
	delete [] m_variableIndex;
	
	for (int i= 0; i<m_numVariables; i++)
		delete [] m_pCenters[i];

	delete [] m_pCenters;
	delete [] m_pNumCenters;

//	delete m_pBuffer;
	delete m_pVarFeatures;
}

int CGaussianRBFGridFeatureMap::getNumVariables(const char* configString)
{
	int count= 0, i= 0;
	if (!configString) return 0;
	while (configString[i]!=0)
	{
		if (configString[i]==',') count++;
		i++;
	}
	return count;
}

double CGaussianRBFGridFeatureMap::getDimValue(int dim, CState* s, CAction* a)
{
	assert(dim >=0 && dim <m_numVariables);
	if (m_variableType[dim]==STATE_VARIABLE)
		return s->getValue(m_variableIndex[dim]);

	return a->getValue(m_variableIndex[dim]);
}

double CGaussianRBFGridFeatureMap::getFeatureFactor(int dim, int feature,double value)
{
	double range,dist;
	assert(feature>=0 && feature<=m_pNumCenters[dim]-1);
	if (value>m_pCenters[dim][feature])
	{
		dist= value-m_pCenters[dim][feature];
		if(feature!=m_pNumCenters[dim]-1)
			range= m_pCenters[dim][feature+1]-m_pCenters[dim][feature];
		else
			range= m_pCenters[dim][feature]-m_pCenters[dim][feature-1];
	}
	else
	{
		dist= m_pCenters[dim][feature]-value;
		if(feature!=0)
			range= m_pCenters[dim][feature]-m_pCenters[dim][feature-1];
		else
			range= m_pCenters[dim][1]-m_pCenters[dim][0];
	}

	//f_gauss(x)= a*exp(-(x-b)^2 / 2c^2 )
	//instead of 2c^2, we use the distance to the next feature
	double f= 2*dist/range;
	double factor= exp(- (f*f));
	return factor;

	//the original feature factor calculation code in RLToolbox:
	//double distance = fabs(difference);
	//return my_exp(- pow(distance / diffNextPart * 2, 2)) ;
}

void CGaussianRBFGridFeatureMap::getDimFeatures (int dim, double value, CFeatureList* outDimFeatures)
{
	double u;
	int i;

	outDimFeatures->clear();

	unsigned int numCenters= m_pNumCenters[dim];

	assert(numCenters>=2);

	if (value<=m_pCenters[dim][1])
	{
		outDimFeatures->add(0,getFeatureFactor(dim,0,value));
		outDimFeatures->add(1,getFeatureFactor(dim,1,value));
		outDimFeatures->add(2,getFeatureFactor(dim,2,value));
	}
	else if (value>=m_pCenters[dim][numCenters-2])
	{
		outDimFeatures->add(numCenters-3,getFeatureFactor(dim,numCenters-3,value));
		outDimFeatures->add(numCenters-2,getFeatureFactor(dim,numCenters-2,value));
		outDimFeatures->add(numCenters-1,getFeatureFactor(dim,numCenters-1,value));
	}
	else
	{
		i= 1;

		while (value>m_pCenters[dim][i+1]) i++;

		u= (value-m_pCenters[dim][i])/(m_pCenters[dim][i+1]-m_pCenters[dim][i]);

		if (u<0.5)
		{
			outDimFeatures->add(i,getFeatureFactor(dim,i,value));
			outDimFeatures->add(i+1,getFeatureFactor(dim,i+1,value));
		}
		else
		{
			outDimFeatures->add(i+1,getFeatureFactor(dim,i+1,value));
			outDimFeatures->add(i,getFeatureFactor(dim,i,value));
		}

		if (value-m_pCenters[dim][i-1] < m_pCenters[dim][i+2]-value)
			outDimFeatures->add(i-1,getFeatureFactor(dim,i-1,value));
		else
			outDimFeatures->add(i+2,getFeatureFactor(dim,i+2,value));
	}
	outDimFeatures->applyThreshold(ACTIVATION_THRESHOLD);
	outDimFeatures->normalize();
}


void CGaussianRBFGridFeatureMap::getFeatures(CState* s,CAction* a,CFeatureList* outFeatures)
{
	unsigned int offset= 1;

	outFeatures->clear();
	//features of the 0th variable in m_pBuffer
	getDimFeatures(0,getDimValue(0,s,a),outFeatures);
	
	for (int i= 1; i<m_numVariables; i++)
	{
		offset*= m_pNumCenters[i-1];

		//we calculate the features of i-th variable
		getDimFeatures(i,getDimValue(i,s,a),m_pVarFeatures);
		//spawn features in buffer with the i-th variable's features
		outFeatures->spawn(m_pVarFeatures,offset);
	}
	outFeatures->applyThreshold(ACTIVATION_THRESHOLD);
	outFeatures->normalize();
}

void CGaussianRBFGridFeatureMap::getFeatureStateAction(unsigned int feature, CState* s, CAction* a)
{
	unsigned int dimFeature;

	for (int i= 0; i<m_numVariables; i++)
	{
		dimFeature= feature % m_pNumCenters[i];

		if (m_variableType[i]==STATE_VARIABLE)
			s->setValue(m_variableIndex[i],m_pCenters[i][dimFeature]);
		else a->setValue(m_variableIndex[i],m_pCenters[i][dimFeature]);

		feature= feature/m_pNumCenters[i];
	}
}
