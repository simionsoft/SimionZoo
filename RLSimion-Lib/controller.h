#pragma once

#include "actor.h"

class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;

class tinyxml2::XMLElement;
class INumericValue;

class CMultiController : public CActor
{
	int m_numControllers;
	CActor **m_pControllers;
public:
	CMultiController(tinyxml2::XMLElement* pParameters);
	virtual ~CMultiController();

	static CActor* getInstance(tinyxml2::XMLElement* pParameters);

	void selectAction(CState *s, CAction *a);
};


class CLQRController : public CActor
{
	int* m_pVariableIndices;
	double *m_pGains;
	int m_outputActionIndex;
	int m_numVars;
public:
	CLQRController(tinyxml2::XMLElement* pParameters);
	~CLQRController();

	void selectAction(CState *s,CAction *a);
};

class CPIDController : public CActor
{
	INumericValue *m_pKP, *m_pKI, *m_pKD;
	int m_outputActionIndex;
	double m_intError;
	int m_errorVariableIndex;
public:
	CPIDController(tinyxml2::XMLElement* pParameters);
	~CPIDController(){}

	void selectAction(CState *s,CAction *a);
};

class CWindTurbineVidalController : public CActor
{
	INumericValue *m_pA, *m_pK_alpha, *m_pKP, *m_pKI, *m_P_s;
public:
	CWindTurbineVidalController(tinyxml2::XMLElement* pParameters);
	~CWindTurbineVidalController(){}

	void selectAction(CState *s,CAction *a);
};

class CWindTurbineBoukhezzarController : public CActor
{
	INumericValue *m_pC_0, *m_pKP, *m_pKI;
	double m_K_t, m_J_t;
public:
	CWindTurbineBoukhezzarController(tinyxml2::XMLElement* pParameters);
	~CWindTurbineBoukhezzarController(){}

	void selectAction(CState *s,CAction *a);
};

class CWindTurbineJonkmanController : public CActor
{
	//generator speed filter's parameters and variables
	double m_CornerFreq, m_GenSpeedF;
	//generator torque controller's parameters and variables
	double m_VS_RtGnSp, m_VS_SlPc, m_VS_Rgn2K, m_VS_Rgn2Sp, m_VS_CtInSp, m_VS_RtPwr;
	double m_VS_TrGnSp, m_VS_SySp, m_VS_Slope15, m_VS_Slope25, m_VS_Rgn3MP;
	//pitch controller's parameters and variables
	double m_IntSpdErr;
	INumericValue *m_PC_KK, *m_PC_KP, *m_PC_KI;
	double m_PC_RefSpd;
public:
	CWindTurbineJonkmanController(tinyxml2::XMLElement* pParameters);
	~CWindTurbineJonkmanController(){}

	void selectAction(CState *s,CAction *a);
};