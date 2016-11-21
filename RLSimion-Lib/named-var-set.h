#pragma once

#define VAR_NAME_MAX_LENGTH 128
#include <vector>

class CNamedVarProperties
{
	char m_name[VAR_NAME_MAX_LENGTH];
	char m_units[VAR_NAME_MAX_LENGTH];
	double m_min;
	double m_max;
public:
	CNamedVarProperties(const char* name, const char* units, double min, double max);
	const char* getName() const { return m_name; }
	void setName(const char* name);
	double getMin() const { return m_min; }
	double getMax() const { return m_max; }
	//double getMin(const char* name) const;
	//double getMax(const char* name) const;
	double getRangeWidth(int i) const;
};

class CNamedVarSet;

class CDescriptor
{
	std::vector<CNamedVarProperties> m_pProperties;
public:
	CNamedVarSet* getInstance();
	int size() const { return m_pProperties.size(); }
	CNamedVarProperties& operator[](int idx) { return m_pProperties[idx]; }
	const CNamedVarProperties& operator[](int idx) const { return m_pProperties[idx]; }
	int addVariable(const char* name, const char* units, double min, double max);
	int getVarIndex (const char* name);
};

class CNamedVarSet
{
	CDescriptor &m_pProperties;
	double *m_pValues;
	int m_numVars;
public:
	CNamedVarSet(CDescriptor& descriptor);
	virtual ~CNamedVarSet();

	int getNumVars() const{ return m_numVars; }

	int getVarIndex(const char* name) const;

public:

	double getValue(const char* varName) const;
	void setValue(const char* varName, double value);

	double* getValueVector(){return m_pValues;}

	double getValue(int i) const;
	double* getValuePtr(int i);
	void setValue(int i, double value);

	double getSumValue() const; //to get the scalarised reward easily

	void copy(CNamedVarSet* nvs);
	CNamedVarProperties& getProperties(int i) const { return m_pProperties[i]; }
	CNamedVarProperties& getProperties(const char* varName) const;
	CDescriptor& getProperties() { return m_pProperties; }
};

using CState= CNamedVarSet;
using CAction= CNamedVarSet;
using CReward= CNamedVarSet;

