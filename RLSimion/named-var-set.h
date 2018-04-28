#pragma once

#define VAR_NAME_MAX_LENGTH 128
#include <vector>

class NamedVarProperties
{
	char m_name[VAR_NAME_MAX_LENGTH];
	char m_units[VAR_NAME_MAX_LENGTH];
	double m_min;
	double m_max;
	bool m_bCircular;
public:
	//bCircular must be true for variables holding angles
	NamedVarProperties(const char* name, const char* units, double min, double max, bool bCircular= false);
	const char* getName() const { return m_name; }
	void setName(const char* name);
	double getMin() const { return m_min; }
	double getMax() const { return m_max; }
	double getRangeWidth() const { return m_max - m_min; }
	void setCircular(bool bCircular) { m_bCircular = bCircular; }
	bool bIsCircular() const { return m_bCircular; }
	const char* getUnits() const { return m_units; }
};

class NamedVarSet;

class Descriptor
{
	std::vector<NamedVarProperties*> m_descriptor;
public:
	NamedVarSet* getInstance();
	size_t size() const { return m_descriptor.size(); }
	NamedVarProperties& operator[](int idx) { return *m_descriptor[idx]; }
	const NamedVarProperties& operator[](int idx) const { return *m_descriptor[idx]; }
	int addVariable(const char* name, const char* units, double min, double max, bool bCircular= false);
	int getVarIndex (const char* name);
};

class NamedVarSet
{
	Descriptor &m_descriptor;
	double *m_pValues;
	int m_numVars;

public:
	NamedVarSet(Descriptor& descriptor);
	virtual ~NamedVarSet();

	int getNumVars() const{ return m_numVars; }

	int getVarIndex(const char* name) const;

public:

	double get(const char* varName) const;
	void set(const char* varName, double value);

	double* getValueVector(){return m_pValues;}

	double get(int i) const;
	double* getValuePtr(int i);
	double& getRef(int i);
	void set(int i, double value);

	//returns the sum of all the values, i.e. used to scalarise a reward vector
	double getSumValue() const;

	void copy(const NamedVarSet* nvs);
	NamedVarProperties& getProperties(int i) const { return m_descriptor[i]; }
	NamedVarProperties& getProperties(const char* varName) const;
	Descriptor& getDescriptor() { return m_descriptor; }
	Descriptor* getDescriptorPtr() { return &m_descriptor; }

public:
	void addOffset(double offset);
};

using State= NamedVarSet;
using Action= NamedVarSet;
using Reward= NamedVarSet;

