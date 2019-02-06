#pragma once

constexpr auto VAR_NAME_MAX_LENGTH = 128;
#include <vector>

class WireHandler;
class NamedVarSet;

using namespace std;


class NamedVarProperties
{
	char m_name[VAR_NAME_MAX_LENGTH];
	char m_units[VAR_NAME_MAX_LENGTH];
	double m_min;
	double m_max;
	bool m_bCircular;
public:
	//bCircular must be true for variables holding angles
	NamedVarProperties();
	NamedVarProperties(const char* name, const char* units, double min, double max, bool bCircular= false);
	const char* getName() const { return m_name; }
	void setName(const char* name);
	double getMin() const { return m_min; }
	double getMax() const { return m_max; }
	void setMin(double minimum) { m_min = minimum; }
	void setMax(double maximum) { m_max = maximum; }
	double getRangeWidth() const { return m_max - m_min; }
	void setCircular(bool bCircular) { m_bCircular = bCircular; }
	bool isCircular() const { return m_bCircular; }
	const char* getUnits() const { return m_units; }
};



class Descriptor
{
	WireHandler* m_pWireHandler = nullptr;
	NamedVarProperties wireDummyProperties;
	std::vector<NamedVarProperties*> m_descriptor;
public:
	Descriptor() = default;
	Descriptor(WireHandler* pWireHandler) { m_pWireHandler = pWireHandler; }

	NamedVarSet* getInstance();
	WireHandler* getWireHandler() { return m_pWireHandler; }
	size_t size() const { return m_descriptor.size(); }
	NamedVarProperties* getProperties(const char* name);
	NamedVarProperties& operator[](size_t idx) { return *m_descriptor[idx]; }
	const NamedVarProperties& operator[](size_t idx) const { return *m_descriptor[idx]; }
	size_t addVariable(const char* name, const char* units, double min, double max, bool bCircular= false);
};

class NamedVarSet
{
	Descriptor &m_descriptor;
	double *m_pValues;
	size_t m_numVars;

	double normalize(const char* varName, double value) const;
	double denormalize(const char*, double value) const;
public:
	NamedVarSet(Descriptor& descriptor);
	virtual ~NamedVarSet();

	size_t getNumVars() const{ return m_numVars; }

	double* getValueVector(){return m_pValues;}

	//these two methods return the absolute value
	double get(size_t i) const;
	double get(const char* varName) const;
	//these two methods return the value normalized in its value range
	double getNormalized(const char* varName) const;

	double* getValuePtr(size_t i);
	double& getRef(size_t i);

	//these two methods accept absolute values
	void set(const char* varName, double value);
	void set(size_t i, double value);
	//these two methods accept normalized values that are de-normalized before storing them
	void setNormalized(const char* varName, double value);

	//returns the sum of all the values, i.e. used to scalarise a reward vector
	double getSumValue() const;

	void copy(const NamedVarSet* nvs);
	NamedVarProperties* getProperties(size_t i) const { return &m_descriptor[i]; }
	NamedVarProperties* getProperties(const char* varName) const;
	Descriptor& getDescriptor() { return m_descriptor; }
	Descriptor* getDescriptorPtr() { return &m_descriptor; }

	void addOffset(double offset);
};

using State= NamedVarSet;
using Action= NamedVarSet;
using Reward= NamedVarSet;

