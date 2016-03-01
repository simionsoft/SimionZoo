#include "stdafx.h"
#include "reward.h"
#include "named-var-set.h"
#include "parameterized-object.h"
#include "xml-parameters.h"
#include "world.h"
#include "globals.h"

#define NUM_MAX_REWARD_COMPONENTS 10

#define VARIABLE_DIFFERENCE 0
#define DEVIATION_VARIABLE 1
#define CONSTANT_DIFFERENCE 2
#define PUNISH_IF_ABOVE 3
#define PUNISH_IF_BELOW 4

double CRewardFunction::m_minReward;
double CRewardFunction::m_maxReward;
#define MAX_REWARD_NAME_SIZE 128
class CRewardFunctionComponent : public CParamObject
{
	char m_name[MAX_REWARD_NAME_SIZE];
	int m_sVariable;
	INumericValue *m_pTolerance;
	INumericValue *m_pWeight;
	double m_lastReward;
public:
	CRewardFunctionComponent(tinyxml2::XMLElement* pParameters);
	~CRewardFunctionComponent();

//	void init(tinyxml2::XMLElement* pParameters/*,int componentIndex*/);
	double getRewardComponent(CState *state);
	double getLastRewardComponent(){return m_lastReward;}
	const char* getName();
};

CRewardFunctionComponent::CRewardFunctionComponent(tinyxml2::XMLElement* pParameters) : CParamObject(pParameters)
{
	//m_errorComponentType[0]= 0;
	//m_controlledVariable[0]= 0;
	//m_setpointVariable[0]= 0;
	//m_controlErrorVariable[0]= 0;
	//m_weight= 0.0;
	//m_componentIndex= -1;
	const char* varName = XMLParameters::getConstString(pParameters->FirstChildElement("Variable"));
	sprintf_s(m_name, MAX_REWARD_NAME_SIZE, "r(%s)", varName);
	m_sVariable = CWorld::m_pDynamicModel->getStateDescriptor()->getVarIndex(varName);
	m_pTolerance = XMLParameters::getNumericHandler(pParameters->FirstChildElement("Tolerance"));
	m_pWeight = XMLParameters::getNumericHandler(pParameters->FirstChildElement("Weight"));
	m_lastReward= 0.0;
}

CRewardFunctionComponent::~CRewardFunctionComponent()
{
}

const char* CRewardFunctionComponent::getName()
{
	return m_name;
}



double CRewardFunctionComponent::getRewardComponent(CState* state)
{
	double rew,error;

	error = state->getValue(m_sVariable);


	error= (error)/(m_pTolerance->getValue());

	rew = CRewardFunction::m_maxReward - fabs(error);
	//rew= exp(-(error*error));
	


	//rew = CReward::m_maxReward - fabs(error);// *error;

	//rew= m_weight*rew;

	//rew= std::max(CReward::m_minReward,rew);

	m_lastReward= rew;

	return rew;
}


CRewardFunction::CRewardFunction(tinyxml2::XMLElement* pParameters) : CParamObject(pParameters)
{
	m_minReward= XMLParameters::getConstDouble(pParameters->FirstChildElement("Min"));
	m_maxReward = XMLParameters::getConstDouble(pParameters->FirstChildElement("Max"));

	m_lastReward= 0.0;

	tinyxml2::XMLElement* pRewardComponents = m_pParameters->FirstChildElement("Reward-components");

	m_numRewardComponents = XMLParameters::countChildren(pRewardComponents);

	//the vector of reward functions
	m_pRewardComponents = new CRewardFunctionComponent*[m_numRewardComponents];
	//the reward vector (named variables)
	m_pReward = new CReward(m_numRewardComponents);

	tinyxml2::XMLElement* pComponent = pRewardComponents->FirstChildElement();
	for (int i= 0; i<m_numRewardComponents; i++)
	{
		m_pRewardComponents[i] = new CRewardFunctionComponent(pComponent);
		m_pReward->setName(i, m_pRewardComponents[i]->getName());

		pComponent = pComponent->NextSiblingElement();
	}
}

CRewardFunction::~CRewardFunction()
{
	delete [] m_pRewardComponents;
}


double CRewardFunction::calculateReward(CState *s, CAction *a, CState *s_p)
{
	double rew = 0.0;

	for (int i= 0; i<m_numRewardComponents; i++)
	{
		rew+= m_pRewardComponents[i]->getRewardComponent(s_p);
	}

	//rew*= (m_maxReward-m_minReward);
	//rew+= m_minReward;

	m_lastReward= rew;
	return rew;
}

CReward* CRewardFunction::getReward()
{
	return m_pReward;
}