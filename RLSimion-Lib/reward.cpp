#include "stdafx.h"
#include "reward.h"
#include "named-var-set.h"
#include "parameterized-object.h"
#include "parameters.h"
#include "world.h"
#include "globals.h"
#include "parameters-numeric.h"

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
	CNumericValue *m_pTolerance;
	CNumericValue *m_pWeight;
	double m_lastReward;
public:
	CRewardFunctionComponent(CParameters* pParameters);
	~CRewardFunctionComponent();

//	void init(CParameters* pParameters/*,int componentIndex*/);
	double getRewardComponent(CState *state);
	double getLastRewardComponent(){return m_lastReward;}
	const char* getName();
};

CLASS_CONSTRUCTOR(CRewardFunctionComponent) : CParamObject(pParameters)
{
	//m_errorComponentType[0]= 0;
	//m_controlledVariable[0]= 0;
	//m_setpointVariable[0]= 0;
	//m_controlErrorVariable[0]= 0;
	//m_weight= 0.0;
	//m_componentIndex= -1;
	STATE_VARIABLE_REF(m_sVariable, "Variable","Input state variable");
	const char* varName = pParameters->getConstString("Variable");
	sprintf_s(m_name, MAX_REWARD_NAME_SIZE, "r//%s)", varName);
	
	NUMERIC_VALUE(m_pTolerance,"Tolerance","Tolerance value: errors greater will result in a negative reward");
	NUMERIC_VALUE(m_pWeight,"Weight","Weight of this reward function component (in case more than one is used)");
	m_lastReward= 0.0;
	END_CLASS();
}

CRewardFunctionComponent::~CRewardFunctionComponent()
{
	delete m_pTolerance;
	delete m_pWeight;
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


CLASS_CONSTRUCTOR_NEW_WINDOW(CRewardFunction) : CParamObject(pParameters)
{
	CONST_DOUBLE_VALUE(m_minReward,"Min",-100.0,"Minimum output of the reward function");
	CONST_DOUBLE_VALUE(m_maxReward,"Max",1.0,"Maximum output of the reward function");

	m_lastReward= 0.0;

	m_numRewardComponents = m_pParameters->countChildren("Reward-Component");

	//the vector of reward functions
	CParameters* pRewardComponents = m_pParameters->getChild("Reward-Component");
	m_pRewardComponents = new CRewardFunctionComponent*[m_numRewardComponents];
	//the reward vector (named variables)
	m_pReward = new CReward(m_numRewardComponents);

	for (int i= 0; i<m_numRewardComponents; i++)
	{
		MULTI_VALUED(m_pRewardComponents[i], "Reward-Component", "A component of the reward function",CRewardFunctionComponent, pRewardComponents);
		m_pReward->setName(i, m_pRewardComponents[i]->getName());

		pRewardComponents = pRewardComponents->getNextChild("Reward-Component");
	}
	END_CLASS();
}

CRewardFunction::~CRewardFunction()
{
	for (int i = 0; i < m_numRewardComponents; i++) delete m_pRewardComponents[i];
	delete [] m_pRewardComponents;
	delete m_pReward;
}


double CRewardFunction::calculateReward(CState *s, CAction *a, CState *s_p)
{
	for (int i= 0; i<m_numRewardComponents; i++)
	{
		m_pReward->setValue(i, m_pRewardComponents[i]->getRewardComponent(s_p));
	}

	//rew*= (m_maxReward-m_minReward);
	//rew+= m_minReward;

	return m_pReward->getSumValue();
}

CReward* CRewardFunction::getReward()
{
	return m_pReward;
}