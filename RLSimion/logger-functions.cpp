#include "logger.h"
#include "app.h"
#include "experiment.h"
#include "function-sampler.h"
#include <unordered_map>
using namespace std;

#define FUNCTION_SAMPLE_HEADER 3
#define MAX_FUNCTION_ID_LENGTH 128

struct FunctionSampleHeader
{
	__int64 magicNumber = FUNCTION_SAMPLE_HEADER;
	unsigned int m_episode;
	unsigned int m_step;
	char m_functionId[MAX_FUNCTION_ID_LENGTH];
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


void Logger::logFunctionSample(FunctionSampler* pFunctionSampler)
{
	FunctionSampleHeader header;
	Experiment* pExperiment = SimionApp::get()->pExperiment.ptr();
	header.m_episode = pExperiment->getEpisodeInEvaluationIndex();
	header.m_step = pExperiment->getStep();
	sprintf_s(header.m_functionId, MAX_FUNCTION_ID_LENGTH,"%s", pFunctionSampler->getFunctionId().c_str());
	header.m_numSamplesX = pFunctionSampler->getNumSamplesX();
	header.m_numSamplesY = pFunctionSampler->getNumSamplesY();

	fwrite(&header, sizeof(FunctionSampleHeader), 1, m_functionLogFile);
}