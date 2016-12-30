#include "Stdafx.h"
#include "FASTWorld.h"

#define INITIAL_TORQUE 40000.0

FASTWorld::FASTWorld()
{
	m_stateDescriptor.addVariable("T_a", "N/m", 0.0, 400000.0);
	m_stateDescriptor.addVariable("P_a", "W", 0.0, 1600000.0);
	m_stateDescriptor.addVariable("P_s", "W", 500000.0, 700000.0);
	m_stateDescriptor.addVariable("P_e", "W", 500000.0, 700000.0);
	m_stateDescriptor.addVariable("E_p", "W", -100000, 100000);
	m_stateDescriptor.addVariable("v", "m/s", 1.0, 50.0);
	m_stateDescriptor.addVariable("omega_r", "rad/s", 2.39823, 6.39823);
	m_stateDescriptor.addVariable("omega_g", "rad/s", 2.39823, 1000.39823); //what range of values does omega_g take???
	m_stateDescriptor.addVariable("E_omega_r", "rad/s", -4.0, 4.0);
	m_stateDescriptor.addVariable("d_omega_r", "rad/s^2", -2.0, 2.0);
	m_stateDescriptor.addVariable("beta", "rad", -0.3490658504, 0.5235987756);
	m_stateDescriptor.addVariable("d_beta", "rad/s", -0.1745329252, 0.1745329252);
	m_stateDescriptor.addVariable("T_g", "N/m", 100000, 162000);
	m_stateDescriptor.addVariable("d_T_g", "N/m/s", -100000, 100000);
	m_stateDescriptor.addVariable("E_int_omega_r", "rad", -100.0, 100.0);
	//action handlers
	m_actionDescriptor.addVariable("d_beta", "rad/s", -10, 10);
	m_actionDescriptor.addVariable("d_T_g", "N/m/s", -100000, 100000);

	m_pS = m_stateDescriptor.getInstance();
	m_pA = m_actionDescriptor.getInstance();
}

void FASTWorld::retrieveStateVariables(float* FASTdata, bool bFirstTime)
{
	//Time = avrSWAP[2 - 1];
	double currentTime= (double) FASTdata[1];
	if (bFirstTime)
		m_lastTime = currentTime;
	m_elapsedTime = currentTime - m_lastTime;

	//Rotor speed: omega_r
	//RotorSpeed = avrSWAP[21 - 1];
	double omega_r = (double)FASTdata[20];
	m_pS->setValue("omega_r",omega_r);

	//Rotor speed acceleration: d_omega_r
	double d_omega_r= 0.0;
	if (!bFirstTime) d_omega_r = (omega_r - m_last_omega_r) / m_elapsedTime;
	m_pS->setValue("d_omega_r", d_omega_r);

	//Generator speed: omega_g		//GenSpeed = avrSWAP[20 - 1];
	m_pS->setValue("omega_g", (double)FASTdata[19]);

	//Wind speed: v					//HorWindV = avrSWAP[27 - 1];
	m_pS->setValue("v", (double)FASTdata[26]);

	//Mesaured power: E_p
	m_pS->setValue("E_p", (double)FASTdata[13]);

	//Aerodynamic torque: T_a
	//T_a = (J_r + J_g*n_g*n_g)*d_omega_r + measuredPower / GenSpeedF;

	//Generator torque: T_g
	//We use d_T_g as the control output, so we have to keep track of the current T_g
	if (bFirstTime) m_last_T_g= INITIAL_TORQUE;
	m_pS->setValue("T_g", m_last_T_g + m_pA->getValue("d_T_g")* m_elapsedTime);
	//Blade pitch: beta
	//The blade pitch is measured instead of tracking it
	m_pS->setValue("beta", (double)FASTdata[4 - 1]);

	m_lastTime = currentTime;
	m_last_omega_r = omega_r;
}

void FASTWorld::setActionVariables(float* FASTdata)
{
	//d_t_g
	FASTdata[34] = 1.0;          //Generator contactor status: 1=main (high speed) variable-speed generator
	FASTdata[55] = 0.0;          //Torque override: 0=yes
	FASTdata[46] = (float)m_last_T_g;   //Demanded generator torque

	//d_beta
	double desiredBeta= m_pS->getValue("beta") + m_pA->getValue("d_beta")*m_elapsedTime;
	desiredBeta = std::min(std::max(desiredBeta, m_pS->getProperties("beta").getMin()), m_pS->getProperties("beta").getMax());
       
	FASTdata[54] = 0.0;       //Pitch override: 0=yes

	FASTdata[41] = (float)desiredBeta; //Use the command angles of all blades if using individual pitch
	FASTdata[42] = (float)desiredBeta; //"
	FASTdata[43] = (float)desiredBeta; //"

	FASTdata[44] = (float)desiredBeta; //Use the command angle of blade 1 if using collective pitch
}