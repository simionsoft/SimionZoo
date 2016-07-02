#include "stdafx.h"
#include "logger.h"
#include "world.h"
#include "named-var-set.h"
#include "parameters.h"
#include "stats.h"
#include "globals.h"
#include "timer.h"
#include "app.h"
#include "utils.h"
#include "SimGod.h"

void *CLogger::m_hLogFile= 0;

CLASS_INIT(CLogger)
{
	if (!pParameters) return;

	const char* boolStr;
	ENUM_VALUE(boolStr,Boolean,"Log-eval-episodes", "False","Log evaluation episodes?");
	m_bLogEvaluationEpisodes = !strcmp(boolStr, "True");
	ENUM_VALUE(boolStr, Boolean, "Log-training-episodes", "False","Log training episodes?");
	m_bLogTrainingEpisodes = !strcmp(boolStr, "True");
	ENUM_VALUE(boolStr,Boolean, "Log-eval-experiment", "True","Log the stats of the evaluation episodes?");
	m_bLogEvaluationExperiment = !strcmp(boolStr, "True");
	ENUM_VALUE(boolStr,Boolean, "Log-training-experiment", "False","Log the stats of the training episodes?");
	m_bLogTrainingExperiment = !strcmp(boolStr, "True");

	CONST_DOUBLE_VALUE(m_logFreq,"Log-Freq", 0.25,"Log frequency. Simulation time in seconds.");

	//_mkdir(m_outputDir);


	m_pEpisodeTimer = new CTimer();
	m_pExperimentTimer = new CTimer();
	m_lastLogSimulationT = 0.0;
	
	END_CLASS();
}

#define OUTPUT_LOG_FILENAME "experiment-log.xml"
void CLogger::setLogDirectory(const char* xmlFilePath)
{
	if (!xmlFilePath)
		throw(std::exception("CLogger. No output directory provided."));

	char outputDir[MAX_FILENAME_LENGTH];// char* outputDir = new char[strlen(xmlFilePath) + 1];
	strcpy_s(outputDir, MAX_FILENAME_LENGTH, xmlFilePath);// strlen(xmlFilePath) + 1, xmlFilePath);

	int i = strlen(outputDir)-1;
	while (i > 0 && outputDir[i] != '/' && outputDir[i] != '\\')
		i--;

	if (i > 0) outputDir[i] = 0;
	
	char fullLogFilename[MAX_FILENAME_LENGTH];

	sprintf_s(fullLogFilename, MAX_FILENAME_LENGTH, "%s/%s", outputDir, OUTPUT_LOG_FILENAME);
	//we register the name for input/output stuff
	CApp::get()->SimGod.registerOutputFile(fullLogFilename);

	openLogFile(fullLogFilename);
}

CLogger::CLogger()
{
	//default values for safety

	m_bLogEvaluationEpisodes = false;
	m_bLogEvaluationExperiment = false;
	m_bLogTrainingEpisodes = true;
	m_bLogTrainingExperiment = false;
	m_logFreq = 0.0;
	m_pEpisodeTimer = 0;
	m_pExperimentTimer = 0;
}


CLogger::~CLogger()
{
	if (m_pExperimentTimer) delete m_pExperimentTimer;
	if (m_pEpisodeTimer) delete m_pEpisodeTimer;

	closeOutputPipe();

	for (auto it = m_stats.begin(); it != m_stats.end(); it++)
		delete *it;

	closeLogFile();
}


/*
void CLogger::getLogFilename(char* buffer, int bufferSize, bool episodeLog, bool evaluation,unsigned int index)
{
	if (episodeLog)
	{
		if (!evaluation)
			sprintf_s(buffer, bufferSize, "%s/log-train-epis-%d.txt", m_outputDir, index);
		else
			sprintf_s(buffer, bufferSize, "%s/log-eval-epis-%d.txt", m_outputDir, index);
	}
	else
	{
		if (!evaluation)
			sprintf_s(buffer, bufferSize, "%s/log-train-exp.txt", m_outputDir);
		else
			sprintf_s(buffer, bufferSize, "%s/log-eval-exp.txt", m_outputDir);
	}
}*/
/*
FILE* CLogger::openLogFile(bool create)// , bool episodeLog, bool evalEpisode, unsigned int episodeIndex)
{
	FILE* pFile;
	//char filename[1024];

	//getLogFilename(filename, 1024, episodeLog, evalEpisode, episodeIndex);

	if (create)
	{
		fopen_s(&pFile, m_outputLogFilename, "w");
		if (pFile)
		{
			//writeLogFileHeader(pFile, episodeLog, evalEpisode);
			fclose(pFile);
		}
		return 0;
	}
	else
	{
		fopen_s(&pFile, filename, "a");
		return pFile;
	}
}*/
/*
void CLogger::writeLogFileHeader(FILE* pFile, bool episodeLog, bool evaluationLog)
{
	if (!pFile) return;
	if (episodeLog)
		fprintf_s(pFile, "Time ");
	else
		fprintf_s(pFile, "Episode Experiment-Time Episode-Duration ");

	for (auto it = m_stats.begin(); it != m_stats.end(); it++)
	{
		fprintf_s(pFile, "%s\\%s ", (*it)->getKey(), (*it)->getSubkey());
	}
	fprintf_s(pFile, "\n");
}*/
/*
void CLogger::writeExperimentLogData(bool evalEpisode, unsigned int episodeIndex)
{
	if (isExperimentTypeLogged(evalEpisode))
	{
		FILE* pFile = openLogFile(false, false, evalEpisode, episodeIndex);

		if (pFile)
		{
			//output the episode index, the elapsed time since the experiment started and the last episode's time length
			//__int64 currentCounter;
			//QueryPerformanceCounter((LARGE_INTEGER*)&currentCounter);
			double experimentTime = m_pExperimentTimer->getElapsedTime(false);//(double)(currentCounter - m_experimentStartCounter) / (double)m_counterFreq;
			double episodeDuration = m_pEpisodeTimer->getElapsedTime(false);// (double)(currentCounter - m_episodeStartCounter) / (double)m_counterFreq;

			fprintf(pFile, "%d %.3f %.3f ", episodeIndex, experimentTime, episodeDuration);
			//output the stats
			for (auto it = m_stats.begin(); it != m_stats.end(); it++)
			{
				fprintf(pFile, "%.3f(%.3f) ", (*it)->getStatsInfo()->getAvg(), (*it)->getStatsInfo()->getStdDev());
			}
			fprintf(pFile, "\n");
			fclose(pFile);
		}
	}
}
*/
/*
void CLogger::writeEpisodeLogData(bool evalEpisode, unsigned int episodeIndex)
{
	if (isEpisodeTypeLogged(evalEpisode))
	{
		FILE* pFile = openLogFile(false, true, evalEpisode, episodeIndex);

		if (pFile)
		{
			fprintf(pFile, "%.3f ", CApp::get()->World.getT());

			for (auto it = m_stats.begin(); it != m_stats.end(); it++)
			{
				fprintf(pFile, "%.3f ", (*it)->getValue());
			}
			fprintf(pFile, "\n");
			fclose(pFile);
		}
	}
}*/


bool CLogger::isEpisodeTypeLogged(bool evalEpisode)
{
	return (evalEpisode && m_bLogEvaluationEpisodes) || (!evalEpisode && m_bLogTrainingEpisodes);
}

bool CLogger::isExperimentTypeLogged(bool evalEpisode)
{
	return (evalEpisode && m_bLogEvaluationExperiment) || (!evalEpisode && m_bLogTrainingExperiment);
}

void CLogger::firstEpisode(bool evalEpisode)
{
	//set episode start time
	m_pEpisodeTimer->startTimer();

	writeLogBuffer(Output::LogFile, "<Experiment>\n");
}

void CLogger::lastEpisode(bool evalEpisode)
{
	writeLogBuffer(Output::LogFile, "</Experiment>\n");
}

char* CLogger::getExperimentTypeName(bool evalEpisode)
{
	if (evalEpisode) return "Evaluation";
	else return "Training";
}

void CLogger::firstStep(bool evalEpisode, unsigned int episodeIndex)
{
	//initialise the episode reward
	m_episodeRewardSum = 0.0;

	if (!isEpisodeTypeLogged(evalEpisode)) return;

	//set episode start time
	m_pEpisodeTimer->startTimer();

	m_lastLogSimulationT = 0.0;
	//reset stats
	for (auto it = m_stats.begin(); it != m_stats.end(); it++)
		(*it)->reset();

	char buffer[BUFFER_SIZE];
	sprintf_s(buffer, BUFFER_SIZE, "  <Episode Index=\"%d\" Type=\"%s\">\n", episodeIndex, getExperimentTypeName(evalEpisode));
	writeLogBuffer(Output::LogFile, buffer);
}

void CLogger::lastStep(bool evalEpisode, unsigned int episodeIndex, unsigned int numEpisodes, unsigned int numSteps)
{
	//in case this is the last step of an evaluation episode, we log it and send the info to the host if there is one
	char buffer[BUFFER_SIZE];
	if (evalEpisode && numEpisodes>0 && numSteps>0)
	{
		sprintf_s(buffer, BUFFER_SIZE, "%f,%f", (double)(episodeIndex - 1) / (double)(numEpisodes - 1)
			, m_episodeRewardSum / (double)numSteps);
		logMessage(MessageType::Evaluation, buffer);
	}
	if (!isEpisodeTypeLogged(evalEpisode)) return;

	writeLogBuffer(Output::LogFile, "  </Episode>\n");
}

void CLogger::timestep(bool evalEpisode, unsigned int episodeIndex,CState* s, CAction* a, CState* s_p, CReward* r)
{
	//we add the scalar reward in evaluation episodes for monitoring purposes, no matter if we are logging this type of episode or not
	if (evalEpisode) m_episodeRewardSum += r->getSumValue();

	if (!isEpisodeTypeLogged(evalEpisode)) return;

	bool bLog = isEpisodeTypeLogged(evalEpisode);

	//update experiment stats
	for (auto iterator = m_stats.begin(); iterator != m_stats.end(); iterator++)
	{
		(*iterator)->addExperimentSample();
	}

	//output episode log data
	if (bLog && (CApp::get()->World.getStepStartT() - m_lastLogSimulationT >= m_logFreq))
	{
		logStepData(episodeIndex, s, a, s_p, r);
		//writeEpisodeLogData(evalEpisode, episodeIndex);
		m_lastLogSimulationT = CApp::get()->World.getStepStartT();
	}
}

void CLogger::logStepData(unsigned int stepIndex,CState* s, CAction* a, CState* s_p, CReward* r)
{
	char buffer[BUFFER_SIZE];
	buffer[0] = 0;
	double simTime = CApp::get()->World.getT();
	double realTime = m_pExperimentTimer->getElapsedTime();
	sprintf_s(buffer, BUFFER_SIZE, "    <Step Index=\"%d\" SimTime=\"%f\" RealTime=\"%f\">\n", stepIndex, simTime, realTime);
	
	logNamedVarSetToBuffer(buffer, s, "State");
	logNamedVarSetToBuffer(buffer, a, "Action");
	logNamedVarSetToBuffer(buffer, r, "Reward");
	logStatsToBuffer(buffer);
	strcat_s(buffer, BUFFER_SIZE, "    </Step>\n");

	writeLogBuffer(Output::LogFile, buffer);
}


void CLogger::logStatsToBuffer(char* pOutBuffer)
{
	char buffer[BUFFER_SIZE];

	strcat_s(pOutBuffer, BUFFER_SIZE, "    <Stats>\n");
	for (auto iterator = m_stats.begin(); iterator != m_stats.end(); iterator++)
	{
		sprintf_s(buffer, BUFFER_SIZE,"      <%s>%f</%s>\n", (*iterator)->getKey(), (*iterator)->getValue(), (*iterator)->getKey());
		strcat_s(pOutBuffer, BUFFER_SIZE, buffer);
	}
	strcat_s(pOutBuffer, BUFFER_SIZE, "    </Stats>\n");
}
void CLogger::logNamedVarSetToBuffer(char* pOutBuffer, CNamedVarSet* pVarSet, const char* id)
{
	char buffer[BUFFER_SIZE];
	sprintf_s(buffer, BUFFER_SIZE, "    <%s>\n", id);
	strcat_s(pOutBuffer, BUFFER_SIZE, buffer);
	for (int i = 0; i < pVarSet->getNumVars(); i++)
	{
		sprintf_s(buffer, BUFFER_SIZE,"        <%s>%f</%s>\n",pVarSet->getName(i), pVarSet->getValue(i),pVarSet->getName(i));
		strcat_s(pOutBuffer, BUFFER_SIZE, buffer);
	}
	sprintf_s(buffer, BUFFER_SIZE, "    </%s>\n", id);
	strcat_s(pOutBuffer, BUFFER_SIZE, buffer);
}


void CLogger::addVarToStats(const char* key, const char* subkey, double* variable)
{
	//all stats added by the loaded classes are calculated
	//if (m_pParameters->getChild(key))
	m_stats.push_back(new CStats(key, subkey, (void*) variable, Double));
}

void CLogger::addVarToStats(const char* key, const char* subkey, int* variable)
{
	//all stats added by the loaded classes are calculated
	//if (m_pParameters->getChild(key))
	m_stats.push_back(new CStats(key, subkey, (void*) variable, Int));
}

void CLogger::addVarToStats(const char* key, const char* subkey, unsigned int* variable)
{
	//all stats added by the loaded classes are calculated
	//if (m_pParameters->getChild(key))
	m_stats.push_back(new CStats(key, subkey, (void*)variable, UnsignedInt));
}

void CLogger::addVarSetToStats(const char* key, CNamedVarSet* varset)
{
	for (int i = 0; i < varset->getNumVars(); i++)
	{
		m_stats.push_back(new CStats(key, varset->getName(i), varset->getValuePtr(i), Double));
	}
}

//WINDOWS-SPECIFIC STUFF
#include <windows.h>
#include <string>

void CLogger::openLogFile(const char* logFilename)
{
	size_t convertedChars;

	wchar_t w_filename[BUFFER_SIZE];

	mbstowcs_s(&convertedChars, w_filename, BUFFER_SIZE, logFilename, BUFFER_SIZE);

	m_hLogFile = CreateFile(w_filename, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0,0);
}
void CLogger::closeLogFile()
{
	if (m_hLogFile) CloseHandle(m_hLogFile);
}

void CLogger::writeLogBuffer(Output output, const char* pBuffer)
{
	unsigned long numBytesWritten = 0;
	if (output==Output::LogFile && m_hLogFile)
		WriteFile(m_hLogFile, pBuffer, strlen(pBuffer),&numBytesWritten,0);
	else if (output==Output::Pipe && m_outputPipe)
		WriteFile(m_outputPipe, pBuffer, strlen(pBuffer),&numBytesWritten,0);
}

MessageOutputMode CLogger::m_messageOutputMode = MessageOutputMode::Console;
void* CLogger::m_outputPipe = 0;

void CLogger::logMessage(MessageType type, const char* message)
{
	char messageLine[1024];

	if (m_messageOutputMode == MessageOutputMode::NamedPipe && m_outputPipe)
	{
		switch (type)
		{
		case Warning:
			sprintf_s(messageLine, 1024, "<Message>WARNING: %s</Message>", message); break;
		case Progress:
			sprintf_s(messageLine, 1024, "<Progress>%s</Progress>", message); break;
		case Evaluation:
			sprintf_s(messageLine, 1024, "<Evaluation>%s</Evaluation>", message); break;
		case Info:
			sprintf_s(messageLine, 1024, "<Message>%s</Message>", message); break;
		case Error:
			sprintf_s(messageLine, 1024, "<Error>ERROR: %s</Error>", message); break;
		}
		writeLogBuffer(Output::Pipe, messageLine);
	}
	else
	{
		switch (type)
		{
		case Warning:
			printf("WARNING: %s\n", message); break;
		case Progress:
			//extra spaces to avoid overwriting only partially previous message
			printf("PROGRESS: %s                     \r", message); break;
		case Evaluation:
			//extra spaces to avoid overwriting only partially previous message
			printf("EVALUATION: %s\n", message); break;
		case Info:
			printf("%s\n", message); break;
		case Error:
			printf("ERROR: %s\n",message); break;
		}
	}
}

void CLogger::closeOutputPipe()
{
	if (m_outputPipe)
	{
		FlushFileBuffers(m_outputPipe);
		CloseHandle(m_outputPipe);
		m_outputPipe = 0;
	}
}

void CLogger::createOutputPipe(const char* pipeName)
{
	size_t convertedChars;
	
	wchar_t w_pipename[512];
	wchar_t w_completePipename[512] = L"\\\\.\\pipe\\";

	
	mbstowcs_s(&convertedChars, w_pipename,512, pipeName, 512);
	wcscat_s(w_completePipename, w_pipename);
	
	m_outputPipe = CreateFile(
		w_completePipename,   // pipe name 
		GENERIC_WRITE,
		0,              // no sharing 
		NULL,           // default security attributes
		OPEN_EXISTING,  // opens existing pipe 
		0,              // default attributes 
		NULL);          // no template file 

	if (m_outputPipe == INVALID_HANDLE_VALUE)
	{
		//printf("FAILED\n");
		logMessage(MessageType::Error, "Could not create pipe");
		return;
	}
	//printf("OK\n");

	m_messageOutputMode = MessageOutputMode::NamedPipe;
	logMessage(MessageType::Info, "Pipe succesfully created");
}