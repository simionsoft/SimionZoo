#pragma once

#define VAR_NAME_MAX_LENGTH 128

class CParameters;

struct CNamedVarProperties
{
	char name[VAR_NAME_MAX_LENGTH];
	char units[VAR_NAME_MAX_LENGTH];
	double min;
	double max;
	CNamedVarProperties();
};



class CNamedVarSet
{
	CNamedVarProperties *m_pProperties;
	double *m_pValues;
	int m_numVars;
public:
	CNamedVarSet(CParameters* pDescription);
	CNamedVarSet(int numVars);
	~CNamedVarSet();

	int getNumVars(){return m_numVars;}

	int getVarIndex(const char* name);

	const char* getName(int i);
	double getMin(int i);
	double getMax(int i);
	double getMin(const char* name);
	double getMax(const char* name);
	double getRangeWidth(int i);

	void setName(int i,const char* name);
	void setMin(int i,double min);
	void setMax(int i,double max);

	void setProperties(int i,const char* name, double min, double max);

private:
	//private to force the use of indices instead of names. How much more efficient?
	double getValue(const char* varName);
	void setValue(const char* varName, double value);
public:

	double* getValueVector(){return m_pValues;}

	double getValue(int i);
	double* getValuePtr(int i);
	void setValue(int i, double value);

	double getSumValue(); //to get the scalarised reward easily

	void copy(CNamedVarSet* nvs);

	CNamedVarSet* getInstance();
};

typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;
typedef CNamedVarSet CReward;

