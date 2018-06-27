#pragma once

#include "named-var-set.h"

class Wire
{
	string m_name;
	double m_value;
	NamedVarProperties m_properties;
	bool m_bPropertiesSet;
public:
	Wire(string name);
	Wire(string name, double minimum, double maximum);
	virtual ~Wire();

	NamedVarProperties* getProperties();
	string getName();
	double getValue();
	void setValue(double value);
	void setLimits(double minimum, double maximum);
};

