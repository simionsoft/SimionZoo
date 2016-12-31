#include "stdafx.h"
#include "world-FAST.h"
#include "named-var-set.h"
#include "config.h"
#include "world.h"
#include "reward.h"
#include "../tools/WindowsUtils/Process.h"


#define FAST_EXE_COMMAND_LINE "../FAST/bin/fast_win32.exe ../FAST/CertTest/TestRlSimion.fst"

CFASTWindTurbine::CFASTWindTurbine(CConfigNode* pConfigNode)
{
	METADATA("World", "FAST-Wind-turbine");

	addStateVariable("T_a", "N/m", 0.0, 400000.0);
	addStateVariable("P_a", "W", 0.0, 1600000.0);
	addStateVariable("P_s", "W", 500000.0, 700000.0);
	addStateVariable("P_e", "W", 500000.0, 700000.0);
	addStateVariable("E_p", "W", -100000, 100000);
	addStateVariable("v", "m/s", 1.0, 50.0);
	addStateVariable("omega_r", "rad/s", 2.39823, 6.39823);
	addStateVariable("omega_g", "rad/s", 0.0, 200.0);
	addStateVariable("E_omega_r", "rad/s", -4.0, 4.0);
	addStateVariable("d_omega_r", "rad/s^2", -2.0, 2.0);
	addStateVariable("beta", "rad", -0.3490658504, 0.5235987756);
	addStateVariable("d_beta", "rad/s", -0.1745329252, 0.1745329252);
	addStateVariable("T_g", "N/m", 100000, 162000);
	addStateVariable("d_T_g", "N/m/s", -100000, 100000);
	addStateVariable("E_int_omega_r", "rad", -100.0, 100.0);
	//action handlers
	addActionVariable("d_beta", "rad/s", -10, 10);
	addActionVariable("d_T_g", "N/m/s", -100000, 100000);

	//model constants
	addConstant("RatedPower", 5e6);				//W
	addConstant("HubHeight", 90);				//m
	addConstant("CutInWindSpeed", 3.0);			//m/s
	addConstant("RatedWindSpeed", 11.4);		//m/s
	addConstant("CutOutWindSpeed", 25.0);		//m/s
	addConstant("CutInRotorSpeed", 0.72256);	//6.9 rpm
	addConstant("CutOutRotorSpeed", 1.26711);	//12.1 rpm
	addConstant("RatedTipSpeed", 8.377);		//80 rpm
	addConstant("RatedGeneratorSpeed", 122.91); //1173.7 rpm
	addConstant("GearBoxRatio", 97.0);
	addConstant("ElectricalGeneratorEfficiency",0.944); //%94.4
	addConstant("GeneratorInertia", 534116.0);			//kg*m^2
	addConstant("HubInertia", 115926.0);				//kg*m^2
	addConstant("DriveTrainTorsionalDamping", 6210000.0); //N*m/(rad/s)

	m_pRewardFunction->addRewardComponent(new CToleranceRegionReward("E_p", 100, 1.0));
	m_pRewardFunction->initialize();

	m_namedPipeServer.openNamedPipeServer(DIMENSIONAL_PORTAL_PIPE_NAME);
}

CFASTWindTurbine::~CFASTWindTurbine()
{
	m_namedPipeServer.closeServer();
}



void CFASTWindTurbine::reset(CState *s)
{
	CProcess FASTprocess;
	//spawn the FAST exe file
	FASTprocess.spawn(FAST_EXE_COMMAND_LINE);
	//wait for the client (FASTDimensionalPortalDLL) to connect
	m_namedPipeServer.waitForClientConnection();
	//receive(s)
	m_namedPipeServer.readToBuffer(s->getValueVector(), s->getNumVars() * sizeof(double));
}

void CFASTWindTurbine::executeAction(CState *s,const CAction *a,double dt)
{
	//send(a)
	//here we have to cheat the compiler (const). We don't want to, but we have to
	double* pActionValues = ((CAction*)a)->getValueVector(); 
	m_namedPipeServer.writeBuffer(pActionValues, a->getNumVars() * sizeof(double));

	//receive(s')
	m_namedPipeServer.readToBuffer(s, s->getNumVars() * sizeof(double));
}
