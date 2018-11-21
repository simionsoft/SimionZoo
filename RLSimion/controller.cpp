#include "controller.h"
#include "named-var-set.h"
#include "worlds/world.h"
#include "config.h"
#include "parameters-numeric.h"
#include "controller-adaptive.h"
#include "app.h"
#include "experiment.h"
#include <algorithm>

vector<double>& Controller::evaluate(const State* s, const Action* a)
{
	for (unsigned int output = 0; output < getNumOutputs(); ++output)
	{
		//we have to saturate the output of evaluate()
		NamedVarProperties* pProperties = a->getProperties(getOutputAction(output));
		m_output[output] = std::min(pProperties->getMax(), std::max(pProperties->getMin(), evaluate(s, a, output)));
	}
	return m_output;
}

double Controller::selectAction(const State *s, Action *a)
{
	for (unsigned int output = 0; output < getNumOutputs(); ++output)
	{
		a->set( getOutputAction(output), evaluate(s, a, output));
	}
	return 1.0;
}

std::shared_ptr<Controller> Controller::getInstance(ConfigNode* pConfigNode)
{
	return CHOICE<Controller>(pConfigNode, "Controller", "The specific controller to be used",
	{
		{"PID",CHOICE_ELEMENT_NEW<PIDController>},
		{"LQR",CHOICE_ELEMENT_NEW<LQRController>},
		{"Jonkman",CHOICE_ELEMENT_NEW<WindTurbineJonkmanController>},
		{"Vidal",CHOICE_ELEMENT_NEW<WindTurbineVidalController>},
		{"Boukhezzar",CHOICE_ELEMENT_NEW<WindTurbineBoukhezzarController>},
	});
}



//LQR//////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
LQRGain::LQRGain(ConfigNode* pConfigNode)
{
	m_variable = STATE_VARIABLE(pConfigNode, "Variable", "The input state variable");
	m_gain = DOUBLE_PARAM(pConfigNode, "Gain", "The gain applied to the input state variable", 0.1);
}

LQRController::LQRController(ConfigNode* pConfigNode)
{
	m_outputAction = ACTION_VARIABLE(pConfigNode, "Output-Action", "The output action");
	m_gains = MULTI_VALUE<LQRGain>(pConfigNode, "LQR-Gain", "An LQR gain on an input state variable");

	for (size_t i = 0; i < m_gains.size(); ++i)
		m_inputStateVariables.push_back( m_gains[i]->m_variable.get() );
	m_output = vector<double> (1);

	//SimionApp::get()->registerStateActionFunction("LQR", this);
}

LQRController::~LQRController(){}

double LQRController::evaluate(const State* s, const Action* a, unsigned int index)
{
	//ignore index
	double output= 0.0;

	for (unsigned int i= 0; i<m_gains.size(); i++)
	{
		output+= s->get(m_gains[i]->m_variable.get())*m_gains[i]->m_gain.get();
	}
	// delta= -K*x
	return -output;
}

unsigned int LQRController::getNumOutputs()
{
	return 1;
}

const char* LQRController::getOutputAction(size_t output)
{
	if (output == 0)
		return m_outputAction.get();
	throw std::runtime_error("LQRController. Invalid action output given.");
}

//PID//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

PIDController::PIDController(ConfigNode* pConfigNode)
{
	m_outputAction= ACTION_VARIABLE(pConfigNode, "Output-Action", "The output action");
	m_pKP = CHILD_OBJECT_FACTORY<NumericValue>(pConfigNode, "KP", "Proportional gain");
	m_pKI = CHILD_OBJECT_FACTORY<NumericValue>(pConfigNode, "KI", "Integral gain");
	m_pKD = CHILD_OBJECT_FACTORY<NumericValue>(pConfigNode, "KD", "Derivative gain");

	m_errorVariable = STATE_VARIABLE(pConfigNode, "Input-Variable", "The input state variable");
	m_intError = 0.0;

	m_inputStateVariables.push_back(m_errorVariable.get());
	m_output = vector<double>(1);

	//SimionApp::get()->registerStateActionFunction("PID", this);
}

PIDController::~PIDController()
{}

unsigned int PIDController::getNumOutputs()
{
	return 1;
}
const char* PIDController::getOutputAction(size_t output)
{
	if (output == 0)
		return m_outputAction.get();
	throw std::runtime_error("LQRController. Invalid action output given.");
}

double PIDController::evaluate(const State* s, const Action* a, unsigned int output)
{
	if (SimionApp::get()->pWorld->getEpisodeSimTime()== 0.0)
		m_intError= 0.0;

	double error= s->get(m_errorVariable.get());
	double dError = error*SimionApp::get()->pWorld->getDT();
	m_intError += error*SimionApp::get()->pWorld->getDT();

	return error * m_pKP->get() + m_intError * m_pKI->get() + dError * m_pKD->get();
}



//VIDAL////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

WindTurbineVidalController::~WindTurbineVidalController()
{
}

WindTurbineVidalController::WindTurbineVidalController(ConfigNode* pConfigNode)
{
	m_pA= CHILD_OBJECT_FACTORY<NumericValue>(pConfigNode, "A", "A parameter of the torque controller", new ConstantValue(1.0));
	m_pK_alpha = CHILD_OBJECT_FACTORY<NumericValue>(pConfigNode,  "K_alpha", "K_alpha parameter of the torque controller", new ConstantValue(5000000));
	m_pKP = CHILD_OBJECT_FACTORY<NumericValue>(pConfigNode, "KP", "Proportional gain of the pitch controller", new ConstantValue(1.0));
	m_pKI = CHILD_OBJECT_FACTORY<NumericValue>(pConfigNode, "KI", "Integral gain of the pitch controller", new ConstantValue(0.0));

	m_ratedPower = World::getDynamicModel()->getConstant("RatedPower")
		/ World::getDynamicModel()->getConstant("ElectricalGeneratorEfficiency");
	m_genElecEff = World::getDynamicModel()->getConstant("ElectricalGeneratorEfficiency");

	m_inputStateVariables.push_back("omega_g");
	m_inputStateVariables.push_back("E_p");
	m_inputStateVariables.push_back("T_g");
	m_output = vector<double>(2);

	//SimionApp::get()->registerStateActionFunction("Vidal", this);
}

unsigned int WindTurbineVidalController::getNumOutputs()
{
	return 2;
}
const char* WindTurbineVidalController::getOutputAction(size_t output)
{
	switch (output)
	{
	case 0: return "beta";
	case 1: return "T_g";
	}
	throw std::runtime_error("LQRController. Invalid action output given.");
}

//aux function used in WindTurbineVidal controller
double WindTurbineVidalController::sgn(double value)
{
	if (value<0.0) return -1.0;
	else if (value>0.0) return 1.0;

	return 0.0;
}

double WindTurbineVidalController::evaluate(const State* s, const Action* a, unsigned int output)
{
	//initialise m_lastT_g if we have to, controllers don't implement reset()
	if (SimionApp::get() && SimionApp::get()->pWorld->getEpisodeSimTime() == 0)
		m_lastT_g = 0.0;

	//d(Tg)/dt= (-1/omega_g)*(T_g*(a*omega_g-d_omega_g)-a*P_setpoint + K_alpha*sgn(P_a-P_setpoint))
	//beta= K_p*(omega_ref - omega_g) + K_i*(error_integral)

	double omega_r = s->get("omega_r");
	double d_omega_r = s->get("d_omega_r");
	double omega_g = s->get("omega_g");
	double d_omega_g = s->get("d_omega_g");

	double error_P= s->get("E_p");

	double T_g= s->get("T_g");

	double e_omega_g, beta, d_T_g;

	switch (output)
	{
	case 0:
		e_omega_g = omega_g - World::getDynamicModel()->getConstant("RatedGeneratorSpeed");
		beta = 0.5*m_pKP->get()*e_omega_g*(1.0 + sgn(e_omega_g))
			+ m_pKI->get()*s->get("E_int_omega_g");
		beta = std::min(a->getProperties("beta")->getMax(), std::max(beta, a->getProperties("beta")->getMin()));
		return beta;
	case 1:
		if (omega_g != 0.0) d_T_g = (-1 / (omega_g*m_genElecEff))*(m_lastT_g*m_genElecEff*(m_pA->get() *omega_g + d_omega_g)
			- m_pA->get()*m_ratedPower + m_pK_alpha->get()*sgn(error_P));
		else d_T_g = 0.0;
		d_T_g = std::min(std::max(s->getProperties("d_T_g")->getMin(), d_T_g), s->getProperties("d_T_g")->getMax());
		double nextT_g = m_lastT_g + d_T_g * SimionApp::get()->pWorld->getDT();
		m_lastT_g = nextT_g;
		return nextT_g;
	}
	return 0.0;
}

//BOUKHEZZAR CONTROLLER////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

WindTurbineBoukhezzarController::~WindTurbineBoukhezzarController()
{

}

WindTurbineBoukhezzarController::WindTurbineBoukhezzarController(ConfigNode* pConfigNode)
{
	m_pC_0	= CHILD_OBJECT_FACTORY<NumericValue>(pConfigNode,"C_0", "C_0 parameter", new ConstantValue(1.0) );
	m_pKP = CHILD_OBJECT_FACTORY<NumericValue>(pConfigNode,"KP", "Proportional gain of the pitch controller", new ConstantValue(1.0) );
	m_pKI = CHILD_OBJECT_FACTORY<NumericValue>(pConfigNode,"KI", "Integral gain of the pitch controller", new ConstantValue(0.0) );

	m_J_t = World::getDynamicModel()->getConstant("TotalTurbineInertia");
	m_K_t = World::getDynamicModel()->getConstant("TotalTurbineTorsionalDamping");
	m_genElecEff = World::getDynamicModel()->getConstant("ElectricalGeneratorEfficiency");

	m_inputStateVariables.push_back("omega_g");
	m_inputStateVariables.push_back("E_p");
	m_inputStateVariables.push_back("T_g");
	m_output = vector<double>(2);

	//SimionApp::get()->registerStateActionFunction("Boukhezzar", this);
}

unsigned int WindTurbineBoukhezzarController::getNumOutputs()
{
	return 2;
}
const char* WindTurbineBoukhezzarController::getOutputAction(size_t output)
{
	switch (output)
	{
	case 0: return "beta";
	case 1: return "T_g";
	}
	throw std::runtime_error("LQRController. Invalid action output given.");
}


double WindTurbineBoukhezzarController::evaluate(const State *s,const Action *a, unsigned int output)
{
	//initialise m_lastT_g if we have to, controllers don't implement reset()
	if (SimionApp::get()->pWorld->getEpisodeSimTime() == 0)
		m_lastT_g = 0.0;// SimionApp::get()->pWorld->getDynamicModel()->getConstant("RatedGeneratorTorque");

	//d(Tg)/dt= (1/omega_g)*(C_0*error_P - (1/J_t)*(T_a*T_g - K_t*omega_g*T_g - T_g*T_g))
	//d(beta)/dt= K_p*(omega_ref - omega_g)
	//Original expression in Boukhezzar's paper:
	//double d_T_g= (1.0/omega_g)*(m_pC_0.get()*error_P - (T_a*m_lastT_g - m_K_t*omega_g*m_lastT_g 
	//	- m_lastT_g *m_lastT_g) / m_J_t );

	double omega_g= s->get("omega_g");
	double d_omega_g = s->get("d_omega_g");		
	
	double beta= s->get("beta");
	double E_p = s->get("E_p");
	double P_e = s->get("P_e");
	
	//Boukhezzar controller without making substitution: d_T_g= (-1/omega_g)(d_omega_g*T_g+C_0*Ep)
	double d_T_g = (-1.0/(omega_g*m_genElecEff))*(d_omega_g*m_lastT_g*m_genElecEff + m_pC_0->get()*s->get("E_p"));

	d_T_g = std::min(std::max(s->getProperties("d_T_g")->getMin(), d_T_g), s->getProperties("d_T_g")->getMax());

	double e_omega_g = omega_g - World::getDynamicModel()->getConstant("RatedGeneratorSpeed");
	double desiredBeta = m_pKP->get()*e_omega_g + m_pKI->get()*s->get("E_int_omega_g");

	switch (output)
	{
	case 0:
		return desiredBeta;
	case 1:

		double nextT_g = m_lastT_g + d_T_g * SimionApp::get()->pWorld->getDT();
		m_lastT_g = nextT_g;
		return nextT_g;
	}

	return 0.0;
}

//JONKMAN//////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

WindTurbineJonkmanController::~WindTurbineJonkmanController()
{
}

WindTurbineJonkmanController::WindTurbineJonkmanController(ConfigNode* pConfigNode)
{
	//GENERATOR SPEED FILTER PARAMETERS
	m_CornerFreq = DOUBLE_PARAM(pConfigNode, "CornerFreq", "Corner Freq. parameter", 1.570796);

	//TORQUE CONTROLLER'S PARAMETERS
	DynamicModel* pDynamicModel = SimionApp::get()->pWorld->getDynamicModel();
	m_ratedGenSpeed = pDynamicModel->getConstant("RatedGeneratorSpeed");
	m_ratedPower = pDynamicModel->getConstant("RatedPower")/pDynamicModel->getConstant("ElectricalGeneratorEfficiency");
	m_VS_SlPc = DOUBLE_PARAM(pConfigNode, "VS_SlPc", "SIPc parameter", 10.0);
	m_VS_Rgn2K = DOUBLE_PARAM(pConfigNode, "VS_Rgn2K", "Rgn2K parameter", 2.332287);
	m_VS_Rgn2Sp = DOUBLE_PARAM(pConfigNode, "VS_Rgn2Sp", "Rgn2Sp parameter", 91.21091);
	m_VS_CtInSp = DOUBLE_PARAM(pConfigNode, "VS_CtInSp", "CtlnSp parameter", 70.16224);
	m_VS_Rgn3MP = DOUBLE_PARAM(pConfigNode, "VS_Rgn3MP", "Rgn3MP parameter", 0.01745329);
	
	m_VS_SySp    = m_ratedGenSpeed/( 1.0 +  0.01*m_VS_SlPc.get() );
	m_VS_Slope15 = ( m_VS_Rgn2K.get()*m_VS_Rgn2Sp.get()*m_VS_Rgn2Sp.get() )/( m_VS_Rgn2Sp.get() - m_VS_CtInSp.get());
	m_VS_Slope25 = ( m_ratedPower/m_ratedGenSpeed)/( m_ratedGenSpeed - m_VS_SySp);

	if ( m_VS_Rgn2K.get()== 0.0 )  //.TRUE. if the Region 2 torque is flat, and thus, the denominator in the ELSE condition is zero
		m_VS_TrGnSp = m_VS_SySp;
	else                          //.TRUE. if the Region 2 torque is quadratic with speed
		m_VS_TrGnSp = ( m_VS_Slope25 - sqrt( m_VS_Slope25*( m_VS_Slope25 - 4.0*m_VS_Rgn2K.get()*m_VS_SySp ) ) )/( 2.0*m_VS_Rgn2K.get() );

	//PITCH CONTROLLER'S PARAMETERS
	m_PC_KK = CHILD_OBJECT_FACTORY<NumericValue>(pConfigNode,"PC_KK","Pitch angle were the the derivative of the...", new ConstantValue(0.1099965));
	m_PC_KP= CHILD_OBJECT_FACTORY<NumericValue>(pConfigNode, "PC_KP","Proportional gain of the pitch controller", new ConstantValue(0.01882681));
	m_PC_KI= CHILD_OBJECT_FACTORY<NumericValue>(pConfigNode, "PC_KI","Integral gain of the pitch controller", new ConstantValue(0.008068634) );
	m_PC_RefSpd= DOUBLE_PARAM(pConfigNode,"PC_RefSpd","Pitch control reference speed", 122.9096);

	m_IntSpdErr= 0.0;

	m_inputStateVariables.push_back("omega_g");
	m_inputStateVariables.push_back("T_g");
	m_inputStateVariables.push_back("beta");
	m_output = vector<double>(2);

	//SimionApp::get()->registerStateActionFunction("Jonkman", this);
}

unsigned int WindTurbineJonkmanController::getNumOutputs()
{
	return 2;
}
const char* WindTurbineJonkmanController::getOutputAction(size_t output)
{
	switch (output)
	{
	case 1: return "beta";
	case 0: return "T_g";
	}

	throw std::runtime_error("LQRController. Invalid action output given.");
}

double WindTurbineJonkmanController::evaluate(const State *s,const Action *a, unsigned int output)
{
	switch (output)
	{
	case 0:

		//Filter the generator speed
		double lowPassFilterAlpha, d_T_g;

		if (SimionApp::get()->pWorld->getEpisodeSimTime() == 0.0)
		{
			m_lastT_g = 0.0;
			lowPassFilterAlpha = 1.0;
			m_GenSpeedF = s->get("omega_g");
			m_IntSpdErr = 0.0;
		}
		else
			lowPassFilterAlpha = exp(-SimionApp::get()->pWorld->getDT()*m_CornerFreq.get());

		m_GenSpeedF = (1.0 - lowPassFilterAlpha)*s->get("omega_g") + lowPassFilterAlpha * m_GenSpeedF;

		//TORQUE CONTROLLER
		double DesiredGenTrq;
		if ((m_GenSpeedF >= m_ratedGenSpeed) || (s->get("beta") >= m_VS_Rgn3MP.get()))   //We are in region 3 - power is constant
			DesiredGenTrq = m_ratedPower / m_GenSpeedF;
		else if (m_GenSpeedF <= m_VS_CtInSp.get())							//We are in region 1 - torque is zero
			DesiredGenTrq = 0.0;
		else if (m_GenSpeedF < m_VS_Rgn2Sp.get())                          //We are in region 1 1/2 - linear ramp in torque from zero to optimal
			DesiredGenTrq = m_VS_Slope15 * (m_GenSpeedF - m_VS_CtInSp.get());
		else if (m_GenSpeedF < m_VS_TrGnSp)                                      //We are in region 2 - optimal torque is proportional to the square of the generator speed
			DesiredGenTrq = m_VS_Rgn2K.get()*m_GenSpeedF*m_GenSpeedF;
		else                                                                       //We are in region 2 1/2 - simple induction generator transition region
			DesiredGenTrq = m_VS_Slope25 * (m_GenSpeedF - m_VS_SySp);

		DesiredGenTrq = std::min(DesiredGenTrq, s->getProperties("T_g")->getMax());   //Saturate the command using the maximum torque limit

		//we limit the torque change rate
		d_T_g = (DesiredGenTrq - m_lastT_g) / SimionApp::get()->pWorld->getDT();
		d_T_g = std::min(std::max(s->getProperties("d_T_g")->getMin(), d_T_g), s->getProperties("d_T_g")->getMax());

		m_lastT_g = m_lastT_g + d_T_g * SimionApp::get()->pWorld->getDT();

		return m_lastT_g;

		//we pass the desired generator torque instead of the rate
		//return DesiredGenTrq;
	case 1:

		//PITCH CONTROLLER
		double GK = 1.0 / (1.0 + s->get("beta") / m_PC_KK->get());

		//Compute the current speed error and its integral w.r.t. time; saturate the
		//  integral term using the pitch angle limits:
		double SpdErr = m_GenSpeedF - m_PC_RefSpd.get();                                 //Current speed error
		m_IntSpdErr = m_IntSpdErr + SpdErr * SimionApp::get()->pWorld->getDT();                           //Current integral of speed error w.r.t. time
		//Saturate the integral term using the pitch angle limits, converted to integral speed error limits
		m_IntSpdErr = std::min(std::max(m_IntSpdErr, s->getProperties("beta")->getMin() / (GK*m_PC_KI->get()))
			, s->getProperties("beta")->getMax() / (GK*m_PC_KI->get()));

		//Compute the pitch commands associated with the proportional and integral  gains:
		double PitComP = GK * m_PC_KP->get() * SpdErr; //Proportional term
		double PitComI = GK * m_PC_KI->get() * m_IntSpdErr; //Integral term (saturated)

		//Superimpose the individual commands to getSample the total pitch command;
		//  saturate the overall command using the pitch angle limits:
		double PitComT = PitComP + PitComI;                                     //Overall command (unsaturated)
		PitComT = std::min(std::max(PitComT, s->getProperties("beta")->getMin())
			, s->getProperties("beta")->getMax());           //Saturate the overall command using the pitch angle limits

		//we pass the desired blade pitch angle to the world
		return PitComT;
	}

	return 0.0;
}