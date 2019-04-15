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

#include "sample-file.h"
#include "../Common/named-var-set.h"
#include "../../3rd-party/tinyxml2/tinyxml2.h"
#include "../../tools/System/FileUtils.h"
#include "logger.h"
using namespace tinyxml2;

SampleFile::SampleFile(string filename)
{
	XMLDocument doc;
	string folder;

	if (doc.LoadFile(filename.c_str()) == XML_NO_ERROR)
	{
		XMLElement* pRoot = doc.FirstChildElement();

		if (strcmp(pRoot->Name(), "SampleFileDescriptor")) return;

		if (pRoot->Attribute("BinaryDataFile") == nullptr || pRoot->Attribute("NumSamples") == nullptr) return;

		folder = getDirectory(filename);
		m_binaryFilename = folder + string(pRoot->Attribute("BinaryDataFile"));
		m_numSamples = atoi(pRoot->Attribute("NumSamples"));

		XMLElement* pChild = pRoot->FirstChildElement();
		while (pChild != nullptr)
		{
			if (!strcmp(pChild->Name(), "State-variable")) m_numStateVariables++;
			else if (!strcmp(pChild->Name(), "Action-variable")) m_numActionVariables++;
			else if (!strcmp(pChild->Name(), "Reward-variable")) m_numRewardVariables++;
			pChild = pChild->NextSiblingElement();
		}

		m_sampleSize = sizeof(double) * (2*m_numStateVariables + m_numActionVariables + m_numRewardVariables); //<s, a, s_p, r>
	}
}

SampleFile::~SampleFile()
{
	if (m_pBinaryFile != nullptr)
		fclose(m_pBinaryFile);
}

void SampleFile::drawRandomSample(State* s, Action* a, State* s_p, double& reward)
{
	if (s->getNumVars() != m_numStateVariables || a->getNumVars() != m_numActionVariables || s_p->getNumVars() != m_numStateVariables)
		Logger::logMessage(MessageType::Error, "Missmatched sample size in offline training file");

	if (m_pBinaryFile == nullptr)
		fopen_s(&m_pBinaryFile, m_binaryFilename.c_str(), "rb");

	int sampleIndex = rand() % m_numSamples;

	fseek(m_pBinaryFile, sampleIndex * sizeof(double) * m_sampleSize, SEEK_SET);
	size_t res;
	res= fread(s->getValueVector(), sizeof(double), m_numStateVariables, m_pBinaryFile);
	res= fread(a->getValueVector(), sizeof(double), m_numActionVariables, m_pBinaryFile);
	res= fread(s_p->getValueVector(), sizeof(double), m_numStateVariables, m_pBinaryFile);
	res= fread(&reward, sizeof(double), 1, m_pBinaryFile); //We only take the first reward value. Should be enough for now
	sampleIndex++;
}