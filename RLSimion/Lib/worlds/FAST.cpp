/*
	SimionZoo: A framework for online model-free Reinforcement Learning on continuous
	control problems

	Copyright (c) 2016 SimionSoft. https://github.com/simionsoft

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

#include "FAST.h"
#include "../../Common/named-var-set.h"
#include "../config.h"
#include "world.h"
#include "../reward.h"
#include "../app.h"
#include "../logger.h"
#include "../experiment.h"
#include "../../../tools/System/Process.h"
#include "../../../tools/System/CrossPlatform.h"
#include <string>
#include <stdio.h>
using namespace std;

#define FAST_FAILURE_REWARD -100.0 //reward given in case a simulation error arises

#define FAST_WIND_CONFIG_FILE "NRELOffshrBsline5MW_InflowWind.dat"
#define FAST_WIND_CONFIG_TEMPLATE_FILE "../config/world/FAST/NRELOffshrBsline5MW_InflowWindTemplate.dat"
#define TURBSIM_TEMPLATE_CONFIG_FILE "../config/world/TurbSim/TurbSimConfigTemplate.inp"
#define FAST_TEMPLATE_CONFIG_FILE "../config/world/FAST/configFileTemplate.fst"
#define FAST_CONFIG_FILE "fast-config.fst"
#define SERVO_MODULE_TEMPLATE_FILE "../config/world/FAST/NRELOffshrBsline5MW_Onshore_ServoDyn_Template.dat"
#define SERVO_MODULE_CONFIG_FILE "../config/world/FAST/NRELOffshrBsline5MW_Onshore_ServoDyn.dat"
#define PORTAL_CONFIG_FILE "FASTDimensionalPortal.xml"
#define DIMENSIONAL_PORTAL_PIPE_NAME "FASTDimensionalPortal"
#define DIMENSIONAL_PORTAL_DLL "../bin/FASTDimensionalPortal.dll"

#define TRAINING_WIND_BASE_FILE_NAME "training-wind-file-"
#define EVALUATION_WIND_BASE_FILE_NAME "eval-wind-file-"

FASTWindTurbine::FASTWindTurbine(ConfigNode* pConfigNode)
{
	//This class is used both in the DimensionalPortal DLL (pConfigNode will be nullptr) 
	//and in RLSimion (pConfigNode will not be nullptr)
	METADATA("World", "FAST-Wind-turbine");
	
	//This if allows prevents erros when the constructor is called without a config node
	if (pConfigNode)
	{
		m_trainingMeanWindSpeeds = MULTI_VALUE_SIMPLE_PARAM<DOUBLE_PARAM, double>(pConfigNode, "Training-Mean-Wind-Speeds", "Mean wind speeds used in training episodes", 12.5);
		m_evaluationMeanWindSpeeds = MULTI_VALUE_SIMPLE_PARAM<DOUBLE_PARAM, double>(pConfigNode, "Evaluation-Mean-Wind-Speeds", "Mean wind speeds in evaluation episodes", 12.5);
	}

	//model constants
	addConstant("RatedPower", 5e6);				//W
	addConstant("HubHeight", 90);				//m
	addConstant("CutInWindSpeed", 3.0);			//m/s
	addConstant("RatedWindSpeed", 11.4);		//m/s
	addConstant("CutOutWindSpeed", 25.0);		//m/s
	addConstant("CutInRotorSpeed", 0.72256);	//6.9 rpm
	addConstant("CutOutRotorSpeed", 1.26711);	//12.1 rpm
	addConstant("RatedRotorSpeed", 1.26711);	//12.1 rpm
	addConstant("RatedTipSpeed", 8.377);		//80 rpm
	addConstant("RatedGeneratorSpeed", 122.91); //1173.7 rpm
	addConstant("RatedGeneratorTorque", 43093.55);
	addConstant("GearBoxRatio", 97.0);
	addConstant("ElectricalGeneratorEfficiency", 0.944); //%94.4
	addConstant("TotalTurbineInertia", 43784725); //J_t= J_r + n_g^2*J_g= 38759228 + 5025497 
	addConstant("GeneratorInertia", 534.116);			//kg*m^2
	addConstant("HubInertia", 115.926);				//kg*m^2
	addConstant("TotalTurbineTorsionalDamping", 6210000.0); //N*m/(rad/s)
	addConstant("RotorDiameter", 128.0); //m
	addConstant("AirDensity", 1.225);	//kg/m^3

	addStateVariable("T_a", "N/m", 0.0, 10000000.0);
	addStateVariable("P_a", "W", 0.0, 16000000.0);
	addStateVariable("P_s", "W", 0.0, 6e6);
	addStateVariable("P_e", "W", 0.0, 10e6);
	addStateVariable("E_p", "W", -10e6, 10e6);
	addStateVariable("v", "m/s", 1.0, 50.0);
	addStateVariable("omega_r", "rad/s", 0.0, 6.0);
	addStateVariable("d_omega_r", "rad/s^2", -10.0, 10.0);
	addStateVariable("E_omega_r", "rad/s", -4.0, 4.0);
	addStateVariable("omega_g", "rad/s", 0.0, 200.0);
	addStateVariable("d_omega_g", "rad/s^2", -50.0, 50.0);
	addStateVariable("E_omega_g", "rad/s", -122.0, 122.0);
	addStateVariable("beta", "rad", 0.0, 1.570796);
	addStateVariable("d_beta", "rad/s", -0.1396263, 0.1396263);
	addStateVariable("T_g", "N/m", 0.0, 47402.91);
	addStateVariable("d_T_g", "N/m/s", -15000, 15000);
	addStateVariable("E_int_omega_r", "rad/s", -1.0e6, 1.0e6);
	addStateVariable("E_int_omega_g", "rad/s", -1.0e6, 1.0e6);
	addStateVariable("theta", "rad", -3.1415, 3.1415, true); //roll angle of the blades in the rotor

	//action handlers
	addActionVariable("beta", "rad", 0.0, 1.570796);
	addActionVariable("T_g", "N/m", 0.0, 47402.91);

	ToleranceRegionReward* pToleranceReward = new ToleranceRegionReward("E_p", 500000.0, 1.0);
	//pToleranceReward->setMin(-1000.0);
	m_pRewardFunction->addRewardComponent(pToleranceReward);
	m_pRewardFunction->initialize();

	if (SimionApp::get())
	{
		//input/output files
		SimionApp::get()->registerInputFile("../bin/FASTDimensionalPortal.dll");
		SimionApp::get()->registerInputFile("../bin/openfast_Win32.exe");
		SimionApp::get()->registerInputFile("../bin/MAP_win32.dll");
		SimionApp::get()->registerInputFile("../bin/TurbSim.exe");

		//FAST data files
		SimionApp::get()->registerInputFile("../config/world/FAST/configFileTemplate.fst");
		SimionApp::get()->registerInputFile("../config/world/FAST/Cylinder1.dat");
		SimionApp::get()->registerInputFile("../config/world/FAST/Cylinder2.dat");
		SimionApp::get()->registerInputFile("../config/world/FAST/DU21_A17.dat");
		SimionApp::get()->registerInputFile("../config/world/FAST/DU25_A17.dat");
		SimionApp::get()->registerInputFile("../config/world/FAST/DU30_A17.dat");
		SimionApp::get()->registerInputFile("../config/world/FAST/DU35_A17.dat");
		SimionApp::get()->registerInputFile("../config/world/FAST/DU40_A17.dat");
		SimionApp::get()->registerInputFile("../config/world/FAST/NACA64_A17.dat");
		SimionApp::get()->registerInputFile("../config/world/FAST/Cylinder1_coords.txt");
		SimionApp::get()->registerInputFile("../config/world/FAST/Cylinder2_coords.txt");
		SimionApp::get()->registerInputFile("../config/world/FAST/DU21_A17_coords.txt");
		SimionApp::get()->registerInputFile("../config/world/FAST/DU25_A17_coords.txt");
		SimionApp::get()->registerInputFile("../config/world/FAST/DU30_A17_coords.txt");
		SimionApp::get()->registerInputFile("../config/world/FAST/DU35_A17_coords.txt");
		SimionApp::get()->registerInputFile("../config/world/FAST/DU40_A17_coords.txt");
		SimionApp::get()->registerInputFile("../config/world/FAST/NACA64_A17_coords.txt");
		SimionApp::get()->registerInputFile("../config/world/FAST/AeroData_Cylinder1.dat");
		SimionApp::get()->registerInputFile("../config/world/FAST/AeroData_Cylinder2.dat");
		SimionApp::get()->registerInputFile("../config/world/FAST/AeroData_DU21_A17.dat");
		SimionApp::get()->registerInputFile("../config/world/FAST/AeroData_DU25_A17.dat");
		SimionApp::get()->registerInputFile("../config/world/FAST/AeroData_DU30_A17.dat");
		SimionApp::get()->registerInputFile("../config/world/FAST/AeroData_DU35_A17.dat");
		SimionApp::get()->registerInputFile("../config/world/FAST/AeroData_DU40_A17.dat");
		SimionApp::get()->registerInputFile("../config/world/FAST/AeroData_NACA64_A17.dat");
		SimionApp::get()->registerInputFile("../config/world/FAST/NRELOffshrBsline5MW_AeroDyn_blade.dat");
		SimionApp::get()->registerInputFile("../config/world/FAST/NRELOffshrBsline5MW_BeamDyn.dat");
		SimionApp::get()->registerInputFile("../config/world/FAST/NRELOffshrBsline5MW_BeamDyn_Blade.dat");
		SimionApp::get()->registerInputFile("../config/world/FAST/NRELOffshrBsline5MW_Blade.dat");
		SimionApp::get()->registerInputFile("../config/world/FAST/NRELOffshrBsline5MW_InflowWindTemplate.dat");
		SimionApp::get()->registerInputFile("../config/world/FAST/NRELOffshrBsline5MW_Onshore_AeroDyn15.dat");
		SimionApp::get()->registerInputFile("../config/world/FAST/NRELOffshrBsline5MW_Onshore_ElastoDyn.dat");
		SimionApp::get()->registerInputFile("../config/world/FAST/NRELOffshrBsline5MW_Onshore_ElastoDyn_BDoutputs.dat");
		SimionApp::get()->registerInputFile("../config/world/FAST/NRELOffshrBsline5MW_Onshore_ElastoDyn_Tower.dat");
		SimionApp::get()->registerInputFile("../config/world/FAST/NRELOffshrBsline5MW_Onshore_ServoDyn.dat");
		
		//TurbSim data files
		SimionApp::get()->registerInputFile("../config/world/TurbSim/TurbSimConfigTemplate.inp");
		SimionApp::get()->registerInputFile("../config/world/TurbSim/EventData/Event00000.dat");
		SimionApp::get()->registerInputFile("../config/world/TurbSim/EventData/Event01917.dat");
		SimionApp::get()->registerInputFile("../config/world/TurbSim/EventData/Event02515.dat");
		SimionApp::get()->registerInputFile("../config/world/TurbSim/EventData/Event02800.dat");
		SimionApp::get()->registerInputFile("../config/world/TurbSim/EventData/Event02893.dat");
		SimionApp::get()->registerInputFile("../config/world/TurbSim/EventData/Event03167.dat");
		SimionApp::get()->registerInputFile("../config/world/TurbSim/EventData/Event03219.dat");
		SimionApp::get()->registerInputFile("../config/world/TurbSim/EventData/Event03326.dat");
		SimionApp::get()->registerInputFile("../config/world/TurbSim/EventData/Event03384.dat");
		SimionApp::get()->registerInputFile("../config/world/TurbSim/EventData/Event03613.dat");
		SimionApp::get()->registerInputFile("../config/world/TurbSim/EventData/Event03692.dat");
		SimionApp::get()->registerInputFile("../config/world/TurbSim/EventData/Event03868.dat");
		SimionApp::get()->registerInputFile("../config/world/TurbSim/EventData/Event04110.dat");
		SimionApp::get()->registerInputFile("../config/world/TurbSim/EventData/Event04434.dat");
		SimionApp::get()->registerInputFile("../config/world/TurbSim/EventData/Event04702.dat");
		SimionApp::get()->registerInputFile("../config/world/TurbSim/EventData/Event99999.dat");
		SimionApp::get()->registerInputFile("../config/world/TurbSim/EventData/Events.les");
		SimionApp::get()->registerInputFile("../config/world/TurbSim/EventData/Events.xtm");
	}
}


void FASTWindTurbine::deferredLoadStep()
{
	string outConfigFileName, exeFileName;
	string commandLine;

	//Generate templated TurbSim wind profiles
	bool bLoaded = m_TurbSimConfigTemplate.load(TURBSIM_TEMPLATE_CONFIG_FILE);
	if (bLoaded)
	{
		Logger::logMessage(MessageType::Info, "Generating TurbSim wind files");

		exeFileName = string("../bin/TurbSim.exe");

		//evaluation wind files
		for (unsigned int i = 0; i < m_evaluationMeanWindSpeeds.size(); i++)
		{
			outConfigFileName = string(SimionApp::get()->getOutputDirectory()) + string("/")
				+ string(EVALUATION_WIND_BASE_FILE_NAME) + to_string(i) + string(".inp");
			m_TurbSimConfigTemplate.instantiateConfigFile(outConfigFileName.c_str()
				, SimionApp::get()->pExperiment->getEpisodeLength() + 30.0	//AnalysisTime
				, SimionApp::get()->pExperiment->getEpisodeLength() + 30.0	//UsableTime
				, m_evaluationMeanWindSpeeds[i]->get());						//URef

			commandLine = exeFileName + string(" ") + outConfigFileName;
			TurbSimProcess.spawn((char*)(commandLine).c_str(), true);
		}
		//set the number of episodes per evaluation
		SimionApp::get()->pExperiment->setNumEpisodesPerEvaluation((int)m_evaluationMeanWindSpeeds.size());

		//training wind files
		for (unsigned int i = 0; i < m_trainingMeanWindSpeeds.size(); i++)
		{
			outConfigFileName = string(SimionApp::get()->getOutputDirectory()) + string("/") 
				+ string(TRAINING_WIND_BASE_FILE_NAME) + to_string(i) + string(".inp");
			m_TurbSimConfigTemplate.instantiateConfigFile(outConfigFileName.c_str()
				, SimionApp::get()->pExperiment->getEpisodeLength()+30.0	//AnalysisTime
				, SimionApp::get()->pExperiment->getEpisodeLength()+30.0	//UsableTime
				, m_trainingMeanWindSpeeds[i]->get());						//URef

			commandLine = exeFileName + string(" ") + outConfigFileName;
			TurbSimProcess.spawn((char*)(commandLine).c_str(), true);
		}
	}
	//Load the template used to tell FAST which wind file to use
	m_FASTWindConfigTemplate.load(FAST_WIND_CONFIG_TEMPLATE_FILE);
	
	//copy input .dat files to experiment directory to avoid problems with FAST adding base config file's directory
	commandLine= string("copy \"../config/world/FAST/*.dat\" ") + string(SimionApp::get()->getOutputDirectory());
	CrossPlatform::ForceUseFolderCharacter(commandLine);
	system(commandLine.c_str());

	//copy input .txt files to experiment directory to avoid problems with FAST adding base config file's directory
	commandLine = string("copy \"../config/world/FAST/*.txt\" ") + string(SimionApp::get()->getOutputDirectory());
	CrossPlatform::ForceUseFolderCharacter(commandLine);
	system(commandLine.c_str());

	//copy DLL to avoid problems with relative paths in the config file
	commandLine = string("copy " + string(DIMENSIONAL_PORTAL_DLL) + string(" ") + string(SimionApp::get()->getOutputDirectory()));
	CrossPlatform::ForceUseFolderCharacter(commandLine);
	system(commandLine.c_str());

	Logger::logMessage(MessageType::Info, "Input files copied");
}

FASTWindTurbine::~FASTWindTurbine()
{
	m_namedPipeServer.closeServer();
	Logger::logMessage(MessageType::Info, "Closed connection to FASTDimensionalPortal");
}



void FASTWindTurbine::reset(State *s)
{
	string outConfigFileName,windFile;
	FILE *pOutConfigFile;

	//Check if a previous instance of FAST is running and, if it is, stop the process
	//This may happen for slight inaccuracies of DT
	if (FASTprocess.isRunning())
		FASTprocess.stop();
	//If the named pipe is already open, close it
	m_namedPipeServer.closeServer();

	//Open the named pipe server
	//FASTDimensionalPortal.xml -> used to pass the pipe's name to the dll
	bool pipeServerOpened = m_namedPipeServer.openUniqueNamedPipeServer(DIMENSIONAL_PORTAL_PIPE_NAME);
	if (pipeServerOpened)
	{
		outConfigFileName = string(SimionApp::get()->getOutputDirectory()) + string("/")
			+ string(PORTAL_CONFIG_FILE);
		CrossPlatform::Fopen_s(&pOutConfigFile, outConfigFileName.c_str(), "w");
		if (pOutConfigFile)
		{
			CrossPlatform::Fprintf_s(pOutConfigFile, "<?xml version=\"1.0\"?>\n<FAST-DIMENSIONAL-PORTAL>\n  <PIPE-NAME>%s</PIPE-NAME>\n</FAST-DIMENSIONAL-PORTAL>"
				, m_namedPipeServer.getPipeFullName());
			fclose(pOutConfigFile);
			Logger::logMessage(MessageType::Info, "FASTDimensionalPortal.dll: pipe server created");
		}
		else Logger::logMessage(MessageType::Error, (string("Couldn't create config file: ") + outConfigFileName).c_str());
	}
	else Logger::logMessage(MessageType::Error, "Couldn't open named pipe server");


	//Instantiate the templated FAST config file
	bool bLoaded = m_FASTConfigTemplate.load(FAST_TEMPLATE_CONFIG_FILE);
	if (bLoaded)
	{
		//choose the wind file
		size_t index;
		if (SimionApp::get()->pExperiment->isEvaluationEpisode())
		{
			//evaluation wind file
			index = (size_t) SimionApp::get()->pExperiment->getEpisodeInEvaluationIndex()-1; //names are 0-based and indices are 1-based
			windFile = string(EVALUATION_WIND_BASE_FILE_NAME)
				+ to_string(index) + string(".bts");
		}
		else
		{
			//training wind file
			index = ((size_t)rand()) % m_trainingMeanWindSpeeds.size();
			windFile = string(TRAINING_WIND_BASE_FILE_NAME)
				+ to_string(index) + string(".bts");
		}
		outConfigFileName = string(SimionApp::get()->getOutputDirectory()) + string("/")
			+ string(FAST_WIND_CONFIG_FILE);
		m_FASTWindConfigTemplate.instantiateConfigFile(outConfigFileName.c_str(), windFile.c_str());

		Logger::logMessage(MessageType::Info, "Instantiating FAST config file");
		outConfigFileName = string(SimionApp::get()->getOutputDirectory()) + string("/")
			+ string(FAST_CONFIG_FILE);
		m_FASTConfigTemplate.instantiateConfigFile(outConfigFileName.c_str()
			, SimionApp::get()->pExperiment->getEpisodeLength()
			, SimionApp::get()->pWorld->getDT()
			, FAST_WIND_CONFIG_FILE
		);
	}
	else
		Logger::logMessage(MessageType::Error, "Couldn't instantiate config file: ../config/world/FAST/configFileTemplate.fst");


	//spawn the FAST exe file
	string commandLine;

	commandLine= string("../bin/openfast_Win32.exe");

	commandLine+= string(" ") + string(SimionApp::get()->getOutputDirectory())+ string("/")
		+ string(FAST_CONFIG_FILE);
	bool bSpawned= FASTprocess.spawn((char*)(commandLine).c_str());
	//wait for the client (FASTDimensionalPortal DLL) to connect
	if (bSpawned && FASTprocess.isRunning())
	{
		Logger::logMessage(MessageType::Info, "Waiting for the client to connect");
		m_namedPipeServer.waitForClientConnection();
		Logger::logMessage(MessageType::Info, "Client connected");
		//receive(s)
		m_namedPipeServer.readToBuffer(s->getValueVector(), (int) s->getNumVars() * sizeof(double));
	}
	else
	{
		Logger::logMessage(MessageType::Info, "FAST process ended prematurely");
		SimionApp::get()->pExperiment->setTerminalState();
		return;
	}
}

void FASTWindTurbine::executeAction(State *s,const Action *a,double dt)
{
	//Check FAST is still running
	if (!FASTprocess.isRunning())
	{
		Logger::logMessage(MessageType::Info, "FAST process ended prematurely");
		SimionApp::get()->pExperiment->setTerminalState();
		m_pRewardFunction->override(FAST_FAILURE_REWARD);
		return;
	}

	//send(a)
	//here we have to cheat the compiler (const). We don't want to, but we have to
	double* pActionValues = ((Action*)a)->getValueVector();
	int numBytesToWrite = sizeof(double) * 2; //hard-coded because there might be auxiliary actions added by the controller
	int numBytesWritten= m_namedPipeServer.writeBuffer(pActionValues, numBytesToWrite);
	if (numBytesToWrite != numBytesWritten)
	{
		Logger::logMessage(MessageType::Info, "FAST process ended prematurely");
		SimionApp::get()->pExperiment->setTerminalState();
		m_pRewardFunction->override(FAST_FAILURE_REWARD);
		return;
	}

	//receive(s')
	size_t numBytesToRead = s->getNumVars() * sizeof(double);
	size_t numBytesRead= m_namedPipeServer.readToBuffer(s->getValueVector(), (int) numBytesToRead);
	if (numBytesToRead!=numBytesRead)
	{
		Logger::logMessage(MessageType::Info, "FAST process ended prematurely");
		SimionApp::get()->pExperiment->setTerminalState();
		m_pRewardFunction->override(FAST_FAILURE_REWARD);
		return;
	}
}
