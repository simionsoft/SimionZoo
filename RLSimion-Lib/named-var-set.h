#pragma once

#define VAR_NAME_MAX_LENGTH 128
#include <vector>

class CNamedVarProperties
{
	char m_name[VAR_NAME_MAX_LENGTH];
	char m_units[VAR_NAME_MAX_LENGTH];
	double m_min;
	double m_max;
	bool m_bCircular;
public:
	CNamedVarProperties(const char* name, const char* units, double min, double max);
	const char* getName() const { return m_name; }
	void setName(const char* name);
	double getMin() const { return m_min; }
	double getMax() const { return m_max; }
	double getRangeWidth(int i) const { return m_max - m_min; }
	void setCircular(bool bCircular) { m_bCircular = bCircular; }
	bool bIsCircular() const { return m_bCircular; }
};

class CNamedVarSet;

class CDescriptor
{
	std::vector<CNamedVarProperties*> m_pProperties;
public:
	CNamedVarSet* getInstance();
	size_t size() const { return m_pProperties.size(); }
	CNamedVarProperties& operator[](int idx) { return *m_pProperties[idx]; }
	const CNamedVarProperties& operator[](int idx) const { return *m_pProperties[idx]; }
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

	double get(const char* varName) const;
	void set(const char* varName, double value);

	double* getValueVector(){return m_pValues;}

	double get(int i) const;
	double* getValuePtr(int i);
	void set(int i, double value);

	//returns the sum of all the values, i.e. used to scalarise a reward vector
	double getSumValue() const;

	void copy(CNamedVarSet* nvs);
	CNamedVarProperties& getProperties(int i) const { return m_pProperties[i]; }
	CNamedVarProperties& getProperties(const char* varName) const;
	CDescriptor& getProperties() { return m_pProperties; }
	CDescriptor* getPropertiesPtr() { return &m_pProperties; }
};

using CState= CNamedVarSet;
using CAction= CNamedVarSet;
using CReward= CNamedVarSet;

