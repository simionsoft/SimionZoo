#include "stdafx.h"
#include "parameters.h"


//int m_numParameters;
//	CParameter *m_pParameters;

CParameters::CParameters(char* parameterFile)
{
	m_numParameters= 0;
	m_pParameters= new CParameter [MAX_NUM_PARAMETERS];

	if (!parameterFile) return;

	loadParameters(parameterFile);
}

CParameters::~CParameters()
{
	if (m_pParameters) delete [] m_pParameters;
}
	


void CParameters::loadParameters(char* parameterFile)
{
	char description[MAX_PARAMETER_NAME_SIZE];
	char stringValue[MAX_STRING_SIZE];

	char line[MAX_LINE_LENGTH];
	double value = 0.0;
	int result;
	char* ret;
	int length;
	char* pComment;


	FILE* stream;
	printf("Loading %s...",parameterFile);
	fopen_s(&stream,parameterFile,"r");

	if (stream)
	{
		printf("ok\n");
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
			result= sscanf_s(line, "%s : %lf", description,MAX_PARAMETER_NAME_SIZE, &value);
			if (result==2 && strstr(line,",")==0)
			{
				strcpy_s(m_pParameters[m_numParameters].name,description);
				m_pParameters[m_numParameters].pValue= (void*) new double;
				* (double*) m_pParameters[m_numParameters].pValue= value;
				m_pParameters[m_numParameters].type= NUMERIC_PARAMETER;
				m_numParameters++;
			}
			else
			{
				result= sscanf_s(line,"%s : %[^\n]s",description,MAX_PARAMETER_NAME_SIZE,stringValue,MAX_STRING_SIZE);
				if (result==2)
				{
					strcpy_s(m_pParameters[m_numParameters].name,MAX_PARAMETER_NAME_SIZE
						,description);
					length= MAX_STRING_SIZE;//strlen(stringValue)+1;
					m_pParameters[m_numParameters].pValue= (void*) new char[length];
					strcpy_s((char*)m_pParameters[m_numParameters].pValue,MAX_STRING_SIZE,stringValue);
					m_pParameters[m_numParameters].type= STRING_PARAMETER;
					m_numParameters++;
					/*addParameter(description,string(stringValue));*/

					//if (strcmp(description,"ADDITIONAL_CONFIG_FILES")==0)
					//{
					//	pToken= strtok(stringValue,",");
					//	while(pToken)
					//	{
					//		loadParameters(pToken);
					//		pToken= strtok(0,",");
					//	}
					//}
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

void CParameters::saveParameters(char *parameterFile)
{

	FILE *stream;
	
	fopen_s(&stream,parameterFile,"w");
	

	if (stream)
	{

		for (int i= 0; i<m_numParameters; i++)
		{
			if(m_pParameters[i].type==NUMERIC_PARAMETER)
				fprintf(stream, "%s : %f\n",m_pParameters[i].name,*(double*)m_pParameters[i].pValue);
			else
				fprintf(stream, "%s : %s\n",m_pParameters[i].name,(char*)m_pParameters[i].pValue);
		}
		fclose(stream);
	}
	else printf("ERROR: Can't save file %s",parameterFile);
	//fprintf(stream,"# END-OF-PARAMETERS #");
}



double * CParameters::add(char *parametername, double value)
{
	assert(m_numParameters<MAX_NUM_PARAMETERS-1);
	m_pParameters[m_numParameters].pValue= new double;
	*((double*)m_pParameters[m_numParameters].pValue)= value;
	m_pParameters[m_numParameters].type= NUMERIC_PARAMETER;
	strcpy_s(m_pParameters[m_numParameters].name,MAX_PARAMETER_NAME_SIZE,parametername);
	m_numParameters++;
	return (double*) m_pParameters[m_numParameters-1].pValue;
}

int CParameters::getParameterIndex(char* parameterName)
{
	for (int i= 0; i<m_numParameters; i++)
	{
		if (strcmp(m_pParameters[i].name,parameterName)==0)
			return i;
	}
	return -1;
}


char *CParameters::getStringPtr(char* parameterName)
{
	int index= getParameterIndex(parameterName);

	return getStringPtr(index);
}

double *CParameters::getDoublePtr(char* parameterName)
{
	int index= getParameterIndex(parameterName);
	assert(index>=0);

	return getDoublePtr(index);
}

double CParameters::getDouble(char* parameterName)
{
	int index= getParameterIndex(parameterName);
	assert(index>=0);

	return *getDoublePtr(index);
}

char *CParameters::getParameterName(int index)
{
	if (index>=0)
		return m_pParameters[index].name;

	return 0;
}

char *CParameters::getStringPtr(int index)
{
	if (index>=0 && m_pParameters[index].type==STRING_PARAMETER)
		return (char*)m_pParameters[index].pValue;

	return 0;
}

double *CParameters::getDoublePtr(int index)
{
	if (index>=0 && m_pParameters[index].type==NUMERIC_PARAMETER)
		return (double*)m_pParameters[index].pValue;

	return 0;
}

bool CParameters::exists(char* paramName)
{
	for (int i= 0; i<m_numParameters; i++)
	{
		if (strcmp(m_pParameters[i].name,paramName)==0)
			return true;
	}
	return false;
}
