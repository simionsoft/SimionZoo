#include "stdafx.h"
#include "LogLoader.h"
#include "../WindowsUtils/FileUtils.h"

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
	}
	else return false;
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