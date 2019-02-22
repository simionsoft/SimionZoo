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

#include "stdafx.h"
#include "LogLoader.h"
#include "../System/FileUtils.h"
#include <algorithm>

Step::Step(int numVariables)
{
	m_numValues = numVariables;
	m_pValues = new double[numVariables];
}

Step::~Step()
{
	if (m_pValues) delete [] m_pValues;
}

void Step::load(FILE* pFile)
{
	size_t elementsRead = fread_s((void*)&m_header, sizeof(StepHeader), sizeof(StepHeader), 1, pFile);
	if (elementsRead == 1 && !bEnd())
	{
		elementsRead = fread_s(m_pValues, sizeof(double)*m_numValues, sizeof(double), m_numValues, pFile);
	}
}

double Step::getValue(int i) const
{
	if (i>=0 && i<m_numValues)
		return m_pValues[i];
	return 0.0;
}

void Step::setValue(int i, double value)
{
	if (i >= 0 && i<m_numValues)
		m_pValues[i]= value;
}

double Step::getExperimentRealTime() const
{
	return m_header.experimentRealTime;
}
double Step::getEpisodeSimTime() const
{
	return m_header.m_episodeSimTime;
}
double Step::getEpisodeRealTime() const
{
	return m_header.episodeRealTime;
}

bool Step::bEnd()
{
	return m_header.magicNumber == EPISODE_END_HEADER;
}

void Episode::load(FILE* pFile)
{
	size_t elementsRead = fread_s((void*)&m_header, sizeof(EpisodeHeader), sizeof(EpisodeHeader), 1, pFile);
	if (elementsRead == 1)
	{
		Step *pStep= new Step((int)m_header.numVariablesLogged);
		pStep->load(pFile);
		while (!pStep->bEnd())
		{
			m_pSteps.push_back(pStep);

			pStep = new Step((int)m_header.numVariablesLogged);
			pStep->load(pFile);
		}
	}
}

Episode::~Episode()
{
	for (auto it = m_pSteps.begin(); it != m_pSteps.end(); ++it)
		delete (*it);
}

Step* Episode::getStep(int i)
{
	if (i>=0 && i<m_pSteps.size())
		return m_pSteps[i];
	return nullptr;
}



bool ExperimentLog::load(string descriptorFile, string& outSceneFile)
{
	//Load the XML descriptor
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLElement *pRoot, *pChild;
	string binaryFile;
	string functionLogFile;
	doc.LoadFile(descriptorFile.c_str());
	if (doc.Error() == tinyxml2::XML_SUCCESS)
	{
		pRoot = doc.FirstChildElement("ExperimentLogDescriptor");
		if (pRoot)
		{
			binaryFile = pRoot->Attribute(xmlTagBinaryDataFile);
			if (pRoot->Attribute(xmlTagFunctionLogFile))
				functionLogFile = pRoot->Attribute(xmlTagFunctionLogFile);
			outSceneFile = pRoot->Attribute(xmlTagSceneFile);
			pChild = pRoot->FirstChildElement();
			while (pChild)
			{
				if (!strcmp(pChild->Name(), "State-variable")
					|| !strcmp(pChild->Name(), "Action-variable")
					|| !strcmp(pChild->Name(), "Reward-variable")
					|| !strcmp(pChild->Name(), "Stat-variable"))
				{
					double maximum = std::numeric_limits<double>::max();
					double minimum = std::numeric_limits<double>::min();
					const char* name = pChild->GetText();
					const char* units = "N/A";
					bool circular = false;
					if (pChild->Attribute("Units"))
						units = (const char*) pChild->Attribute("Units");
					if (pChild->Attribute("Max"))
						maximum = atof(pChild->Attribute("Max"));
					if (pChild->Attribute("Min"))
						minimum = atof(pChild->Attribute("Min"));
					if (pChild->Attribute("Circular") && !strcmp(pChild->Attribute("Circular"), "true"))
						circular = true;
					m_descriptor.addVariable(name, units, minimum, maximum, circular);
				}

				pChild = pChild->NextSiblingElement();
			}
		}
		else return false;
	}
	else return false;

	//Load the binary file from the same directory
	FILE* pFile;

	string logDirectory = getDirectory(descriptorFile);
	fopen_s(&pFile, (logDirectory + binaryFile).c_str(), "rb");
	if (pFile)
	{
		size_t elementsRead = fread_s((void*)&m_header, sizeof(ExperimentHeader), sizeof(ExperimentHeader), 1, pFile);
		if (elementsRead == 1)
		{
			m_pEpisodes = new Episode[getNumEpisodes()];
			for (int i = 0; i < getNumEpisodes(); ++i)
			{
				m_pEpisodes[i].load(pFile);
			}
		}
		fclose(pFile);
	}
	else return false;

	//Load the function log file from the same directory
	if (!functionLogFile.empty())
		m_functionLog.load((logDirectory + functionLogFile).c_str());

	return true;
}

ExperimentLog::~ExperimentLog()
{
	if (m_pEpisodes) delete [] m_pEpisodes;
}

int ExperimentLog::getVariableIndex(string variableName) const
{
	for (int i = 0; i < m_descriptor.size(); ++i)
		if (string(m_descriptor[i].getName()) == variableName)
			return i;
	return -1;
}

#define FUNCTION_SAMPLE_HEADER 6543
#define FUNCTION_DECLARATION_HEADER 5432
#define FUNCTION_LOG_FILE_HEADER 4321
#define FUNCTION_LOG_FILE_VERSION 1

FunctionSample::FunctionSample(size_t episode, size_t step, size_t experimentStep, size_t numSamples)
{
	m_episode = episode;
	m_step = step;
	m_experimentStep = experimentStep;
	m_values = vector<double>(numSamples);
}

Function::Function(string name, size_t numSamplesX, size_t numSamplesY, size_t numSamplesZ)
{
	m_name = name;
	m_numSamplesX = numSamplesX;
	m_numSamplesY = numSamplesY;
	m_numSamplesZ = numSamplesZ;

	m_interpolatedValues = vector<double>(numSamples());
}

const vector<double>& Function::getInterpolatedData(size_t episode, size_t step, bool &dataChanged)
{
	int previous = 0, next;
	episode++; //saved episodes start from 1
	if (episode != m_lastInterpolatedEpisode)
	{
		if (m_samples.size() > 1)
		{
			while (previous < m_samples.size() - 2 && m_samples[previous + 1]->episode() < episode)
				++previous;

			next = previous + 1;
		}
		else
			previous = next = 0;

		double u = ((double)(episode - m_samples[previous]->episode())) / (double)(m_samples[next]->episode() - m_samples[previous]->episode());
		double inv_u = 1. - u;

		for (size_t i = 0; i < numSamples(); i++)
			m_interpolatedValues[i] = m_samples[previous]->value(i)*inv_u + m_samples[next]->value(i)*u;

		m_lastInterpolatedEpisode = (int)episode;
		dataChanged = true;
	}
	else dataChanged = false;
	return m_interpolatedValues;
}

void FunctionLog::load(const char* functionLogFile)
{
	FunctionLogHeader logHeader;
	FunctionDeclarationHeader funDeclHeader;
	FunctionSampleHeader sampleHeader;
	FILE* pFile;
	
	fopen_s(&pFile, functionLogFile, "rb");
	if (pFile)
	{
		//read function log header
		fread_s(&logHeader, sizeof(FunctionLogHeader), sizeof(FunctionLogHeader), 1, pFile);
		if (logHeader.magicNumber != FUNCTION_LOG_FILE_HEADER)
			throw exception("Incorrect magic number trying to read function log file");
		if (logHeader.fileVersion != FUNCTION_LOG_FILE_VERSION)
			throw exception("Missmatched function log file version read");

		//read function declarations
		for (size_t f = 0; f < (size_t) logHeader.numFunctions; ++f)
		{
			fread_s(&funDeclHeader, sizeof(FunctionDeclarationHeader), sizeof(FunctionDeclarationHeader), 1, pFile);
			if (funDeclHeader.magicNumber != FUNCTION_DECLARATION_HEADER)
				throw exception("Incorrect magic number trying to read function log file");
			Function* pFunction = new Function(funDeclHeader.name, (size_t) funDeclHeader.numSamplesX
				, (size_t)funDeclHeader.numSamplesY, (size_t) funDeclHeader.numSamplesZ);
			m_functions.push_back(pFunction);
		}

		//read saved samples
		while (!feof(pFile))
		{
			if (fread_s(&sampleHeader, sizeof(FunctionSampleHeader), sizeof(FunctionSampleHeader), 1, pFile)==1)
			{
				if (sampleHeader.magicNumber != FUNCTION_SAMPLE_HEADER)
					throw exception("Incorrect magic number trying to read function log file");
				if ((size_t)sampleHeader.id >= m_functions.size() || sampleHeader.id < 0)
					throw exception("Wrong function Id trying to read function log file");
				size_t numSamples = m_functions[sampleHeader.id]->numSamples();

				FunctionSample* pSample = new FunctionSample(sampleHeader.episode, sampleHeader.step, sampleHeader.experimentStep, numSamples);

				if (fread_s(&pSample->values()[0], numSamples * sizeof(double), sizeof(double), numSamples, pFile) == numSamples)
					m_functions[sampleHeader.id]->addSample(pSample);
				else
					break; //file wasn't fully saved. Silent error, this is expected to happen sometimes
			}
			else
				break;
		}

		fclose(pFile);
	}
}

FunctionLog::~FunctionLog()
{
	for each (Function* pFunction in m_functions)
	{
		delete pFunction;
	}
}

Function::~Function()
{
	for each (FunctionSample* pSample in m_samples)
	{
		delete pSample;
	}
}

FunctionSample::~FunctionSample()
{
}