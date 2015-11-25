#pragma once


#define MAX_PARAMETER_NAME_SIZE 512
#define MAX_STRING_SIZE 512


#define NOT_INITIALIZED 0
#define NUMERIC_PARAMETER 1
#define STRING_PARAMETER 2

class CParameter
{
	char name[MAX_PARAMETER_NAME_SIZE];
	void *pValue;
	int type;
public:
	CParameter(char* name, char* stringValue);
	CParameter(char* name, double value);
	CParameter();
	~CParameter();

	void releasePtr();

	CParameter& operator= (const CParameter& parameter);
	bool operator== (const CParameter& parameter);

	const char* getName() const;
	void setName(char* pName);
	bool nameIs(const char* pName) const;
	//use these methods only in constructors or file readers
	void setValue(char* string);
	void setValue(double value);
	void setValue(CParameter& parameter);
	const char* getStringPtr() const;
	double getDouble() const;
	void saveParameter(void* pFile, char* prefix) const;
};
