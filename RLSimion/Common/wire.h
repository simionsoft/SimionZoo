#pragma once

#include <string>
using namespace std;

#include "named-var-set.h"

class Wire
{
	std::string m_name;
	double m_value;
	NamedVarProperties m_properties;
	bool m_bPropertiesSet;
public:
	Wire(std::string name);
	Wire(std::string name, double minimum, double maximum);
	virtual ~Wire();

	NamedVarProperties* getProperties();
	string getName();
	double getValue();
	void setValue(double value);
	void setLimits(double minimum, double maximum);
};

