#include "stdafx.h"
#include "LogLoader.h"
#include "../WindowsUtils/FileUtils.h"

//vector<string> CExperimentLog::m_variableNames;

CStep::CStep(int numVariables)
{
	m_numValues = numVariables;
	m_pValues = new double[numVariables];
}

CStep::~CStep()
{
	if (m_pValues) delete [] m_pValues;
}

void CStep::load(FILE* pFile)
{
	int elementsRead = fread_s((void*)&m_header, sizeof(StepHeader), sizeof(StepHeader), 1, pFile);
	if (elementsRead == 1 && !bEnd())
	{
		elementsRead = fread_s(m_pValues, sizeof(double)*m_numValues, sizeof(double), m_numValues, pFile);
	}
}

double CStep::getValue(int i) const
{
	if (i>=0 && i<m_numValues)
		return m_pValues[i];
	return 0.0;
}

void CStep::setValue(int i, double value)
{
	if (i >= 0 && i<m_numValues)
		m_pValues[i]= value;
}

double CStep::getExperimentRealTime() const
{
	return m_header.experimentRealTime;
}
double CStep::getEpisodeSimTime() const
{
	return m_header.m_episodeSimTime;
}
double CStep::getEpisodeRealTime() const
{
	return m_header.episodeRealTime;
}

bool CStep::bEnd()
{
	return m_header.magicNumber == EPISODE_END_HEADER;
}

void CEpisode::load(FILE* pFile)
{
	int elementsRead = fread_s((void*)&m_header, sizeof(EpisodeHeader), sizeof(EpisodeHeader), 1, pFile);
	if (elementsRead == 1)
	{
		CStep *pStep= new CStep((int)m_header.numVariablesLogged);
		pStep->load(pFile);
		while (!pStep->bEnd())
		{
			m_pSteps.push_back(pStep);

			pStep = new CStep((int)m_header.numVariablesLogged);
			pStep->load(pFile);
		}
	}
}

CEpisode::~CEpisode()
{
	for (auto it = m_pSteps.begin(); it != m_pSteps.end(); ++it)
		delete (*it);
}

CStep* CEpisode::getStep(int i)
{
	if (i>=0 && i<m_pSteps.size())
		return m_pSteps[i];
	return nullptr;
}



bool CExperimentLog::load(string descriptorFile, string& outSceneFile)
{
	//Load the XML descriptor
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLElement *pRoot, *pChild;
	string binaryFile;
	doc.LoadFile(descriptorFile.c_str());
	if (doc.Error() == tinyxml2::XML_SUCCESS)
	{
		pRoot = doc.FirstChildElement("ExperimentLogDescriptor");
		if (pRoot)
		{
			binaryFile = pRoot->Attribute(xmlTagBinaryDataFile);
			outSceneFile = pRoot->Attribute(xmlTagSceneFile);
			pChild = pRoot->FirstChildElement();
			while (pChild)
			{
				if (!strcmp(pChild->Name(), "State-variable")
					|| !strcmp(pChild->Name(), "Action-variable")
					|| !strcmp(pChild->Name(), "Reward-variable")
					|| !strcmp(pChild->Name(), "Stat-variable"))
					m_variableNames.push_back(string(pChild->GetText()));

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
		int elementsRead = fread_s((void*)&m_header, sizeof(ExperimentHeader), sizeof(ExperimentHeader), 1, pFile);
		if (elementsRead == 1)
		{
			m_pEpisodes = new CEpisode[getNumEpisodes()];
			for (int i = 0; i < getNumEpisodes(); ++i)
			{
				m_pEpisodes[i].load(pFile);
			}
		}
	}
	else return false;
	return true;
}

CExperimentLog::~CExperimentLog()
{
	if (m_pEpisodes) delete [] m_pEpisodes;
}

int CExperimentLog::getVariableIndex(string variableName) const
{
	for (int i = 0; i < m_variableNames.size(); ++i)
		if (m_variableNames[i] == variableName)
			return i;
	return -1;
}