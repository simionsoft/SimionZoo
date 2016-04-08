#include "stdafx.h"
#include "logger.h"
#include "world.h"
#include "named-var-set.h"
#include "parameters.h"
#include "stats.h"
#include "globals.h"
#include "timer.h"
#include "app.h"


#define MAX_FILENAME_LENGTH 1024

CLASS_INIT(CLogger)
{
	if (!pParameters) return;

	m_outputDir = 0;// new char[MAX_FILENAME_LENGTH];

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

void CLogger::setLogDirectory(const char* xmlFilePath)
{
	if (!xmlFilePath)
		throw(std::exception("CLogger. No output directory provided."));

	m_outputDir = new char[strlen(xmlFilePath)+1];
	strcpy_s(m_outputDir, strlen(xmlFilePath) + 1, xmlFilePath);

	int i = strlen(m_outputDir)-1;
	while (i > 0 && m_outputDir[i] != '/' && m_outputDir[i] != '\\')
		i--;

	if (i > 0)
		m_outputDir[i] = 0;
	//_splitpath_s(xmlFilePath,NULL, 0,m_outputDir, sizeof(m_outputDir),NULL, 0,NULL, 0);

}

CLogger::CLogger()
{
	//default values for safety


	m_bLogEvaluationEpisodes = false;
	m_bLogEvaluationExperiment = false;
	m_bLogTrainingEpisodes = true;
	m_bLogTrainingExperiment = false;
	m_logFreq = 0.0;
}


CLogger::~CLogger()
{
	delete m_pExperimentTimer;
	delete m_pEpisodeTimer;

	if (m_outputDir) delete[] m_outputDir;
}

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
}

FILE* CLogger::openLogFile(bool create, bool episodeLog, bool evalEpisode, unsigned int episodeIndex)
{
	FILE* pFile;
	char filename[1024];

	getLogFilename(filename, 1024, episodeLog, evalEpisode, episodeIndex);

	if (create)
	{
		fopen_s(&pFile, filename, "w");
		if (pFile)
		{
			writeLogFileHeader(pFile, episodeLog, evalEpisode);
			fclose(pFile);
		}
		return 0;
	}
	else
	{
		fopen_s(&pFile, filename, "a");
		return pFile;
	}
}

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
}

void CLogger::writeExperimentLogData(bool evalEpisode, unsigned int episodeIndex)
{
	if (logCurrentEpisodeInExperiment(evalEpisode))
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

void CLogger::writeEpisodeLogData(bool evalEpisode, unsigned int episodeIndex)
{
	if (logCurrentEpisode(evalEpisode))
	{
		FILE* pFile = openLogFile(false, true, evalEpisode, episodeIndex);

		if (pFile)
		{
			fprintf(pFile, "%.3f ", CApp::World.getT());

			for (auto it = m_stats.begin(); it != m_stats.end(); it++)
			{
				fprintf(pFile, "%.3f ", (*it)->getValue());
			}
			fprintf(pFile, "\n");
			fclose(pFile);
		}
	}
}

bool CLogger::logCurrentEpisode(bool evalEpisode)
{
	return (evalEpisode && m_bLogEvaluationEpisodes) || (!evalEpisode && m_bLogTrainingEpisodes);
}

bool CLogger::logCurrentEpisodeInExperiment(bool evalEpisode)
{
	return (evalEpisode && m_bLogEvaluationExperiment) || (!evalEpisode && m_bLogTrainingExperiment);
}

void CLogger::firstEpisode(bool evalEpisode)
{
	//set episode start time
	m_pEpisodeTimer->startTimer();
	//QueryPerformanceCounter((LARGE_INTEGER*)&m_experimentStartCounter);

	if ((m_bLogEvaluationExperiment && evalEpisode) || (m_bLogTrainingExperiment && !evalEpisode))
		openLogFile(true, false, evalEpisode, 1);
}

void CLogger::firstStep(bool evalEpisode, unsigned int episodeIndex)
{
	//set episode start time
	//QueryPerformanceCounter((LARGE_INTEGER*)&m_episodeStartCounter);
	m_pEpisodeTimer->startTimer();

	m_lastLogSimulationT = 0.0;
	//reset stats
	for (auto it = m_stats.begin(); it != m_stats.end(); it++)
		(*it)->reset();


	//create the episode log file
	if (logCurrentEpisode(evalEpisode))
		openLogFile(true, true, evalEpisode, episodeIndex);
}

void CLogger::timestep(bool evalEpisode, unsigned int episodeIndex)
{
	bool bLog = logCurrentEpisode(evalEpisode);

	//update experiment stats
	for (auto iterator = m_stats.begin(); iterator != m_stats.end(); iterator++)
	{
		(*iterator)->addExperimentSample();
	}

	//output episode log data
	if (bLog && (CApp::World.getStepStartT() - m_lastLogSimulationT >= m_logFreq))
	{
		writeEpisodeLogData(evalEpisode, episodeIndex);
		m_lastLogSimulationT = CApp::World.getStepStartT();
	}

}

void CLogger::lastStep(bool evalEpisode, unsigned int episodeIndex)
{
	writeExperimentLogData(evalEpisode, episodeIndex);
}

void CLogger::lastEpisode(bool evalEpisode)
{
	//so far, we are doing nothing
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

MessageOutputMode CLogger::m_messageOutputMode = MessageOutputMode::Console;
void* CLogger::m_outputPipe = 0;

void CLogger::logMessage(MessageType type, const char* message)
{

	char messageLine[1024];
	unsigned long bytesWritten;


	if (m_messageOutputMode == MessageOutputMode::NamedPipe && m_outputPipe)
	{
		//mbstowcs_s(&numChars, messageMByte, 1024, message, 1023);
		switch (type)
		{
		case Warning:
			sprintf_s(messageLine, 1024, "<Message>WARNING: %s</Message>\n", message);
			WriteFile(m_outputPipe, messageLine, strlen(messageLine), &bytesWritten, 0);
			break;
		case Progress:
			//extra spaces to avoid overwriting only partially previous message
			sprintf_s(messageLine, 1024, "<Progress>%s</Progress>\n", message);
			WriteFile(m_outputPipe, messageLine, strlen(messageLine), &bytesWritten, 0);
			break;
		case Info:
			sprintf_s(messageLine, 1024, "<Message>INFO: %s</Message>\n", message);
			WriteFile(m_outputPipe, messageLine, strlen(messageLine), &bytesWritten, 0);
			break;
		case Error:
			sprintf_s(messageLine, 1024, "<Message>FATAL ERROR: %ls</Message>\n", message);
			WriteFile(m_outputPipe, messageLine, strlen(messageLine), &bytesWritten, 0);
			closeOutputPipe();
			exit(-1);
		}
	}
	else
	{
		switch (type)
		{
		case Warning:
			printf("WARNING: %s\n", message);
			break;
		case Progress:
			//extra spaces to avoid overwriting only partially previous message
			printf("PROGRESS: %s                     \r", message);
			break;
		case Info:
			printf("INFO: %s", message);
			break;
		case Error:
			printf("FATAL ERROR: %s\n",message);
			break;
		}
	}
}

void CLogger::closeOutputPipe()
{
	if (m_outputPipe)
	{
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



	//wprintf(L"Creating pipe: %ls\n", w_completePipename);

	//m_outputPipe = CreateNamedPipe(
	//	w_pipename,             // pipe name 
	//	PIPE_ACCESS_OUTBOUND,       // write access 
	//	PIPE_TYPE_MESSAGE |       // message type pipe 
	//	PIPE_WAIT,                // blocking mode 
	//	PIPE_UNLIMITED_INSTANCES, // max. instances  
	//	1024,              // output buffer size 
	//	0,              // input buffer size 
	//	NMPWAIT_USE_DEFAULT_WAIT, // client time-out 
	//	NULL);                    // default security attribute  

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

		return;
	}
	//printf("OK\n");

	m_messageOutputMode = MessageOutputMode::NamedPipe;
}