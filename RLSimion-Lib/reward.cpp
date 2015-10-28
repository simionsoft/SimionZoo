#include "stdafx.h"
#include "reward.h"
#include "states-and-actions.h"
#include "parameters.h"


#define NUM_MAX_REWARD_COMPONENTS 10

#define VARIABLE_DIFFERENCE 0
#define DEVIATION_VARIABLE 1
#define CONSTANT_DIFFERENCE 2
#define PUNISH_IF_ABOVE 3
#define PUNISH_IF_BELOW 4


class CErrorComponent
{
	char m_errorComponentType[MAX_PARAMETER_NAME_SIZE];
	char m_controlledVariable[MAX_PARAMETER_NAME_SIZE];
	char m_setpointVariable[MAX_PARAMETER_NAME_SIZE];
	char m_controlErrorVariable[MAX_PARAMETER_NAME_SIZE];

	double m_setpointConstant;
	double m_weight;
	int m_componentIndex;
	//double m_rewardComponentMu;
	double m_tolerance;
	double m_lastReward;
public:
	CErrorComponent();
	~CErrorComponent();

	void init(CParameters* pParameters,int componentIndex);
	double getRewardComponent(CState *state);
	double getLastRewardComponent(){return m_lastReward;}
};

CErrorComponent::CErrorComponent()
{
	m_errorComponentType[0]= 0;
	m_controlledVariable[0]= 0;
	m_setpointVariable[0]= 0;
	m_controlErrorVariable[0]= 0;
	m_weight= 0.0;
	m_componentIndex= -1;
	m_lastReward= 0.0;
}

CErrorComponent::~CErrorComponent()
{
}

void CErrorComponent::init(CParameters* pParameters,int componentIndex)
{
	char parameterName[256];

	m_componentIndex= componentIndex;

	sprintf_s(parameterName,256,"WORLD/REWARD/COMPONENT_TYPE_%d",componentIndex);
	sprintf_s(m_errorComponentType,MAX_PARAMETER_NAME_SIZE,"%s",pParameters->getStringPtr(parameterName));
	
	sprintf_s(parameterName,256,"WORLD/REWARD/COMPONENT_WEIGHT_%d",componentIndex);
	m_weight= pParameters->getDouble(parameterName);

	sprintf_s(parameterName,256,"WORLD/REWARD/COMPONENT_TOLERANCE_%d",componentIndex);
	m_tolerance= pParameters->getDouble(parameterName);

	if (strcmp(m_errorComponentType,"VARIABLE_DIFFERENCE")==0)
	{
		sprintf_s(parameterName,256,"WORLD/REWARD/CONTROLLED_VARIABLE_%d",componentIndex);
		sprintf_s(m_controlledVariable,MAX_PARAMETER_NAME_SIZE,"%s",pParameters->getStringPtr(parameterName));
		sprintf_s(parameterName,256,"WORLD/REWARD/SETPOINT_VARIABLE_%d",componentIndex);
		sprintf_s(m_setpointVariable,MAX_PARAMETER_NAME_SIZE,"%s",pParameters->getStringPtr(parameterName));
	}
	else if (strcmp(m_errorComponentType,"DEVIATION_VARIABLE")==0)
	{
		sprintf_s(parameterName,256,"WORLD/REWARD/CONTROL_ERROR_VARIABLE_%d",componentIndex);
		sprintf_s(m_controlErrorVariable,MAX_PARAMETER_NAME_SIZE,"%s", pParameters->getStringPtr(parameterName));
	}
	else if ( (strcmp(m_errorComponentType,"CONSTANT_DIFFERENCE")==0)
			|| (strcmp(m_errorComponentType,"PUNISH_IF_ABOVE")==0)
			|| (strcmp(m_errorComponentType,"PUNISH_IF_BELOW")==0) )
	{
		sprintf_s(parameterName,256,"WORLD/REWARD/CONTROLLED_VARIABLE_%d",componentIndex);
		sprintf_s(m_controlledVariable,MAX_PARAMETER_NAME_SIZE,"%s",pParameters->getStringPtr(parameterName));
		sprintf_s(parameterName,256,"WORLD/REWARD/SETPOINT_CONSTANT_%d",componentIndex);
		m_setpointConstant= pParameters->getDouble(parameterName);
	}
	else assert(0);
}
#define MIN_REWARD -100.0
#define MAX_REWARD 1.0
double CErrorComponent::getRewardComponent(CState* state)
{
	double rew,error;

	if (strcmp(m_errorComponentType,"VARIABLE_DIFFERENCE")==0)
	{
		error= state->getValue(m_setpointVariable)
			- state->getValue(m_controlledVariable);
	}
	else if (strcmp(m_errorComponentType,"DEVIATION_VARIABLE")==0) 
	{
		error= state->getValue(m_controlErrorVariable);
	}
	else if (strcmp(m_errorComponentType,"CONSTANT_DIFFERENCE")==0)
	{
		error= m_setpointConstant 
			- state->getValue(m_controlledVariable);
	}
	else if (strcmp(m_errorComponentType,"PUNISH_IF_ABOVE")==0)
	{
		error= std::max(0.0,state->getValue(m_controlledVariable)
			-m_setpointConstant);
	}
	else if (strcmp(m_errorComponentType,"PUNISH_IF_BELOW")==0)
	{
		error= std::max(0.0,m_setpointConstant - state->getValue(m_controlledVariable));
	}

	error= (error)/m_tolerance;

	rew= MAX_REWARD- error*error;//fabs(error);
	/*rew= tanh(fabs(error)/m_rewardComponentMu);///w);
	rew*= rew;

	rew= m_weight*(1.0-rew);*/

	rew= m_weight*rew;

	rew= std::max(MIN_REWARD,rew);

	m_lastReward= rew;

	return rew;
}


CReward::CReward(CParameters* pParameters)
{
	m_numRewardComponents= (int) pParameters->getDouble("WORLD/REWARD/NUM_COMPONENTS");
	m_pErrorComponents= new CErrorComponent[m_numRewardComponents];

	m_minReward= pParameters->getDouble("WORLD/REWARD/MIN_REWARD");
	m_maxReward= pParameters->getDouble("WORLD/REWARD/MAX_REWARD");

	m_lastReward= 0.0;

	for (int i= 0; i<m_numRewardComponents; i++)
	{
		m_pErrorComponents[i].init(pParameters,i);
	}
}

CReward::~CReward()
{
	delete [] m_pErrorComponents;
}


double CReward::getReward(CState *s,CAction *a, CState *s_p)
{
	double rew= 0.0;

	for (int i= 0; i<m_numRewardComponents; i++)
	{
		rew+= m_pErrorComponents[i].getRewardComponent(s_p);
	}

	//rew*= (m_maxReward-m_minReward);
	//rew+= m_minReward;

	m_lastReward= rew;
	return rew;
}
