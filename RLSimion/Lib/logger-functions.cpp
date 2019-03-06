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

#include "logger.h"
#include "app.h"
#include "experiment.h"
#include "function-sampler.h"
#include "../../tools/System/CrossPlatform.h"
#include <unordered_map>
using namespace std;

#define FUNCTION_SAMPLE_HEADER 6543
#define FUNCTION_DECLARATION_HEADER 5432
#define FUNCTION_LOG_FILE_HEADER 4321
#define FUNCTION_LOG_FILE_VERSION 1

#define MAX_FUNCTION_ID_LENGTH 128

//using long long int to assure C# data-type compatibility

struct FunctionLogHeader
{
	long long int magicNumber = FUNCTION_LOG_FILE_HEADER;
	long long int fileVersion = FUNCTION_LOG_FILE_VERSION;
	long long int numFunctions = 0;
};

struct FunctionDeclarationHeader
{
	long long int magicNumber = FUNCTION_DECLARATION_HEADER;
	long long int id;
	char name[MAX_FUNCTION_ID_LENGTH];
	long long int numSamplesX;
	long long int numSamplesY;
	long long int numSamplesZ;
};

struct FunctionSampleHeader
{
	long long int magicNumber = FUNCTION_SAMPLE_HEADER;
	long long int episode;
	long long int step;
	long long int experimentStep;
	long long int id;
};

/// <summary>
/// Creates a file where functions will be logged
/// </summary>
/// <param name="filename">Path to the output file</param>
void Logger::openFunctionLogFile(const char* filename)
{
	CrossPlatform::Fopen_s(&m_functionLogFile, m_outputFunctionLogBinary.c_str(), "wb");
	if (m_functionLogFile)
	{
		//write function log header
		FunctionLogHeader functionLogHeader;
		functionLogHeader.numFunctions = SimionApp::get()->getFunctionSamplers().size();
		fwrite(&functionLogHeader, sizeof(FunctionLogHeader), 1, m_functionLogFile);

		//write function declarations
		unsigned int functionId = 0;
		FunctionDeclarationHeader functionDeclarationHeader;
		for (FunctionSampler* sampler : SimionApp::get()->getFunctionSamplers())
		{
			functionDeclarationHeader.id = functionId;
			CrossPlatform::Sprintf_s(functionDeclarationHeader.name, MAX_FUNCTION_ID_LENGTH, "%s", sampler->getFunctionId().c_str());
			functionDeclarationHeader.numSamplesX = (unsigned int)sampler->getNumSamplesX();
			functionDeclarationHeader.numSamplesY = (unsigned int)sampler->getNumSamplesY();
			functionDeclarationHeader.numSamplesZ = 1; //for now, not using it

			fwrite(&functionDeclarationHeader, sizeof(FunctionDeclarationHeader), 1, m_functionLogFile);
			functionId++;
		}
	}
	else Logger::logMessage(MessageType::Warning, "Function log file couldn't be opened, so no function info will be saved.");
}

/// <summary>
/// Closes the file used for logging functions
/// </summary>
void Logger::closeFunctionLogFile()
{
	if (m_functionLogFile)
		fclose(m_functionLogFile);
}

/// <summary>
/// Adds a sample from each function to the log file
/// </summary>
void Logger::writeFunctionLogSample()
{
	if (!m_functionLogFile)
		return;

	unsigned int functionId = 0;
	for (FunctionSampler* sampler : SimionApp::get()->getFunctionSamplers())
	{
		//Write the header
		FunctionSampleHeader header;
		Experiment* pExperiment = SimionApp::get()->pExperiment.ptr();
		header.episode = pExperiment->getEvaluationIndex();
		header.step = pExperiment->getStep();
		header.experimentStep = pExperiment->getExperimentStep();
		header.id = functionId;

		fwrite(&header, sizeof(FunctionSampleHeader), 1, m_functionLogFile);

		//Write the values sampled
		const vector<double>& valuesSampled = sampler->sample();

		fwrite(&valuesSampled[0], sizeof(double), valuesSampled.size(), m_functionLogFile);

		functionId++;
	}
}
