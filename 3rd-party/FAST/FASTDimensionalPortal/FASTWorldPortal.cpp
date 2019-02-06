#include "Stdafx.h"
#include "FASTWorldPortal.h"
#include "../../../RLSimion/Lib/worlds/FAST.h"



FASTWorldPortal::FASTWorldPortal()
{
	FASTWindTurbine *pModel= new FASTWindTurbine(0);
	m_stateDescriptor = pModel->getStateDescriptor();
	m_actionDescriptor = pModel->getActionDescriptor();
	
	for (int i= 0; i<pModel->getNumConstants(); i++)
	{
		m_constants[pModel->getConstantName(i)]= pModel->getConstant(i);
	}

	J_r = m_constants["HubInertia"];
	J_g = m_constants["GeneratorInertia"];
	n_g = m_constants["GearBoxRatio"];

	s = m_stateDescriptor.getInstance();
	a = m_actionDescriptor.getInstance();
}

void FASTWorldPortal::retrieveStateVariables(float* FASTdata, bool bFirstTime)
{
	//Time = avrSWAP[2 - 1];
	double currentTime= (double) FASTdata[1];
	if (bFirstTime)
	{
		m_lastTime = currentTime;
		m_elapsedTime = 0.00000001; //this avoids zero divisions in the first step
	}
	else
		m_elapsedTime = currentTime - m_lastTime;

	//Rotor speed: omega_r
	//RotorSpeed = avrSWAP[21 - 1];
	double last_omega_r = s->get("omega_r");
	double omega_r = (double)FASTdata[20];
	s->set("omega_r",omega_r);

	//Rotor speed acceleration: d_omega_r
	double d_omega_r= 0.0;
	if (!bFirstTime)
		d_omega_r = (omega_r - last_omega_r) / m_elapsedTime;
	s->set("d_omega_r", d_omega_r);

	s->set("E_omega_r", s->get("omega_r") - m_constants["RatedRotorSpeed"]);
	s->set("E_int_omega_r", s->get("E_omega_r")*m_elapsedTime);

	//Generator speed: omega_g		//GenSpeed = avrSWAP[20 - 1];
	double last_omega_g = s->get("omega_g");
	double omega_g = (double)FASTdata[19];
	s->set("omega_g", omega_g);

	double d_omega_g = 0.0;
	if (!bFirstTime)
		d_omega_g = (omega_g - last_omega_g) / m_elapsedTime; //to avoid zero division
	s->set("d_omega_g", d_omega_g);

	s->set("E_omega_g", s->get("omega_g") - m_constants["RatedGeneratorSpeed"]);
	s->set("E_int_omega_g", s->get("E_omega_g")*m_elapsedTime);

	//Wind speed: v					//HorWindV = avrSWAP[27 - 1];
	s->set("v", (double)FASTdata[26]);

	//Measured electrical power: P_e
	s->set("P_e", (double)FASTdata[14]);

	//Power error: E_p
	s->set("E_p", s->get("P_e") - m_constants["RatedPower"]);

	//Aerodynamic torque: T_a
	//https://wind.nrel.gov/forum/wind/viewtopic.php?t=961
	//T_aero = (J_rotor + J_gen*GBRatio^2)*Alpha + T_gen*GbRatio
	s->set("T_a", (J_r + J_g*n_g*n_g)*d_omega_r + s->get("T_g")*n_g);

	//Aerodynamic power: P_a
	s->set("P_a", s->get("T_a")*s->get("omega_r"));

	//Power setpoint: P_s, assumed to be the rated power
	s->set("P_s", m_constants["RatedPower"]);

	//Generator torque: T_g
	if (bFirstTime)
	{
		m_prevGenTorque = m_constants["RatedGeneratorTorque"];
		s->set("T_g", m_constants["RatedGeneratorTorque"]);
		s->set("d_T_g", 0.0);
	}
	else m_prevGenTorque = s->get("T_g");
	
	//Blade pitch: beta
	double beta = (double)FASTdata[3];

	if (bFirstTime)
	{
		m_prevPitch = s->get("beta");
		s->set("beta", beta);
		s->set("d_beta", 0.0);
	}
	else m_prevPitch = beta;

	if (!bFirstTime)
		s->set("theta", s->get("theta") + s->get("omega_r")*m_elapsedTime);
	else s->set("theta", 0.0);

	m_lastTime = currentTime;
}

void FASTWorldPortal::setActionVariables(float* FASTdata, bool bFirstTime)
{
	//We saturate here the desired pitch/torque rates to avoid discrepancies between RLSimion::D_t and FAST::DT

	//T_g
	FASTdata[34] = 1.0;          //Generator contactor status: 1=main (high speed) variable-speed generator
	FASTdata[55] = 0.0;          //Torque override: 0=yes

	double demanded_T_g = a->get("T_g");
	if (!bFirstTime)
	{
		s->set("d_T_g",(demanded_T_g - m_prevGenTorque) / m_elapsedTime);
		demanded_T_g = s->get("T_g") + s->get("d_T_g")*m_elapsedTime;
		demanded_T_g = std::min(std::max(demanded_T_g, s->getProperties("T_g")->getMin()), s->getProperties("T_g")->getMax());
		s->set("T_g", demanded_T_g);
	}
	FASTdata[46] = (float)demanded_T_g;// (float)m_last_T_g;   //Demanded generator torque

	//beta: first iteration we don't update it to make sure elapsedTime is properly calculated
	double demanded_beta = a->get("beta");
	if (!bFirstTime)
		s->set("d_beta", (demanded_beta - m_prevPitch) / m_elapsedTime);
	else
		s->set("d_beta", 0.0);

	demanded_beta = s->get("beta") + s->get("d_beta")*m_elapsedTime;
	demanded_beta = std::min(std::max(demanded_beta, s->getProperties("beta")->getMin()), s->getProperties("beta")->getMax());
	s->set("beta", demanded_beta);
	
	FASTdata[54] = 0.0;       //Pitch override: 0=yes

	FASTdata[41] = (float)demanded_beta; //Use the command angles of all blades if using individual pitch
	FASTdata[42] = (float)demanded_beta; //"
	FASTdata[43] = (float)demanded_beta; //"

	FASTdata[44] = (float)demanded_beta; //Use the command angle of blade 1 if using collective pitch


	//unused variables
	FASTdata[36 - 1] = 0.0; //Shaft brake status: 0=off
	FASTdata[41 - 1] = 0.0; //Demanded yaw actuator torque
	FASTdata[46 - 1] = 0.0; //Demanded pitch rate (Collective pitch)
	FASTdata[48 - 1] = 0.0; //Demanded nacelle yaw rate
	FASTdata[65 - 1] = 0.0; //Number of variables returned for logging
	FASTdata[72 - 1] = 0.0; //Generator start-up resistance
	FASTdata[79 - 1] = 0.0; //Request for loads: 0=none
	FASTdata[80 - 1] = 0.0; //Variable slip current status
	FASTdata[81 - 1] = 0.0; //Variable slip current demand
}

bool  FASTWorldPortal::connectToNamedPipeServer(const char* pipeName)
{
	return m_namedPipeClient.connectToServer(pipeName,false); //bAddPrefix= false because we are reading the full name from xml config file
}

void FASTWorldPortal::disconnectFromNamedPipeServer()
{
	m_namedPipeClient.closeConnection();
}

void FASTWorldPortal::sendState()
{
	double *pValues= s->getValueVector();
	m_namedPipeClient.writeBuffer(pValues, (int) s->getNumVars()*sizeof(double));
}

void FASTWorldPortal::receiveAction()
{
	double *pValues = a->getValueVector();
	m_namedPipeClient.readToBuffer(pValues, (int) a->getNumVars() * sizeof(double));
}