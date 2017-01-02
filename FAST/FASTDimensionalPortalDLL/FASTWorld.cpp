#include "Stdafx.h"
#include "FASTWorld.h"
#include "../../RLSimion-Lib/world-FAST.h"
#if _DEBUG
	#pragma comment(lib,"../../Debug/WindowsUtils.lib")
	#pragma comment(lib,"../../Debug/RLSimion-Lib.lib")
	#pragma comment(lib,"../../Debug/tinyxml2.lib")
#else
	#pragma comment(lib,"../../Release/WindowsUtils.lib")
	#pragma comment(lib,"../../Release/RLSimion-Lib.lib")
	#pragma comment(lib,"../../Release/tinyxml2.lib")
#endif

#define INITIAL_TORQUE 0.0

bool g_bDummyTest = false;	//for debugging

FASTWorld::FASTWorld()
{
	CFASTWindTurbine *pModel= new CFASTWindTurbine(0);
	m_stateDescriptor = pModel->getStateDescriptor();
	m_actionDescriptor = pModel->getActionDescriptor();
	
	for (int i= 0; i<pModel->getNumConstants(); i++)
	{
		m_constants[pModel->getConstantName(i)]= pModel->getConstant(i);
	}

	J_r = m_constants["HubInertia"];
	J_g = m_constants["GeneratorInertia"];
	n_g = m_constants["GearBoxRatio"];

	m_pS = m_stateDescriptor.getInstance();
	m_pA = m_actionDescriptor.getInstance();
}

bool FASTWorld::retrieveStateVariables(float* FASTdata, bool bFirstTime)
{
	//Time = avrSWAP[2 - 1];
	double currentTime= (double) FASTdata[1];
	if (bFirstTime)
		m_lastTime = currentTime;
	m_elapsedTime = currentTime - m_lastTime;

	//Rotor speed: omega_r
	//RotorSpeed = avrSWAP[21 - 1];
	double last_omega_r = m_pS->getValue("omega_r");
	double omega_r = (double)FASTdata[20];
	m_pS->setValue("omega_r",omega_r);

	//Rotor speed acceleration: d_omega_r
	double d_omega_r= 0.0;
	if (!bFirstTime) 
		d_omega_r = (omega_r - last_omega_r) / m_elapsedTime;
	m_pS->setValue("d_omega_r", d_omega_r);

	//Generator speed: omega_g		//GenSpeed = avrSWAP[20 - 1];
	m_pS->setValue("omega_g", (double)FASTdata[19]);

	//Wind speed: v					//HorWindV = avrSWAP[27 - 1];
	m_pS->setValue("v", (double)FASTdata[26]);

	//Mesaured electrical power: P_e
	m_pS->setValue("P_e", (double)FASTdata[13]);

	//Power error: E_p
	m_pS->setValue("E_p", (double)FASTdata[13] - m_constants["RatedPower"]);

	//Aerodynamic torque: T_a
	m_pS->setValue("T_a", (J_r + J_g*n_g*n_g)*d_omega_r + m_pS->getValue("E_p") / m_pS->getValue("omega_g"));

	//Generator torque: T_g
	if (bFirstTime)
		m_pS->setValue("T_g", m_constants["RatedGeneratorTorque"]);
	else
		//we have to track it because FAST doesn't actually calculate this value
		m_pS->setValue("T_g", m_pS->getValue("T_g") + m_pA->getValue("d_T_g")* m_elapsedTime);
	//Blade pitch: beta
	//The blade pitch is measured instead of tracking it
	m_pS->setValue("beta", (double)FASTdata[4 - 1]);

	if (m_elapsedTime<CONTROL_DT)
		return false;

	m_lastTime = currentTime;
	return true;
}

void FASTWorld::setActionVariables(float* FASTdata)
{
	//d_t_g
	FASTdata[34] = 1.0;          //Generator contactor status: 1=main (high speed) variable-speed generator
	FASTdata[55] = 0.0;          //Torque override: 0=yes
	double demanded_T_g = m_pS->getValue("T_g") + m_pA->getValue("d_T_g")*m_elapsedTime;
	demanded_T_g = std::min(std::max(demanded_T_g, m_pS->getProperties("T_g").getMin()), m_pS->getProperties("T_g").getMax());
	FASTdata[46] = (float)demanded_T_g;// (float)m_last_T_g;   //Demanded generator torque

	//d_beta
	double demanded_beta= m_pS->getValue("beta") + m_pA->getValue("d_beta")*m_elapsedTime;
	demanded_beta = std::min(std::max(demanded_beta, m_pS->getProperties("beta").getMin()), m_pS->getProperties("beta").getMax());
       
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

void FASTWorld::connectToNamedPipeServer()
{
	if (!g_bDummyTest)
		m_namedPipeClient.connectToServer(DIMENSIONAL_PORTAL_PIPE_NAME);
}

void FASTWorld::disconnectFromNamedPipeServer()
{
	if (!g_bDummyTest)
		m_namedPipeClient.closeConnection();
}

void FASTWorld::sendState()
{
	double *pValues= m_pS->getValueVector();
	if (!g_bDummyTest)
		m_namedPipeClient.writeBuffer(pValues, m_pS->getNumVars()*sizeof(double));
	else
	{
		printf("State=[");
		for (int i = 0; i < m_pS->getNumVars(); i++)
			printf("%.2f ", pValues[i]);
		printf("]\n");
	}
}

void FASTWorld::receiveAction()
{
	double *pValues = m_pA->getValueVector();
	if (!g_bDummyTest)
		m_namedPipeClient.readToBuffer(pValues, m_pA->getNumVars() * sizeof(double));
	else
	{
		printf("Action=[");
		for (int i = 0; i < m_pA->getNumVars(); i++)
			printf("%.2f ", pValues[i]);
		printf("]\n");
	}
}