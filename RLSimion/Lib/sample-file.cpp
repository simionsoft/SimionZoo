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
#include "../../tools/System/CrossPlatform.h"
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

		m_numSamplesInFile = atoi(pRoot->Attribute("NumSamples"));

		XMLElement* pChild = pRoot->FirstChildElement();
		while (pChild != nullptr)
		{
			if (!strcmp(pChild->Name(), "State-variable")) m_numStateVariables++;
			else if (!strcmp(pChild->Name(), "Action-variable")) m_numActionVariables++;
			else if (!strcmp(pChild->Name(), "Reward-variable")) m_numRewardVariables++;
			pChild = pChild->NextSiblingElement();
		}
		m_numElementsPerSample = 2 * m_numStateVariables + m_numActionVariables + m_numRewardVariables;
		m_sampleSizeInBytes = sizeof(double) * m_numElementsPerSample; //<s, a, s_p, r>

		//We are using an arbitrary data chunk size (10^5). With 100 variables per tuple, we would need 100*8*10^5= 80Mb of memory, which seems reasonable
		m_dataChunkSizeInSamples= 10000;
		m_dataChunkSizeInElements= m_dataChunkSizeInSamples * m_numElementsPerSample;
		m_numChunksInFile = (m_numSamplesInFile / m_dataChunkSizeInSamples);
		if (m_numSamplesInFile % m_dataChunkSizeInSamples != 0) m_numChunksInFile++;
		m_cachedData = vector<double>(m_dataChunkSizeInSamples * m_sampleSizeInBytes);

		//Open the sample file and read the first data chunk
		m_currentChunk = 0;
		CrossPlatform::Fopen_s((FILE **) &m_pBinaryFile, m_binaryFilename.c_str(), "rb");
		if (m_pBinaryFile && m_numChunksInFile > 0)
		{
			m_numValidSamplesInCache = (int)fread(m_cachedData.data(), m_sampleSizeInBytes, m_dataChunkSizeInSamples, (FILE*)m_pBinaryFile);
			Logger::logMessage(MessageType::Error, (string("Sample file loaded: ") + filename).c_str());
		}
		else Logger::logMessage(MessageType::Error, (string("Error loading sample file: ") + filename).c_str());
	}
	else Logger::logMessage(MessageType::Error, (string("Could not load sample file: ") + filename).c_str());
}

SampleFile::~SampleFile()
{
	if (m_pBinaryFile)
		fclose((FILE *) m_pBinaryFile);
}

void SampleFile::loadNextDataChunkFromFile()
{
	//Read a data chunk from the sample file
	if (m_pBinaryFile != nullptr && m_numChunksInFile>0)
	{
		m_currentChunk= m_currentSampleIndex / m_dataChunkSizeInSamples;

		fseek((FILE*) m_pBinaryFile, m_currentChunk * (m_dataChunkSizeInSamples * m_sampleSizeInBytes), SEEK_SET);
		m_numValidSamplesInCache = (int) fread(m_cachedData.data(), m_sampleSizeInBytes, m_dataChunkSizeInSamples, (FILE*) m_pBinaryFile);
	}
	else Logger::logMessage(MessageType::Error, "Failed to open offline sample file");
}

void SampleFile::drawRandomSample(State* s, Action* a, State* s_p, double& reward)
{
	if (m_currentSampleIndex >= m_currentChunk * m_dataChunkSizeInSamples + m_numValidSamplesInCache
		|| m_currentSampleIndex < m_currentChunk * m_dataChunkSizeInSamples)
		loadNextDataChunkFromFile();

	int sampleOffset = m_numElementsPerSample * (m_currentSampleIndex - m_currentChunk * m_dataChunkSizeInSamples);
	for (int i = 0; i < s->getNumVars(); i++) s->set(i, m_cachedData[sampleOffset + i]);
	sampleOffset += (int) s->getNumVars();
	for (int i = 0; i < a->getNumVars(); i++) a->set(i, m_cachedData[sampleOffset + i]);
	sampleOffset += (int) a->getNumVars();
	for (int i = 0; i < s_p->getNumVars(); i++) s_p->set(i, m_cachedData[sampleOffset + i]);
	sampleOffset += (int) s_p->getNumVars();
	reward = m_cachedData[sampleOffset]; //We only take the first reward value. Should be enough for now

	m_currentSampleIndex = ++m_currentSampleIndex % m_numSamplesInFile;
}