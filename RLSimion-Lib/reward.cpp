#include "stdafx.h"
#include "reward.h"
#include "states-and-actions.h"
#include "parameterized-object.h"
#include "parameters-xml-helper.h"
#include "world.h"
#include "globals.h"

#define NUM_MAX_REWARD_COMPONENTS 10

#define VARIABLE_DIFFERENCE 0
#define DEVIATION_VARIABLE 1
#define CONSTANT_DIFFERENCE 2
#define PUNISH_IF_ABOVE 3
#define PUNISH_IF_BELOW 4

double CReward::m_minReward;
double CReward::m_maxReward;

class CRewardComponent : public CParamObject
{
	//char m_errorComponentType[MAX_PARAMETER_NAME_SIZE];
	//char m_controlledVariable[MAX_PARAMETER_NAME_SIZE];
	//char m_setpointVariable[MAX_PARAMETER_NAME_SIZE];
	//char m_controlErrorVariable[MAX_PARAMETER_NAME_SIZE];

	//double m_setpointConstant;
	//double m_weight;
	//int m_componentIndex;
	////double m_rewardComponentMu;
	//double m_tolerance;
	const char* m_variable;
	INumericValue *m_pTolerance;
	INumericValue *m_pWeight;
	double m_lastReward;
public:
	CRewardComponent(tinyxml2::XMLElement* pParameters);
	~CRewardComponent();

//	void init(tinyxml2::XMLElement* pParameters/*,int componentIndex*/);
	double getRewardComponent(CState *state);
	double getLastRewardComponent(){return m_lastReward;}
};

CRewardComponent::CRewardComponent(tinyxml2::XMLElement* pParameters) : CParamObject(pParameters)
{
	//m_errorComponentType[0]= 0;
	//m_controlledVariable[0]= 0;
	//m_setpointVariable[0]= 0;
	//m_controlErrorVariable[0]= 0;
	//m_weight= 0.0;
	//m_componentIndex= -1;
	m_variable = pParameters->FirstChildElement("Variable")->GetText();
	m_pTolerance = XMLParameters::getNumericHandler(pParameters->FirstChildElement("Tolerance"));
	m_pWeight = XMLParameters::getNumericHandler(pParameters->FirstChildElement("Weight"));
	m_lastReward= 0.0;
}

CRewardComponent::~CRewardComponent()
{
}
/*
void CRewardComponent::init(tinyxml2::XMLElement* pParameters)
{
	char parameterName[256];

	sprintf_s(parameterName,256,"COMPONENT_TYPE_%d",componentIndex);
	sprintf_s(m_errorComponentType,MAX_PARAMETER_NAME_SIZE,"%s",pParameters->getParameter(parameterName)->getStringPtr());
	
	sprintf_s(parameterName,256,"COMPONENT_WEIGHT_%d",componentIndex);
	m_weight= pParameters->getParameter(parameterName)->getDouble();

	sprintf_s(parameterName,256,"COMPONENT_TOLERANCE_%d",componentIndex);
	m_tolerance= pParameters->getParameter(parameterName)->getDouble();

	if (strcmp(m_errorComponentType,"VARIABLE_DIFFERENCE")==0)
	{
		sprintf_s(parameterName,256,"CONTROLLED_VARIABLE_%d",componentIndex);
		sprintf_s(m_controlledVariable,MAX_PARAMETER_NAME_SIZE,"%s",pParameters->getParameter(parameterName)->getStringPtr());
		sprintf_s(parameterName,256,"SETPOINT_VARIABLE_%d",componentIndex);
		sprintf_s(m_setpointVariable,MAX_PARAMETER_NAME_SIZE,"%s",pParameters->getParameter(parameterName)->getStringPtr());
	}
	else if (strcmp(m_errorComponentType,"DEVIATION_VARIABLE")==0)
	{
		sprintf_s(parameterName,256,"CONTROL_ERROR_VARIABLE_%d",componentIndex);
		sprintf_s(m_controlErrorVariable,MAX_PARAMETER_NAME_SIZE,"%s", pParameters->getParameter(parameterName)->getStringPtr());
	}
	else if ( (strcmp(m_errorComponentType,"CONSTANT_DIFFERENCE")==0)
			|| (strcmp(m_errorComponentType,"PUNISH_IF_ABOVE")==0)
			|| (strcmp(m_errorComponentType,"PUNISH_IF_BELOW")==0) )
	{
		sprintf_s(parameterName,256,"CONTROLLED_VARIABLE_%d",componentIndex);
		sprintf_s(m_controlledVariable,MAX_PARAMETER_NAME_SIZE,"%s",pParameters->getParameter(parameterName)->getStringPtr());
		sprintf_s(parameterName,256,"SETPOINT_CONSTANT_%d",componentIndex);
		m_setpointConstant= pParameters->getParameter(parameterName)->getDouble();
	}
	else assert(0);
}*/




double CRewardComponent::getRewardComponent(CState* state)
{
	double rew,error;
	/*const char* deviationVariable;

	deviationVariable = m_pParameters->FirstChildElement("Variable")->GetText();
*/
	error = state->getValue(m_variable);
/*
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
*/

	error= (error)/(m_pTolerance->getValue());

	rew = CReward::m_maxReward - fabs(error);
	//rew= exp(-(error*error));
	


	//rew = CReward::m_maxReward - fabs(error);// *error;

	//rew= m_weight*rew;

	rew= std::max(CReward::m_minReward,rew);

	m_lastReward= rew;

	return rew;
}


CReward::CReward(tinyxml2::XMLElement* pParameters) : CParamObject(pParameters)
{
	m_minReward= XMLParameters::getConstDouble(pParameters->FirstChildElement("Min"));
	m_maxReward = XMLParameters::getConstDouble(pParameters->FirstChildElement("Max"));

	m_lastReward= 0.0;

	tinyxml2::XMLElement* pRewardComponents = m_pParameters->FirstChildElement("Reward-components");

	m_numRewardComponents = XMLParameters::countChildren(pRewardComponents);

	m_pRewardComponents = new CRewardComponent*[m_numRewardComponents];

	tinyxml2::XMLElement* pComponent = pRewardComponents->FirstChildElement();
	for (int i= 0; i<m_numRewardComponents; i++)
	{
		m_pRewardComponents[i] = new CRewardComponent(pComponent);

		pComponent = pComponent->NextSiblingElement();
	}
}

CReward::~CReward()
{
	delete [] m_pRewardComponents;
}


double CReward::getReward(CState *s,CAction *a, CState *s_p)
{
	double rew = 1.0;

	for (int i= 0; i<m_numRewardComponents; i++)
	{
		rew+= m_pRewardComponents[i]->getRewardComponent(s_p);
	}

	//rew*= (m_maxReward-m_minReward);
	//rew+= m_minReward;

	m_lastReward= rew;
	return rew;
}

double CReward::getLastRewardComponent(int i)
{
	if (i >= 0 && i < m_numRewardComponents)
		return m_pRewardComponents[i]->getLastRewardComponent();
	return CReward::m_minReward;
}