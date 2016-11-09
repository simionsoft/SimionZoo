#pragma once

#include "simion.h"
#include "parameterized-object.h"
#include <vector>
class CNamedVarSet;
typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;

class CParameters;
class CNumericValue;

class CController : public CSimion
{

public:
	virtual ~CController(){}
	virtual int getNumOutputs()= 0;
	virtual int getOutputActionIndex(int output)= 0;

	static CController* getInstance(CParameters* pParameters);

	virtual void updateValue(const CState *s, const CAction *a, const CState *s_p, double r){ }

	virtual void selectAction(const CState *s, CAction *a)= 0;

	virtual void updatePolicy(const CState *s, const CAction *a, const CState *s_p, double r){}
};

class CLQRGain
{
public:
	CLQRGain(CParameters* pParameters);
	virtual ~CLQRGain(){}
	unsigned int m_variableIndex;
	double m_gain;
};

class CLQRController : public CController, public CParamObject
{
	std::vector<CLQRGain*> m_gains;
	int m_outputActionIndex;
public:
	CLQRController(CParameters* pParameters);
	virtual ~CLQRController();

	int getNumOutputs();
	int getOutputActionIndex(int output);

	void selectAction(const CState *s,CAction *a);
};

class CPIDController : public CController, public CParamObject
{
	CNumericValue *m_pKP, *m_pKI, *m_pKD;
	int m_outputActionIndex;
	double m_intError;
	int m_errorVariableIndex;
public:
	CPIDController(CParameters* pParameters);
	virtual ~CPIDController();

	int getNumOutputs();
	int getOutputActionIndex(int output);

	void selectAction(const CState *s,CAction *a);
};

class CWindTurbineVidalController : public CController, public CParamObject
{
	//state variable indices
	int m_omega_r_index, m_d_omega_r_index;
	int m_E_p_index, m_T_g_index, m_beta_index;
	int m_E_int_omega_r_index;
	//action variable indices
	int m_d_beta_index, m_d_T_g_index;
	CNumericValue *m_pA, *m_pK_alpha, *m_pKP, *m_pKI, *m_P_s;
public:
	CWindTurbineVidalController(CParameters* pParameters);
	virtual ~CWindTurbineVidalController();

	int getNumOutputs();
	int getOutputActionIndex(int output);

	void selectAction(const CState *s,CAction *a);
};

class CWindTurbineBoukhezzarController : public CController, public CParamObject
{
	//state variable indices
	int m_omega_r_index, m_d_omega_r_index;
	int m_E_p_index, m_T_a_index, m_T_g_index;
	int m_beta_index, m_E_int_omega_r_index;

	//action variable indices
	int m_d_beta_index, m_d_T_g_index;

	CNumericValue *m_pC_0, *m_pKP, *m_pKI;
	double m_K_t, m_J_t;
public:
	CWindTurbineBoukhezzarController(CParameters* pParameters);
	virtual ~CWindTurbineBoukhezzarController();

	int getNumOutputs();
	int getOutputActionIndex(int output);

	void selectAction(const CState *s,CAction *a);
};

class CWindTurbineJonkmanController : public CController, public CParamObject
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
	CNumericValue *m_PC_KK, *m_PC_KP, *m_PC_KI;
	double m_PC_RefSpd;
public:
	CWindTurbineJonkmanController(CParameters* pParameters);
	virtual ~CWindTurbineJonkmanController();

	int getNumOutputs();
	int getOutputActionIndex(int output);

	void selectAction(const CState *s,CAction *a);
};