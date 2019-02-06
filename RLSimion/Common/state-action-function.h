#pragma once

#ifndef __STATE_ACTION_FUNCTION__ //this avoids compilation problems with g++ when included from different projects
#define __STATE_ACTION_FUNCTION__

#include <vector>
#include <string>
using namespace std;

class NamedVarSet;
using State = NamedVarSet;
using Action = NamedVarSet;

class StateActionFunction
{
public:

	virtual unsigned int getNumOutputs() = 0;
	virtual vector<double>& evaluate(const State* s, const Action* a) = 0;
	virtual const vector<string>& getInputStateVariables() = 0;
	virtual const vector<string>& getInputActionVariables() = 0;
};

#endif //__STATE_ACTION_FUNCTION