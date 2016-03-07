#pragma once

#include "actor.h"

class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;

class CParameters;
class INumericValue;

class CMultiController : public CActor
{
	int m_numControllers;
	CActor **m_pControllers;
public:
	CMultiController(CParameters* pParameters);
	virtual ~CMultiController();

	static CActor* getInstance(CParameters* pParameters);

	void selectAction(const CState *s, CAction *a);
};


class CLQRController : public CActor
{
	int* m_pVariableIndices;
	double *m_pGains;
	int m_outputActionIndex;
	int m_numVars;
public:
	CLQRController(CParameters* pParameters);
	~CLQRController();

	void selectAction(const CState *s,CAction *a);
};

class CPIDController : public CActor
{
	INumericValue *m_pKP, *m_pKI, *m_pKD;
	int m_outputActionIndex;
	double m_intError;
	int m_errorVariableIndex;
public:
	CPIDController(CParameters* pParameters);
	~CPIDController();

	void selectAction(const CState *s,CAction *a);
};

class CWindTurbineVidalController : public CActor
{
	//state variable indices
	int m_omega_r_index, m_d_omega_r_index;
	int m_E_p_index, m_T_g_index, m_beta_index;
	int m_E_int_omega_r_index;
	//action variable indices
	int m_d_beta_index, m_d_T_g_index;
	INumericValue *m_pA, *m_pK_alpha, *m_pKP, *m_pKI, *m_P_s;
public:
	CWindTurbineVidalController(CParameters* pParameters);
	~CWindTurbineVidalController(){}

	void selectAction(const CState *s,CAction *a);
};

class CWindTurbineBoukhezzarController : public CActor
{
	//state variable indices
	int m_omega_r_index, m_d_omega_r_index;
	int m_E_p_index, m_T_a_index, m_T_g_index;
	int m_beta_index, m_E_int_omega_r_index;

	//action variable indices
	int m_d_beta_index, m_d_T_g_index;

	INumericValue *m_pC_0, *m_pKP, *m_pKI;
	double m_K_t, m_J_t;
public:
	CWindTurbineBoukhezzarController(CParameters* pParameters);
	~CWindTurbineBoukhezzarController(){}

	void selectAction(const CState *s,CAction *a);
};

class CWindTurbineJonkmanController : public CActor
{
	int m_omega_g_index, m_d_omega_r_index;
	int m_E_p_index, m_T_g_index, m_beta_index;
	int m_d_beta_index, m_d_T_g_index;

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
	CWindTurbineJonkmanController(CParameters* pParameters);
	~CWindTurbineJonkmanController(){}

	void selectAction(const CState *s,CAction *a);
};