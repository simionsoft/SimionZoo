#include "logger.h"
#include "app.h"
#include "experiment.h"
#include "function-sampler.h"
#include <unordered_map>
using namespace std;

#define FUNCTION_SAMPLE_HEADER 6543
#define FUNCTION_DECLARATION_HEADER 5432
#define FUNCTION_LOG_FILE_HEADER 4321
#define FUNCTION_LOG_FILE_VERSION 1

#define MAX_FUNCTION_ID_LENGTH 128

//using __int64 to assure C# data-type compatibility

struct FunctionLogHeader
{
	__int64 magicNumber = FUNCTION_LOG_FILE_HEADER;
	__int64 fileVersion = FUNCTION_LOG_FILE_VERSION;
	__int64 numFunctions = 0;
};

struct FunctionDeclarationHeader
{
	__int64 magicNumber = FUNCTION_DECLARATION_HEADER;
	__int64 id;
	char name[MAX_FUNCTION_ID_LENGTH];
	__int64 numSamplesX;
	__int64 numSamplesY;
	__int64 numSamplesZ;
};

struct FunctionSampleHeader
{
	__int64 magicNumber = FUNCTION_SAMPLE_HEADER;
	__int64 episode;
	__int64 step;
	__int64 experimentStep;
	__int64 id;
};

void Logger::openFunctionLogFile(const char* filename)
{
	fopen_s(&m_functionLogFile, m_outputFunctionLogBinary.c_str(), "wb");
	if (m_functionLogFile)
	{
		//write function log header
		FunctionLogHeader functionLogHeader;
		functionLogHeader.numFunctions = SimionApp::get()->getFunctionSamplers().size();
		fwrite(&functionLogHeader, sizeof(FunctionLogHeader), 1, m_functionLogFile);

		//write function declarations
		unsigned int functionId = 0;
		FunctionDeclarationHeader functionDeclarationHeader;
		for each (FunctionSampler* sampler in SimionApp::get()->getFunctionSamplers())
		{
			functionDeclarationHeader.id = functionId;
			sprintf_s(functionDeclarationHeader.name, MAX_FUNCTION_ID_LENGTH, "%s", sampler->getFunctionId().c_str());
			functionDeclarationHeader.numSamplesX = (unsigned int)sampler->getNumSamplesX();
			functionDeclarationHeader.numSamplesY = (unsigned int)sampler->getNumSamplesY();
			functionDeclarationHeader.numSamplesZ = 1; //for now, not using it

			fwrite(&functionDeclarationHeader, sizeof(FunctionDeclarationHeader), 1, m_functionLogFile);
			functionId++;
		}
	}
}

void Logger::closeFunctionLogFile()
{
	if (m_functionLogFile)
		fclose(m_functionLogFile);
}


void Logger::writeFunctionLogSample()
{
	unsigned int functionId = 0;
	for each (FunctionSampler* sampler in SimionApp::get()->getFunctionSamplers())
	{
		//Write the header
		FunctionSampleHeader header;
		Experiment* pExperiment = SimionApp::get()->pExperiment.ptr();
		header.episode = pExperiment->getEvaluationIndex();
		header.step = pExperiment->getStep();
		header.experimentStep = pExperiment->getExperimentStep();
		header.id = functionId;

		size_t numElemWritten = fwrite(&header, sizeof(FunctionSampleHeader), 1, m_functionLogFile);

		//Write the values sampled
		const vector<double>& valuesSampled = sampler->sample();

		fwrite(valuesSampled.data(), sizeof(double), valuesSampled.size(), m_functionLogFile);

		functionId++;
	}
}
