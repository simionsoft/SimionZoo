#pragma once

#define MAX_NUM_PARAMETERS 512
#define MAX_PARAMETER_NAME_SIZE 256
#define MAX_STRING_SIZE 512
#define MAX_LINE_LENGTH (MAX_PARAMETER_NAME_SIZE + MAX_STRING_SIZE + 3)

#define NUMERIC_PARAMETER 1
#define STRING_PARAMETER 2

struct CParameter
{
	char name[MAX_PARAMETER_NAME_SIZE];
	void *pValue;
	int type;
};

class CParameters
{
	int m_numParameters;
	CParameter *m_pParameters;
public:
	CParameters(char* parameterFile);
	~CParameters();

	double *add(char *parametername, double value);
		
	void loadParameters(char *parameterFile);
	void saveParameters(char *parameterFile);

	char *getStringPtr(char* parameterName);
	double *getDoublePtr(char* parameterName);
	double getDouble(char* parameterName);

	char *getParameterName(int parameterIndex);
	char *getStringPtr(int parameterIndex);
	double *getDoublePtr(int parameterIndex);

	int getParameterIndex(char* parameterName);

	int getNumParameters(){return m_numParameters;}

	bool exists(char* parameterName);
};
