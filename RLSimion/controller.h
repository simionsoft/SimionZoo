#pragma once

#include "simion.h"
#include "parameters.h"
class NamedVarSet;
typedef NamedVarSet State;
typedef NamedVarSet Action;

class ConfigNode;
class NumericValue;

class Controller : public Simion
{

public:
	virtual ~Controller(){}
	virtual size_t getNumOutputs()= 0;
	virtual const char* getOutputAction(size_t output)= 0;

	static std::shared_ptr<Controller> getInstance(ConfigNode* pConfigNode);

	//regular controllers need not update. Default implementation does nothing but it can be overriden
	//by adaptive controllers if need to
	virtual double update(const State *s, const Action *a, const State *s_p, double r, double behaviorProb) = 0;

	virtual double selectAction(const State *s, Action *a)= 0;
};

class LQRGain
{
public:
	LQRGain(ConfigNode* pConfigNode);
	virtual ~LQRGain(){}
	STATE_VARIABLE m_variableIndex;
	DOUBLE_PARAM m_gain;
};

class LQRController : public Controller
{
	MULTI_VALUE<LQRGain> m_gains;
	ACTION_VARIABLE m_outputAction;
public:
	LQRController(ConfigNode* pConfigNode);
	virtual ~LQRController();

	size_t getNumOutputs();
	const char* getOutputAction(size_t output);

	double selectAction(const State *s,Action *a);
	virtual double update(const State *s, const Action *a, const State *s_p, double r, double behaviorProb) { return 1.0; }
};

class PIDController : public Controller
{
	CHILD_OBJECT_FACTORY<NumericValue> m_pKP;
	CHILD_OBJECT_FACTORY<NumericValue> m_pKI;
	CHILD_OBJECT_FACTORY<NumericValue> m_pKD;
	ACTION_VARIABLE m_outputAction;
	double m_intError;
	STATE_VARIABLE m_errorVariable;
public:
	PIDController(ConfigNode* pConfigNode);
	virtual ~PIDController();

	size_t getNumOutputs();
	const char* getOutputAction(size_t output);

	double selectAction(const State *s,Action *a);
	virtual double update(const State *s, const Action *a, const State *s_p, double r, double behaviorProb) { return 1.0; }
};

class WindTurbineVidalController : public Controller
{
protected:
	//aux function
	double sgn(double value);

	double m_ratedPower;
	double m_genElecEff;
	double m_lastT_g = 0.0;
	CHILD_OBJECT_FACTORY<NumericValue> m_pA, m_pK_alpha, m_pKP, m_pKI;
public:
	WindTurbineVidalController(ConfigNode* pConfigNode);
	virtual ~WindTurbineVidalController();

	size_t getNumOutputs();
	const char* getOutputAction(size_t output);

	virtual double selectAction(const State *s,Action *a);
	virtual double update(const State *s, const Action *a, const State *s_p, double r, double behaviorProb) { return 1.0; }
};

class WindTurbineBoukhezzarController : public Controller
{
protected:
	CHILD_OBJECT_FACTORY<NumericValue> m_pC_0, m_pKP, m_pKI;
	double m_K_t, m_J_t;
	double m_lastT_g = 0.0;
	double m_genElecEff;
public:
	WindTurbineBoukhezzarController(ConfigNode* pConfigNode);
	virtual ~WindTurbineBoukhezzarController();

	size_t getNumOutputs();
	const char* getOutputAction(size_t output);

	virtual double selectAction(const State *s,Action *a);
	virtual double update(const State *s, const Action *a, const State *s_p, double r, double behaviorProb) { return 1.0; }
};

class WindTurbineJonkmanController : public Controller
{
protected:
	//generator speed filter's parameters and variables
	DOUBLE_PARAM m_CornerFreq;
	double m_GenSpeedF;
	//generator torque controller's parameters and variables
	DOUBLE_PARAM m_VS_SlPc, m_VS_Rgn2K, m_VS_Rgn2Sp, m_VS_CtInSp;
	DOUBLE_PARAM m_VS_Rgn3MP;
	double m_ratedGenSpeed, m_ratedPower;
	double m_VS_SySp, m_VS_Slope15, m_VS_Slope25, m_VS_TrGnSp;
	//pitch controller's parameters and variables
	double m_IntSpdErr;
	CHILD_OBJECT_FACTORY<NumericValue> m_PC_KK, m_PC_KP, m_PC_KI;
	DOUBLE_PARAM m_PC_RefSpd;
public:
	WindTurbineJonkmanController(ConfigNode* pConfigNode);
	virtual ~WindTurbineJonkmanController();

	size_t getNumOutputs();
	const char* getOutputAction(size_t output);

	virtual double selectAction(const State *s,Action *a);
	virtual double update(const State *s, const Action *a, const State *s_p, double r, double behaviorProb) { return 1.0; }
};