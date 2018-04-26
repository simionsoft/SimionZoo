#include "controller.h"
#include "named-var-set.h"
#include "worlds/world.h"
#include "config.h"
#include "parameters-numeric.h"
#include "controller-adaptive.h"
#include "app.h"
#include "experiment.h"
#include <algorithm>


std::shared_ptr<Controller> Controller::getInstance(ConfigNode* pConfigNode)
{
	return CHOICE<Controller>(pConfigNode, "Controller", "The specific controller to be used",
	{
		{"PID",CHOICE_ELEMENT_NEW<PIDController>},
		{"LQR",CHOICE_ELEMENT_NEW<LQRController>},
		{"Jonkman",CHOICE_ELEMENT_NEW<WindTurbineJonkmanController>},
		{"Extended-Jonkman",CHOICE_ELEMENT_NEW<ExtendedWindTurbineJonkmanController>},
		{"Vidal",CHOICE_ELEMENT_NEW<WindTurbineVidalController>},
		{"Extended-Vidal",CHOICE_ELEMENT_NEW<ExtendedWindTurbineVidalController>},
		{"Boukhezzar",CHOICE_ELEMENT_NEW<WindTurbineBoukhezzarController>},
		{"Extended-Boukhezzar",CHOICE_ELEMENT_NEW<ExtendedWindTurbineBoukhezzarController>}
	});
}



//LQR//////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
	/*int* m_pVariableIndices;
	double *m_pGains;
	int numVars;*/

LQRGain::LQRGain(ConfigNode* pConfigNode)
{
	m_variableIndex = STATE_VARIABLE(pConfigNode, "Variable", "The input state variable");
	m_gain = DOUBLE_PARAM(pConfigNode, "Gain", "The gain applied to the input state variable", 0.1);
}
LQRController::LQRController(ConfigNode* pConfigNode)
{
	m_outputAction = ACTION_VARIABLE(pConfigNode, "Output-Actionn", "The output action");
	m_gains = MULTI_VALUE<LQRGain>(pConfigNode, "LQR-Gain", "An LQR gain on an input state variable");

}

LQRController::~LQRController(){}

double LQRController::selectAction(const State *s, Action *a)
{
	double output= 0.0; // only 1-dimension so far

	for (unsigned int i= 0; i<m_gains.size(); i++)
	{
		output+= s->get(m_gains[i]->m_variableIndex.get())*m_gains[i]->m_gain.get();
	}
	// delta= -K*x
	a->set(m_outputAction.get(), -output);

	return 1.0;
}

int LQRController::getNumOutputs()
{
	return 1;
}
int LQRController::getOutputActionIndex(int output)
{
	if (output == 0)
		return m_outputAction.get();
	throw std::exception("LQRController. Invalid action output given.");
	return -1;
}

//PID//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

PIDController::PIDController(ConfigNode* pConfigNode)
{
	m_outputAction= ACTION_VARIABLE(pConfigNode, "Output-Action", "The output action");
	m_pKP = CHILD_OBJECT_FACTORY<NumericValue>(pConfigNode, "KP", "Proportional gain");
	m_pKI = CHILD_OBJECT_FACTORY<NumericValue>(pConfigNode, "KI", "Integral gain");
	m_pKD = CHILD_OBJECT_FACTORY<NumericValue>(pConfigNode, "KD", "Derivative gain");

	m_errorVariableIndex = STATE_VARIABLE(pConfigNode, "Input-Variable", "The input state variable");

	m_intError= 0.0;
}

PIDController::~PIDController()
{}

int PIDController::getNumOutputs()
{
	return 1;
}
int PIDController::getOutputActionIndex(int output)
{
	if (output == 0)
		return m_outputAction.get();
	throw std::exception("LQRController. Invalid action output given.");
	return -1;
}

double PIDController::selectAction(const State *s, Action *a)
{
	if (SimionApp::get()->pWorld->getEpisodeSimTime()== 0.0)
		m_intError= 0.0;

	double error= s->get(m_errorVariableIndex.get());
	double dError = error*SimionApp::get()->pWorld->getDT();
	m_intError += error*SimionApp::get()->pWorld->getDT();

	a->set(m_outputAction.get()
		,error*m_pKP->get() + m_intError*m_pKI->get() + dError*m_pKD->get());

	return 1.0;
}



//VIDAL////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

WindTurbineVidalController::~WindTurbineVidalController()
{
}

WindTurbineVidalController::WindTurbineVidalController(ConfigNode* pConfigNode)
{
	m_pA= DOUBLE_PARAM(pConfigNode, "A", "A parameter of the torque controller",1.0);
	m_pK_alpha = DOUBLE_PARAM(pConfigNode,  "K_alpha", "K_alpha parameter of the torque controller",5000000);
	m_pKP = DOUBLE_PARAM(pConfigNode, "KP", "Proportional gain of the pitch controller",1.0);
	m_pKI = DOUBLE_PARAM(pConfigNode, "KI", "Integral gain of the pitch controller",0.0);

	Descriptor& pStateDescriptor = World::getDynamicModel()->getStateDescriptor();
	m_omega_r = pStateDescriptor.getVarIndex("omega_r");
	m_d_omega_r = pStateDescriptor.getVarIndex("d_omega_r");
	m_omega_g = pStateDescriptor.getVarIndex("omega_g");
	m_d_omega_g = pStateDescriptor.getVarIndex("d_omega_g");
	m_E_p = pStateDescriptor.getVarIndex("E_p");
	m_T_g = pStateDescriptor.getVarIndex("T_g");
	m_beta = pStateDescriptor.getVarIndex("beta");
	m_E_int_omega_r = pStateDescriptor.getVarIndex("E_int_omega_r");

	Descriptor& pActionDescriptor = World::getDynamicModel()->getActionDescriptor();

	m_a_beta = pActionDescriptor.getVarIndex("beta");
	m_a_T_g = pActionDescriptor.getVarIndex("T_g");

	m_ratedPower = World::getDynamicModel()->getConstant("RatedPower")
		/ World::getDynamicModel()->getConstant("ElectricalGeneratorEfficiency");
	m_genElecEff = World::getDynamicModel()->getConstant("ElectricalGeneratorEfficiency");
}

int WindTurbineVidalController::getNumOutputs()
{
	return 2;
}
int WindTurbineVidalController::getOutputActionIndex(int output)
{
	switch (output)
	{
	case 0: return m_a_beta;
	case 1: return m_a_T_g;
	default: throw std::exception("LQRController. Invalid action output given.");
	}
	
	return -1;
}

//aux function used in WindTurbineVidal controller
double WindTurbineVidalController::sgn(double value)
{
	if (value<0.0) return -1.0;
	else if (value>0.0) return 1.0;

	return 0.0;
}

double WindTurbineVidalController::selectAction(const State *s,Action *a)
{
	bool evaluation = SimionApp::get()->pExperiment->isEvaluationEpisode();
	//initialise m_lastT_g if we have to, controllers don't implement reset()
	if (SimionApp::get()->pWorld->getEpisodeSimTime() == 0)
		m_lastT_g = 0.0;

	//d(Tg)/dt= (-1/omega_g)*(T_g*(a*omega_g-d_omega_g)-a*P_setpoint + K_alpha*sgn(P_a-P_setpoint))
	//beta= K_p*(omega_ref - omega_g) + K_i*(error_integral)

	double omega_r = s->get(m_omega_r);
	double d_omega_r = s->get(m_d_omega_r);
	double omega_g = s->get(m_omega_g);
	double d_omega_g = s->get(m_d_omega_g);

	double error_P= s->get(m_E_p);

	double T_g= s->get(m_T_g);
	
	double d_T_g;
	
	if (omega_g != 0.0) d_T_g = (-1 / (omega_g*m_genElecEff))*(m_lastT_g*m_genElecEff*(m_pA.get() *omega_g + d_omega_g)
		- m_pA.get()*m_ratedPower + m_pK_alpha.get()*sgn(error_P));
	else d_T_g= 0.0;

	double e_omega_g = omega_g - World::getDynamicModel()->getConstant("RatedGeneratorSpeed");
	double beta = 0.5*m_pKP.get()*e_omega_g*(1.0 + sgn(e_omega_g))
				+ m_pKI.get()*s->get("E_int_omega_g");

	beta = std::min(a->getProperties("beta").getMax(), std::max(beta, a->getProperties("beta").getMin()));
	d_T_g = std::min(std::max(s->getProperties("d_T_g").getMin(), d_T_g), s->getProperties("d_T_g").getMax());
	a->set(m_a_beta,beta);
	double nextT_g = m_lastT_g + d_T_g* SimionApp::get()->pWorld->getDT();
	a->set(m_a_T_g,nextT_g);
	m_lastT_g = nextT_g;

	return 1.0;
}

//BOUKHEZZAR CONTROLLER////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

WindTurbineBoukhezzarController::~WindTurbineBoukhezzarController()
{

}

WindTurbineBoukhezzarController::WindTurbineBoukhezzarController(ConfigNode* pConfigNode)
{
	m_pC_0	= DOUBLE_PARAM(pConfigNode,"C_0", "C_0 parameter",1.0);
	m_pKP = DOUBLE_PARAM(pConfigNode,"KP", "Proportional gain of the pitch controller",1.0);
	m_pKI = DOUBLE_PARAM(pConfigNode,"KI", "Integral gain of the pitch controller",0.0);

	m_J_t = World::getDynamicModel()->getConstant("TotalTurbineInertia");
	m_K_t = World::getDynamicModel()->getConstant("TotalTurbineTorsionalDamping");
	m_genElecEff = World::getDynamicModel()->getConstant("ElectricalGeneratorEfficiency");

	Descriptor& pStateDescriptor = World::getDynamicModel()->getStateDescriptor();
	m_omega_g = pStateDescriptor.getVarIndex("omega_g");
	m_d_omega_g = pStateDescriptor.getVarIndex("d_omega_g");
	m_E_p = pStateDescriptor.getVarIndex("E_p");
	m_T_g = pStateDescriptor.getVarIndex("T_g");
	m_T_a_index = pStateDescriptor.getVarIndex("T_a");
	m_beta = pStateDescriptor.getVarIndex("beta");
	m_d_T_g = pStateDescriptor.getVarIndex("d_T_g");
	m_E_int_omega_g = pStateDescriptor.getVarIndex("E_int_omega_g");

	Descriptor& pActionDescriptor = World::getDynamicModel()->getActionDescriptor();

	m_a_beta = pActionDescriptor.getVarIndex("beta");
	m_a_T_g = pActionDescriptor.getVarIndex("T_g");
}

int WindTurbineBoukhezzarController::getNumOutputs()
{
	return 2;
}
int WindTurbineBoukhezzarController::getOutputActionIndex(int output)
{
	switch (output)
	{
	case 0: return m_a_beta;
	case 1: return m_a_T_g;
	default: throw std::exception("LQRController. Invalid action output given.");
	}

	return -1;
}


double WindTurbineBoukhezzarController::selectAction(const State *s,Action *a)
{
	//initialise m_lastT_g if we have to, controllers don't implement reset()
	if (SimionApp::get()->pWorld->getEpisodeSimTime() == 0)
		m_lastT_g = 0.0;// SimionApp::get()->pWorld->getDynamicModel()->getConstant("RatedGeneratorTorque");

	//d(Tg)/dt= (1/omega_g)*(C_0*error_P - (1/J_t)*(T_a*T_g - K_t*omega_g*T_g - T_g*T_g))
	//d(beta)/dt= K_p*(omega_ref - omega_g)
	//Original expression in Boukhezzar's paper:
	//double d_T_g= (1.0/omega_g)*(m_pC_0.get()*error_P - (T_a*m_lastT_g - m_K_t*omega_g*m_lastT_g 
	//	- m_lastT_g *m_lastT_g) / m_J_t );

	double omega_g= s->get(m_omega_g);
	double d_omega_g = s->get(m_d_omega_g);	
	double T_a= s->get(m_T_a_index);		
	
	double beta= s->get(m_beta);
	double E_p = s->get(m_E_p);
	double P_e = s->get("P_e");
	
	//Boukhezzar controller without making substitution: d_T_g= (-1/omega_g)(d_omega_g*T_g+C_0*Ep)
	double d_T_g = (-1.0/(omega_g*m_genElecEff))*(d_omega_g*m_lastT_g*m_genElecEff+m_pC_0.get()*s->get(m_E_p));

	d_T_g = std::min(std::max(s->getProperties(m_d_T_g).getMin(), d_T_g), s->getProperties(m_d_T_g).getMax());

	double e_omega_g = omega_g - World::getDynamicModel()->getConstant("RatedGeneratorSpeed");
	double desiredBeta = m_pKP.get()*e_omega_g +m_pKI.get()*s->get(m_E_int_omega_g);

	a->set(m_a_beta,desiredBeta);
	double nextT_g = m_lastT_g + d_T_g*SimionApp::get()->pWorld->getDT();
	a->set(m_a_T_g, nextT_g);
	m_lastT_g = nextT_g;

	return 1.0;
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
	m_PC_KK = DOUBLE_PARAM(pConfigNode,"PC_KK","Pitch angle were the the derivative of the...", 0.1099965);
	m_PC_KP= DOUBLE_PARAM(pConfigNode, "PC_KP","Proportional gain of the pitch controller",0.01882681);
	m_PC_KI= DOUBLE_PARAM(pConfigNode, "PC_KI","Integral gain of the pitch controller",0.008068634);
	m_PC_RefSpd= DOUBLE_PARAM(pConfigNode,"PC_RefSpd","Pitch control reference speed", 122.9096);

	m_IntSpdErr= 0.0;

	Descriptor& pStateDescriptor = World::getDynamicModel()->getStateDescriptor();
	m_omega_g = pStateDescriptor.getVarIndex("omega_g");

	m_E_p = pStateDescriptor.getVarIndex("E_p");
	m_T_g = pStateDescriptor.getVarIndex("T_g");
	m_beta = pStateDescriptor.getVarIndex("beta");

	Descriptor& pActionDescriptor = World::getDynamicModel()->getActionDescriptor();

	m_a_beta = pActionDescriptor.getVarIndex("beta");
	m_a_T_g = pActionDescriptor.getVarIndex("T_g");
}

int WindTurbineJonkmanController::getNumOutputs()
{
	return 2;
}
int WindTurbineJonkmanController::getOutputActionIndex(int output)
{
	switch (output)
	{
	case 0: return m_a_beta;
	case 1: return m_a_T_g;
	default: throw std::exception("LQRController. Invalid action output given.");
	}

	return -1;
}

double WindTurbineJonkmanController::selectAction(const State *s,Action *a)
{
	//Filter the generator speed
	double lowPassFilterAlpha;

	double time = SimionApp::get()->pWorld->getEpisodeSimTime();

	if (SimionApp::get()->pWorld->getEpisodeSimTime() == 0.0)
	{
		lowPassFilterAlpha= 1.0;
		m_GenSpeedF= s->get(m_omega_g);
		m_IntSpdErr = 0.0;
	}
	else
		lowPassFilterAlpha = exp(-SimionApp::get()->pWorld->getDT()*m_CornerFreq.get());

	m_GenSpeedF = (1.0 - lowPassFilterAlpha)*s->get(m_omega_g) + lowPassFilterAlpha*m_GenSpeedF;

	//TORQUE CONTROLLER
	double DesiredGenTrq;
	if ( (m_GenSpeedF >= m_ratedGenSpeed ) || (  s->get(m_beta) >= m_VS_Rgn3MP.get() ) )   //We are in region 3 - power is constant
		DesiredGenTrq = m_ratedPower/m_GenSpeedF;
	else if ( m_GenSpeedF <= m_VS_CtInSp.get() )							//We are in region 1 - torque is zero
		DesiredGenTrq = 0.0;
	else if ( m_GenSpeedF <  m_VS_Rgn2Sp.get() )                          //We are in region 1 1/2 - linear ramp in torque from zero to optimal
		DesiredGenTrq = m_VS_Slope15*( m_GenSpeedF - m_VS_CtInSp.get() );
	else if ( m_GenSpeedF <  m_VS_TrGnSp )                                      //We are in region 2 - optimal torque is proportional to the square of the generator speed
		DesiredGenTrq = m_VS_Rgn2K.get()*m_GenSpeedF*m_GenSpeedF;
	else                                                                       //We are in region 2 1/2 - simple induction generator transition region
		DesiredGenTrq = m_VS_Slope25*( m_GenSpeedF - m_VS_SySp   );

	DesiredGenTrq  = std::min( DesiredGenTrq, s->getProperties("T_g").getMax()  );   //Saturate the command using the maximum torque limit

	//we pass the desired generator torque instead of the rate
	a->set(m_a_T_g, DesiredGenTrq);

	//PITCH CONTROLLER
	double GK= 1.0 / (1.0 + s->get(m_beta) / m_PC_KK.get());

	//Compute the current speed error and its integral w.r.t. time; saturate the
	//  integral term using the pitch angle limits:
	double SpdErr= m_GenSpeedF - m_PC_RefSpd.get();                                 //Current speed error
	m_IntSpdErr = m_IntSpdErr + SpdErr*SimionApp::get()->pWorld->getDT();                           //Current integral of speed error w.r.t. time
	//Saturate the integral term using the pitch angle limits, converted to integral speed error limits
	m_IntSpdErr = std::min( std::max( m_IntSpdErr, s->getProperties(m_beta).getMin()/( GK*m_PC_KI.get() ) )
		, s->getProperties(m_beta).getMax()/( GK*m_PC_KI.get() ));
  
	//Compute the pitch commands associated with the proportional and integral  gains:
	double PitComP   = GK* m_PC_KP.get() * SpdErr; //Proportional term
	double PitComI   = GK* m_PC_KI.get() * m_IntSpdErr; //Integral term (saturated)

	//Superimpose the individual commands to getSample the total pitch command;
	//  saturate the overall command using the pitch angle limits:
	double PitComT   = PitComP + PitComI;                                     //Overall command (unsaturated)
	PitComT   = std::min( std::max( PitComT, s->getProperties(m_beta).getMin() )
		, s->getProperties(m_beta).getMax() );           //Saturate the overall command using the pitch angle limits

	//we pass the desired blade pitch angle to the world
	a->set(m_a_beta,PitComT);

	return 1.0;
}