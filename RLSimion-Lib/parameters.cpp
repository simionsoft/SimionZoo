#include "stdafx.h"
#include "parameters.h"
#include "parameter.h"


///////////////////////////////////////////////////////////////////////////
//CPARAMETERS
////////////////////////////////////////////////////////////////////////////

CParameters* CParameters::m_pRootNode = 0;

void CParameters::reset()
{
	if (m_pParameters)
	{
		delete[] m_pParameters;
		m_pParameters = 0;
	}
	if (m_pChildren)
	{
		for (int i = 0; i < m_numChildren; i++)
		{
			assert(m_pChildren[i]);
			m_pChildren[i]->reset();
			delete m_pChildren[i];
			m_pChildren[i] = 0;
		}
		delete[] m_pChildren;
		m_pChildren = 0;
	}

	m_numParameters = 0;
	m_numChildren = 0;
	m_name[0] = 0;
	m_pParent = 0;
}

CParameters::CParameters()
{
	allocate();
}

void CParameters::allocate()
{
	m_numParameters = 0;
	m_pParameters = new CParameter[MAX_NUM_PARAMETERS];

	m_numChildren = 0;
	m_pChildren = new CParameters*[MAX_NUM_CHILDREN];

	m_name[0] = 0;
	m_pParent = 0;
}

CParameters::CParameters(const char* parameterFile)
{
	allocate();

	sprintf_s(m_name, MAX_PARAMETER_NAME_SIZE, "Root");

	loadParameters(parameterFile);
	m_pParent = 0;
}

CParameters::~CParameters()
{
	reset();
}

char* CParameters::getName()
{
	return m_name;
}

void CParameters::setName(const char* name)
{
	strcpy_s(m_name, MAX_PARAMETER_NAME_SIZE, name);
}


bool CParameters::parseLine(char* line, CParameter& parameter)
{
	int result;
	char description[MAX_PARAMETER_NAME_SIZE];
	char stringValue[MAX_STRING_SIZE];
	double value = 0.0;

	parameter.releasePtr();

	result = sscanf_s(line, "%s : %lf", description, MAX_PARAMETER_NAME_SIZE, &value);
	if (result == 2 && strstr(line, ",") == 0)
	{
		parameter.setName(description);
		parameter.setValue(value);
	
		return true;
	}
	else
	{
		result = sscanf_s(line, "%s : %[^\n]s", description, MAX_PARAMETER_NAME_SIZE, stringValue, MAX_STRING_SIZE);
		if (result == 2)
		{
			parameter.setName(description);
			parameter.setValue(stringValue);

			return true;
		}
	}
	return false;
}


void CParameters::loadParameters(const char* parameterFile, bool bLoadAsTree)
{
	char line[MAX_LINE_LENGTH];
	char strippedParameterName[MAX_PARAMETER_NAME_SIZE];
	CParameters* pParent;

	char* ret;
	char* pComment;
	CParameter parameter;

	FILE* stream;
	printf("Loading %s...",parameterFile);
	fopen_s(&stream,parameterFile,"r");

	if (stream)
	{
		printf("ok\n");

		//allocate memory buffers if needed
		if (!m_pParameters) allocate();
		//if we load parameters on this parameter node, it will act as root from now on
		m_pRootNode = this;

		ret= fgets(line,MAX_LINE_LENGTH,stream);
		while (ret)
		{
			//first get rid of comments
			pComment= strstr(line,"//");
			if (pComment)
			{
				*pComment= 0;
			}

			//parse line
			if (parseLine(line, parameter))
			{
				if (bLoadAsTree)
				{
					//load the parameter into a tree structure
					pParent= findParent(parameter.getName(), strippedParameterName, true);
					if (pParent)
					{
						parameter.setName(strippedParameterName);
						pParent->addParameter(parameter);
					}
				}
				else
				{
					//load the parameter into a table
					addParameter(parameter);
				}

			}
			
			ret= fgets(line,MAX_LINE_LENGTH,stream);
		}
		fclose(stream);
	}
	else
	{
		printf("ERROR: Can't load file %s",parameterFile);
	}
}

void CParameters::saveParametersRecursive(void* file, char* prefix)
{
	char fullName[MAX_PARAMETER_NAME_SIZE];

	if (prefix)
		sprintf_s(fullName, MAX_PARAMETER_NAME_SIZE, "%s/%s", prefix, m_name);
	else
		strcpy_s(fullName, MAX_PARAMETER_NAME_SIZE, m_name);

	//first save own parameters
	for (int i = 0; m_numParameters; i++)
	{
		m_pParameters[i].saveParameter(file, fullName);
	}
	
	//next children
	for (int i = 0; m_numChildren; i++)
	{
		m_pChildren[i]->saveParametersRecursive(file, fullName);
	}
}

void CParameters::saveParameters(const char *parameterFile)
{
	//Self parameter's are not saved, only children's parameters
	FILE *stream;
	
	fopen_s(&stream,parameterFile,"w");

	if (stream)
	{
		for (int i = 0; i < m_numParameters; i++)
			m_pParameters[i].saveParameter((void*)stream, 0);
		for (int i = 0; i < m_numChildren; i++)
			m_pChildren[i]->saveParametersRecursive((void*)stream, 0);

		fclose(stream);
	}
	else printf("ERROR: Can't save file %s",parameterFile);
}

CParameters* CParameters::findParent(const char* fullParameterName, char* outStrippedName, bool bCreateIfNotExists)
{
	char nameCopy[MAX_PARAMETER_NAME_SIZE];
	CParameters* pNode, *pAux;
	char *nextToken, *token= 0;
	
	strcpy_s(nameCopy, MAX_PARAMETER_NAME_SIZE, fullParameterName);
	pNode = this;

	token = strtok_s(nameCopy, "/", &nextToken);
	while (nextToken!=0 && nextToken[0]!=0)
	{
		pAux = pNode->getChild(token);
		if (!pAux)
			pAux = pNode->addChild(token);
		pNode = pAux;

		token = strtok_s(0, "/", &nextToken);
	}

	//output the stripped parameter's name
	if (outStrippedName)
		strcpy_s(outStrippedName,MAX_PARAMETER_NAME_SIZE, token);

	return pNode;
}

CParameter* CParameters::getParameter(const char* paramName)
{
	for (int i = 0; i<m_numParameters; i++)
	{
		if (strcmp(m_pParameters[i].getName(), paramName) == 0)
			return &m_pParameters[i];
	}
	return 0;
}

CParameter* CParameters::getParameter(int index)
{
	if (index >= 0 && index < m_numParameters)
		return &m_pParameters[index];
	return 0;
}

bool CParameters::exists(const char* paramName)
{
	if (getParameter(paramName))
		return true;
	return false;
}

CParameter* CParameters::setParameter(const CParameter& parameter)
{
	CParameter* pParameter = getParameter(parameter.getName());

	if (pParameter)
		*pParameter = parameter;

	return pParameter; //be it a valid pointer or null
}
CParameter* CParameters::setParameter(const CParameter* parameter)
{
	CParameter* pParameter = getParameter(parameter->getName());

	if (pParameter)
		*pParameter = *parameter;

	return pParameter; //be it a valid pointer or null
}

CParameter* CParameters::addParameter(const CParameter& parameter)
{
	CParameter *pParameter = setParameter(parameter);

	if (!pParameter)
	{
		if (!m_pParameters) allocate();

		m_pParameters[m_numParameters] = parameter;
		pParameter = &m_pParameters[m_numParameters];
		m_numParameters++;
	}
	return pParameter;
}
CParameter* CParameters::addParameter(const CParameter* parameter)
{
	CParameter *pParameter = setParameter(parameter);

	if (!pParameter)
	{
		if (!m_pParameters) allocate();

		m_pParameters[m_numParameters] = *parameter;
		pParameter = &m_pParameters[m_numParameters];
		m_numParameters++;
	}
	return pParameter;
}


CParameters* CParameters::getChild(int i)
{
	if (i < m_numChildren && i>=0)
		return m_pChildren[i];
	return 0;
}

CParameters* CParameters::getChild(const char* name)
{
	for (int i = 0; i < m_numChildren; i++)
	{
		if (strcmp(name, m_pChildren[i]->getName()) == 0)
			return m_pChildren[i];
	}
	return 0;
}

CParameters* CParameters::addChild(const char* name)
{
	CParameters* pChild= 0;
	char newName[MAX_PARAMETER_NAME_SIZE];

	//if a name is provided, we first check if the child already exists
	if (name)
		pChild= getChild(name);
	else
	{
		//if no name is provided, we make up a new name and create it
		sprintf_s(newName, MAX_PARAMETER_NAME_SIZE, "%d", m_numChildren);
		name = newName;
	}

	if (!pChild)
	{
		if (!m_pChildren) allocate();

		m_pChildren[m_numChildren] = new CParameters();
		pChild = m_pChildren[m_numChildren];
		pChild->setName(name);
		pChild->setParent(this);
		m_numChildren++;	
	}

	return pChild;
}