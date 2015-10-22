#ifndef __STATES_AND_ACTIONS__
#define __STATES_AND_ACTIONS__

#define VAR_NAME_MAX_LENGTH 128

class CParameters;

struct CNamedVarProperties
{
	char name[VAR_NAME_MAX_LENGTH];
	double min;
	double max;
	CNamedVarProperties()
	{
		name[0]= 0;
		min= 0.0;
		max= 0.0;
	}
};



class CNamedVarSet
{
	CNamedVarProperties *m_pProperties;
	double *m_pValues;
	int m_numVars;
public:
	CNamedVarSet(int numVars);
	//CNamedVarSet(char* descriptionFile);
	~CNamedVarSet();

	int getNumVars(){return m_numVars;}

	int getVarIndex(char* name);

	char* getName(int i);
	double getMin(int i);
	double getMax(int i);
	double getMin(char* name);
	double getMax(char* name);

	void setName(int i,char* name);
	void setMin(int i,double min);
	void setMax(int i,double max);

	void setProperties(int i, char* name, double min, double max);

	double getValue(char* varName);
	void setValue(char* varName, double value);
	
	double* getValueVector(){return m_pValues;}

	double getValue(int i);
	void setValue(int i, double value);

	void copy(CNamedVarSet* nvs);

	CNamedVarSet* getInstance();
};

typedef CNamedVarSet CState;
typedef CNamedVarSet CAction;

#endif