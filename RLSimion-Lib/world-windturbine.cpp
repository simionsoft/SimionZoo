#include "stdafx.h"
#include "world.h"
#include "world-windturbine.h"
#include "setpoint.h"
#include "states-and-actions.h"
#include "experiment.h"
#include "globals.h"
#include "xml-parameters.h"

//[1]
//"Torque and pitch angle control for VSWT in all operating regimes"
//Adel Merabet, Jogendra Thongam, Jason Gu

//[2]
//"Pitch and torque control strategy for VSWT"
//L. Lupu, B. Boukhezzar, H. Siguerdidjane

//[3]
//"Nonlinear Control of a Variable-Speed Wind Turbine Using a Two-Mass Model"
//Boubekeur Boukhezzar and Houria Siguerdidjane

//[4]
//"Multivariable control strategy for variable speed, variable pitch wind turbines"
//Boubekeur Boukhezzar and L. Lupu and H. Siguerdidjane, M. Hand

//[5]
//"Power control design for Variable-speed Wind Turbines"
//Yolanda Vidal, Leonardo Acho, Ningsu Luo, Mauricio Zapateiro and Francesc Pozo

//[6]
//FAST: test 13


#define MIN_BLADE_ANGLE (-5*2*3.1415/360.0) // radians //degrees [4]
#define MAX_BLADE_ANGLE (30*2*3.1415/360.0) // radians //degrees [4]  

#define MAX_AERODYNAMIC_POWER  1600000.0 //W
#define MAX_AERODYNAMIC_TORQUE 400000.0 //N/m

#define NOMINAL_ROTOR_SPEED 4.39823//rad/s - 42 rpm[thesis boukhezzar]
#define MAX_ROTOR_SPEED NOMINAL_ROTOR_SPEED +2.0
#define MIN_ROTOR_SPEED NOMINAL_ROTOR_SPEED -2.0

//////////////////////////////////////////////////////////
#define INITIAL_ROTOR_SPEED NOMINAL_ROTOR_SPEED //rad/s nominal-speed [6]
#define INITIAL_ACTION_TORQUE 142000.0//-36.279653322202989 //[4] N/m

#define MAX_GENERATOR_TORQUE_INC 100000


#define P_e_nom 600000.0 //W [4]
#define HUB_HEIGHT 36.6 //m [4]
#define N_g 43.165 //[4]
#define D 43.3 //m [4]
//#define J_r 3.25e5 //kg*m^2 [3]
//#define K_r 27.36 //N*m/rad/s [3]
#define rho 1.29 //kg/m^3 [3]
//#define J_g 34.400 //kg*m^2 [3]
//#define K_g 0.2 //N*m/rad/s [3]
#define K_t 400.0 //N*m/rad/s [thesis boukhezzar]
#define J_t 3.92e5 //kg*m^2


#define NUM_ACTION_VARS 2

#define MIN_GENERATOR_TORQUE 100000 //from log
#define MAX_GENERATOR_TORQUE 162000 //N*m [2]
#define MAX_ACTION_BLADE_RATE ((10.0*2*3.14159265)/360.0)// //rad/s +/- 10º/s

#define MIN_INTEGRATIVE_ROTOR_SPEED_ERROR -100
#define MAX_INTEGRATIVE_ROTOR_SPEED_ERROR 100

#define MAX_INITIAL_BLADE_ANGLE MIN_BLADE_ANGLE//((30.0*2*3.14159265)/360.0)
#define MIN_INITIAL_BLADE_ANGLE MAX_BLADE_ANGLE//((0.0*2*3.14159265)/360.0)

#define MAX_WIND_SPEED 50.0
#define MAX_TSR 15.0
#define NUM_BETA_SAMPLES 10000
#define NUM_WIND_SPEED_SAMPLES 1000
#define NUM_TSR_SAMPLES 1000


double C_p(double lambda, double beta) //[1]
{
	double lambda_i,c_p;
	beta= beta*360.0/(2*3.14159265);

	lambda_i= 1.0/((1.0/(lambda+0.08*beta)) - 0.035/(pow(beta,3.0)+1.0));
	//lambda_i= 1.0/((1.0/(lambda+0.089)) - 0.035/(pow(beta,3.0)+1.0));
	if (lambda_i==0.0)
		return 0.00001;

	double e= exp(-16.5/lambda_i);
	c_p= 0.5*(116.0/lambda_i - 0.4*beta -5.0)*exp(-16.5/lambda_i);
	//c_p= 0.5*(116.0/lambda_i - 0.4*(beta -5.0))*exp(-16.5/lambda_i);
	return std::max(0.00001,c_p);
}
double C_q(double lambda, double beta)
{
	if (lambda==0.0)
		return 0.0;

	return C_p(lambda,beta)/lambda;
}
double AerodynamicTorque(double tip_speed_ratio,double beta, double wind_speed)
{
	double cq= C_q(tip_speed_ratio,beta);

	//Ta= 0.5 * rho * pi * R^3 * C_q(lambda,beta) * v^2
	double torque= 0.5*rho*3.14159265*pow(D*0.5,3.0)*cq*wind_speed*wind_speed;
	return torque;
}

double AerodynamicPower(double tip_speed_ratio,double beta, double wind_speed)
{
	double cp= C_p(tip_speed_ratio,beta);

	//Pa= 0.5 * rho * pi * R^2 * C_p(lambda,beta) * v^3
	double power= 0.5*rho*3.14159265*(D*0.5)*(D*0.5)*cp*pow(wind_speed,3.0);
	return power;
}
double AerodynamicPower(double cp, double wind_speed)
{
	//Pa= 0.5 * rho * pi * R^2 * C_p(lambda,beta) * v^3
	double power= 0.5*rho*3.14159265*(D*0.5)*(D*0.5)*cp*pow(wind_speed,3.0);
	return power;
}



void FindSuitableParameters(double initial_wind_speed,double initial_rotor_speed
							,double &initial_torque,double &initial_blade_angle)
{
	double c_p,best_c_p= -1000.0;
	double beta, tsr;
	double torque, best_torque= -1000000000.0;
//	double desired_c_p= (initial_power*2)/(rho*3.14159265*(D*0.5)*(D*0.5)*pow(initial_wind_speed,3.0));
//	double check= AerodynamicPower(desired_c_p,initial_wind_speed);

	initial_blade_angle= 0.01;
	tsr= initial_rotor_speed*D*0.5/initial_wind_speed;

	double minInitialBeta= MIN_INITIAL_BLADE_ANGLE;
	double initialBetaRange= MAX_INITIAL_BLADE_ANGLE-MIN_INITIAL_BLADE_ANGLE;

	for (int j= 0; j<NUM_BETA_SAMPLES; j++)
	{
		beta= minInitialBeta + (double)j * (initialBetaRange/(double)NUM_BETA_SAMPLES);
		
		torque= AerodynamicTorque(tsr,beta,initial_wind_speed);
		c_p= C_p(tsr,beta);

		if (fabs(initial_torque-torque)<fabs(initial_torque-best_torque))
		{
			initial_blade_angle= beta;
			best_c_p= c_p;
			//initial_rotor_speed= initial_wind_speed*tsr/(D*0.5);
			best_torque= torque;
		}
	}
	initial_torque= best_torque;
}


CWindTurbine::CWindTurbine(tinyxml2::XMLElement *pParameters)
: CDynamicModel(XMLParameters::getConstString(pParameters->FirstChildElement("World-Definition")))
{
	/*
	//STATE VARIABLES
	m_pStateDescriptor= new CState(14);
	
	m_pStateDescriptor->setProperties(0,"P_s",500000.0,700000.0);//0.0
	m_pStateDescriptor->setProperties(1,"P_e",500000.0,700000.0);//0.0
	m_pStateDescriptor->setProperties(2,"P_a",0.0,1600000.0);//0.0
	m_pStateDescriptor->setProperties(3,"v",1.0,50.0);//0.0
	m_pStateDescriptor->setProperties(4,"T_a",0.0,400000.0);//0.0
	m_pStateDescriptor->setProperties(5,"omega_r",2.39823,6.39823);//4.39823
	m_pStateDescriptor->setProperties(6,"d_omega_r",-2.0,2.0);//0.0
	m_pStateDescriptor->setProperties(7,"beta",-0.3490658504,0.5235987756);//0.0
	m_pStateDescriptor->setProperties(8,"d_beta",-0.1745329252,0.1745329252);//0.0
	m_pStateDescriptor->setProperties(9,"T_g",100000,162000);//0.0
	m_pStateDescriptor->setProperties(10,"d_T_g",-100000,100000);//0.0
	m_pStateDescriptor->setProperties(11,"E_p",-100000.0,100000.0);//0.0
	m_pStateDescriptor->setProperties(12,"E_omega_r",-4.0,4.0);//0.0
	m_pStateDescriptor->setProperties(13,"E_int_omega_r",-100.0,100.0);//0.0

	//ACTION VARIABLES
	m_pActionDescriptor= new CAction(2);
	m_pActionDescriptor->setProperties(0,"d_beta",-0.1745329252,0.1745329252);
	m_pActionDescriptor->setProperties(1,"d_T_g",-100000,100000);*/

	//load all the wind data files
	m_currentDataFile = 0;

	//evaluation file
	m_pEvaluationWindData = 
		new CHHFileSetPoint(pParameters->FirstChildElement("Evaluation-Wind-Data")->GetText());

	//training files
	tinyxml2::XMLElement* trainingWindFiles = pParameters->FirstChildElement("Training-Wind-Data");
	m_numDataFiles = XMLParameters::countChildren(trainingWindFiles, "File");
	m_pTrainingWindData = new CSetPoint*[m_numDataFiles];

	tinyxml2::XMLElement* pElement = trainingWindFiles->FirstChildElement("File");
	for (int i = 0; i<m_numDataFiles; i++)
	{
		m_pTrainingWindData[i] = new CHHFileSetPoint(pElement->GetText());
		pElement = pElement->NextSiblingElement("File");
	}

	//m_pWindData = new CHHFileSetPoint(pParameters->getParameter("WIND_DATA_FILE")->getStringPtr());
	m_pPowerSetpoint= new CFileSetPoint(pParameters->FirstChildElement("Power-Set-Point")->GetText());

	double initial_T_g= P_e_nom/NOMINAL_ROTOR_SPEED;
	m_initial_torque= initial_T_g + K_t*NOMINAL_ROTOR_SPEED;
	m_initial_blade_angle= 0.0;
}

CWindTurbine::~CWindTurbine()
{
	for (int i = 0; i < m_numDataFiles; i++)
	{
		delete m_pTrainingWindData[i];
	}
	delete[] m_pTrainingWindData;
	delete m_pEvaluationWindData;
	delete m_pPowerSetpoint;
}





void CWindTurbine::reset(CState *s)
{
	if (RLSimion::g_pExperiment->isEvaluationEpisode())
		m_pCurrentWindData = m_pEvaluationWindData;
	else
		m_pCurrentWindData = m_pTrainingWindData[rand() % m_numDataFiles];

	double initial_wind_speed = m_pCurrentWindData->getPointSet(0.0);
	double initial_rotor_speed= NOMINAL_ROTOR_SPEED;


	if (m_initial_blade_angle==0.0)
	{
		printf("Calculating initial torque and blade angle parameters...\n");
		FindSuitableParameters(initial_wind_speed,initial_rotor_speed,m_initial_torque,m_initial_blade_angle);
		printf("T_g= %f     //    Beta= %f\n",m_initial_torque,m_initial_blade_angle);
	}

	double tsr= initial_rotor_speed*D*0.5/initial_wind_speed;

	s->setValue("T_a",AerodynamicTorque(tsr,m_initial_blade_angle,initial_wind_speed));
	s->setValue("P_a", s->getValue("T_a")*initial_rotor_speed);
	s->setValue("P_s",m_pPowerSetpoint->getPointSet(0.0));

	s->setValue("P_e", P_e_nom);
	s->setValue("E_p",s->getValue("P_e") - s->getValue("P_s"));
	s->setValue("v",initial_wind_speed);

	s->setValue("omega_r",initial_rotor_speed);
	s->setValue("E_omega_r",initial_rotor_speed-NOMINAL_ROTOR_SPEED);
	s->setValue("d_omega_r",0.0);
	s->setValue("beta",m_initial_blade_angle);
	s->setValue("d_beta",0.0);
	s->setValue("T_g",P_e_nom/initial_rotor_speed);
	s->setValue("d_T_g",0.0);
	s->setValue("E_int_omega_r", 0.0);
}


void CWindTurbine::executeAction(CState *s, CAction *a, double dt)
{
	s->setValue("P_s",m_pPowerSetpoint->getPointSet(CWorld::getT()));
	s->setValue("v", m_pCurrentWindData->getPointSet(CWorld::getT()));

	//beta= beta + d(beta)/dt
	double beta= s->getValue("beta");
	
	//P_e= T_g*omega_r
	double omega_r= s->getValue("omega_r");
	double T_g= s->getValue("T_g");

	s->setValue("P_e",T_g*omega_r);
	s->setValue("E_p",s->getValue("P_e") - s->getValue("P_s"));

	double tip_speed_ratio= (s->getValue("omega_r")*D*0.5) / s->getValue("v");
	
	//C_p(tip_speed_ratio,blade_angle)
	//double power_coef=C_p(tip_speed_ratio,beta);
	//P_a= 0.5*rho*pi*R^2*C_p(lambda,beta)v^3
	double P_a= AerodynamicPower(tip_speed_ratio,beta,s->getValue("v"));
	s->setValue("P_a",P_a);
	//T_a= P_a/omega_r
	double T_a= P_a/omega_r;//AerodynamicTorque(tip_speed_ratio,beta,state->getContinuousState(DIM_v));
	s->setValue("T_a",T_a);
	s->setValue("E_omega_r",s->getValue("omega_r")-NOMINAL_ROTOR_SPEED);
	s->setValue("E_int_omega_r",s->getValue("E_int_omega_r")+ s->getValue("E_omega_r")*dt);

	//d(omega_r)= (T_a - K_t*omega_r - T_g) / J_t
	double d_omega_r= (T_a - K_t*omega_r - T_g) / J_t;

	s->setValue("d_omega_r",d_omega_r);

	s->setValue("omega_r",omega_r + d_omega_r*dt);
	
	s->setValue("d_T_g", a->getValue("d_T_g"));
	T_g= s->getValue("T_g") + a->getValue("d_T_g")*dt;
	s->setValue("d_beta", a->getValue("d_beta"));
	beta= s->getValue("beta") + a->getValue("d_beta")*dt;

	s->setValue("T_g",T_g);
	s->setValue("beta", beta);
}