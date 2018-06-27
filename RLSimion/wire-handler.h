#pragma once

#include <string>
using namespace std;

class Wire;

class WireHandler
{
public:
	virtual Wire* wireGet(string name) = 0;
	virtual void wireRegister(string name) = 0;
	virtual void wireRegister(string name, double minimum, double maximum) = 0;
};