#include "logger.h"
#include "app.h"
#include "experiment.h"
#include "function-sampler.h"
#include <unordered_map>
using namespace std;

#define FUNCTION_SAMPLE_HEADER 3
#define FUNCTION_SAMPLER_FILE_VERSION 1
#define MAX_FUNCTION_ID_LENGTH 128

struct FunctionSampleHeader
{
	//we replicate this info in each header to simplify the programming
	unsigned int fileVersion = FUNCTION_SAMPLER_FILE_VERSION;
	unsigned int magicNumber = FUNCTION_SAMPLE_HEADER;
	unsigned int m_episode;
	unsigned int m_step;
	unsigned int m_experimentStep;
	unsigned int m_id;
	char m_name[MAX_FUNCTION_ID_LENGTH];
	unsigned int m_numSamplesX;
	unsigned int m_numSamplesY;
};

void Logger::openFunctionLogFile(const char* filename)
{
	fopen_s(&m_functionLogFile, m_outputFunctionLogBinary.c_str(), "w");
}

void Logger::closeFunctionLogFile()
{
	fclose(m_functionLogFile);
}


void Logger::logFunctions()
{
	unsigned int functionId = 0;
	for each (FunctionSampler* sampler in SimionApp::get()->getFunctionSamplers())
	{
		//Write the header
		FunctionSampleHeader header;
		Experiment* pExperiment = SimionApp::get()->pExperiment.ptr();
		header.m_episode = pExperiment->getEpisodeInEvaluationIndex();
		header.m_step = pExperiment->getStep();
		header.m_experimentStep = pExperiment->getExperimentStep();
		header.m_id = functionId;
		sprintf_s(header.m_name, MAX_FUNCTION_ID_LENGTH, "%s", sampler->getFunctionId().c_str());
		header.m_numSamplesX = (unsigned int)sampler->getNumSamplesX();
		header.m_numSamplesY = (unsigned int)sampler->getNumSamplesY();

		fwrite(&header, sizeof(FunctionSampleHeader), 1, m_functionLogFile);

		//Write the values sampled
		const vector<double> valuesSampled = sampler->sample();

		fwrite(valuesSampled.data(), valuesSampled.size(), sizeof(double), m_functionLogFile);

		functionId++;
	}
}
