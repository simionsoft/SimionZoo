#pragma once

#include "actor.h"

class CParameters;
class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;



class CLQRController: public CActor
{
	int* m_pVariableIndices;
	double *m_pGains;
	int m_numVars;
public:
	CLQRController(CParameters* pParameters);
	~CLQRController();

	double selectAction(CState *s,CAction *a);

	void updatePolicy(CState *s,CAction *a,CState *s_p,double r,double td){}
};

class CPIDController: public CActor
{
	double m_kP, m_kI, m_kD;
	double m_intError;
	int m_errorVariableIndex;
public:
	CPIDController(CParameters* pParameters);
	~CPIDController(){}

	double selectAction(CState *s,CAction *a);

	void updatePolicy(CState *s,CAction *a,CState *s_p,double r,double td){}
};

class CWindTurbineVidalController: public CActor
{
	double *m_pA, *m_pK_alpha, *m_pKP, *m_pKI;
	double m_P_s;
public:
	CWindTurbineVidalController(CParameters* pParameters);
	~CWindTurbineVidalController(){}

	double selectAction(CState *s,CAction *a);

	void updatePolicy(CState *s,CAction *a,CState *s_p,double r,double td){}
};

class CWindTurbineBoukhezzarController: public CActor
{
	double *m_pC_0, *m_pKP, *m_pKI;
	double m_K_t, m_J_t;
public:
	CWindTurbineBoukhezzarController(CParameters* pParameters);
	~CWindTurbineBoukhezzarController(){}

	double selectAction(CState *s,CAction *a);

	void updatePolicy(CState *s,CAction *a,CState *s_p,double r,double td){}
};

class CWindTurbineJonkmanController: public CActor
{
	//generator speed filter's parameters and variables
	double m_CornerFreq, m_GenSpeedF;
	//generator torque controller's parameters and variables
	double m_VS_RtGnSp, m_VS_SlPc, m_VS_Rgn2K, m_VS_Rgn2Sp, m_VS_CtInSp, m_VS_RtPwr;
	double m_VS_TrGnSp, m_VS_SySp, m_VS_Slope15, m_VS_Slope25, m_VS_Rgn3MP;
	//pitch controller's parameters and variables
	double m_IntSpdErr;
	double m_PC_KK, m_PC_KP, m_PC_KI, m_PC_RefSpd;
public:
	CWindTurbineJonkmanController(CParameters* pParameters);
	~CWindTurbineJonkmanController(){}

	double selectAction(CState *s,CAction *a);

	void updatePolicy(CState *s,CAction *a,CState *s_p,double r,double td){}
};