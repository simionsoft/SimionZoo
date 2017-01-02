#include "stdafx.h"
#include "world-FAST.h"
#include "named-var-set.h"
#include "config.h"
#include "world.h"
#include "reward.h"
#include "app.h"

#include "../tools/WindowsUtils/Process.h"


#define FAST_EXE "../Release/fast_win32.exe"
#define FAST_CONFIG_FILE "fast-config.fst"

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
	addStateVariable("beta", "rad", 0.0, 1.570796);
	addStateVariable("d_beta", "rad/s", -0.1396263, 0.1396263);
	addStateVariable("T_g", "N/m", 0.0, 47402.91);
	addStateVariable("d_T_g", "N/m/s", -15000, 15000);
	addStateVariable("E_int_omega_r", "rad", -100.0, 100.0);
	//action handlers
	addActionVariable("d_beta", "rad/s", -0.1396263, 0.1396263);
	addActionVariable("d_T_g", "N/m/s", -15000.0, 15000.0);

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
	addConstant("RatedGeneratorTorque", 43093.55);
	addConstant("GearBoxRatio", 97.0);
	addConstant("ElectricalGeneratorEfficiency", 0.944); //%94.4
	addConstant("GeneratorInertia", 534116.0);			//kg*m^2
	addConstant("HubInertia", 115926.0);				//kg*m^2
	addConstant("DriveTrainTorsionalDamping", 6210000.0); //N*m/(rad/s)

	m_pRewardFunction->addRewardComponent(new CToleranceRegionReward("E_p", 100, 1.0));
	m_pRewardFunction->initialize();

	if (CSimionApp::get())
	{
		//config file
		CSimionApp::get()->pSimGod->registerInputFile("../Release/FAST_win32.exe");
		CSimionApp::get()->pSimGod->registerInputFile("../Release/MAP_win32.dll");
		CSimionApp::get()->pSimGod->registerInputFile("../config/world/FAST/configFileTemplate.fst");
		CSimionApp::get()->pSimGod->registerInputFile("../config/world/FAST/90m_12mps_twr.bts");
		CSimionApp::get()->pSimGod->registerInputFile("../config/world/FAST/Cylinder1.dat");
		CSimionApp::get()->pSimGod->registerInputFile("../config/world/FAST/Cylinder2.dat");
		CSimionApp::get()->pSimGod->registerInputFile("../config/world/FAST/DU21_A17.dat");
		CSimionApp::get()->pSimGod->registerInputFile("../config/world/FAST/DU25_A17.dat");
		CSimionApp::get()->pSimGod->registerInputFile("../config/world/FAST/DU30_A17.dat");
		CSimionApp::get()->pSimGod->registerInputFile("../config/world/FAST/DU35_A17.dat");
		CSimionApp::get()->pSimGod->registerInputFile("../config/world/FAST/DU40_A17.dat");
		CSimionApp::get()->pSimGod->registerInputFile("../config/world/FAST/NACA64_A17.dat");
		CSimionApp::get()->pSimGod->registerInputFile("../config/world/FAST/NRELOffshrBsline5MW_AeroDyn_blade.dat");
		CSimionApp::get()->pSimGod->registerInputFile("../config/world/FAST/NRELOffshrBsline5MW_BeamDyn.dat");
		CSimionApp::get()->pSimGod->registerInputFile("../config/world/FAST/NRELOffshrBsline5MW_BeamDyn_Blade.dat");
		CSimionApp::get()->pSimGod->registerInputFile("../config/world/FAST/NRELOffshrBsline5MW_Blade.dat");
		CSimionApp::get()->pSimGod->registerInputFile("../config/world/FAST/NRELOffshrBsline5MW_InflowWind_12mps.dat");
		CSimionApp::get()->pSimGod->registerInputFile("../config/world/FAST/NRELOffshrBsline5MW_Onshore_AeroDyn15.dat");
		CSimionApp::get()->pSimGod->registerInputFile("../config/world/FAST/NRELOffshrBsline5MW_Onshore_ElastoDyn.dat");
		CSimionApp::get()->pSimGod->registerInputFile("../config/world/FAST/NRELOffshrBsline5MW_Onshore_ElastoDyn_BDoutputs.dat");
		CSimionApp::get()->pSimGod->registerInputFile("../config/world/FAST/NRELOffshrBsline5MW_Onshore_ElastoDyn_Tower.dat");
		CSimionApp::get()->pSimGod->registerInputFile("../config/world/FAST/NRELOffshrBsline5MW_Onshore_ServoDyn.dat");
	}
}


void CFASTWindTurbine::deferredLoadStep()
{
	//bool bLoaded = loadTemplateConfigFile(FAST_CONFIG_TEMPLATE_FILE);

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
	FASTprocess.spawn(FAST_EXE);
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
	m_namedPipeServer.readToBuffer(s->getValueVector(), s->getNumVars() * sizeof(double));
}
