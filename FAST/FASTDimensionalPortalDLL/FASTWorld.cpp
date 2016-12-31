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

#define INITIAL_TORQUE 40000.0

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
	m_pS->setValue("T_a", (J_r + J_g*n_g*n_g)*d_omega_r + m_pS->getValue("E_p") / m_pS->getValue("omega_g"));

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