#pragma once

#include <vector>
using namespace std;

class StateActionFunction;
class NamedVarSet;
using State = NamedVarSet;
using Action = NamedVarSet;
class Descriptor;

class FunctionSampler
{
protected:
	size_t m_numInputs = 2;
	State* m_pState = nullptr;
	Action* m_pAction = nullptr;

	vector<NamedVarSet*> m_sampledVariableSources;
	vector<string> m_sampledVariableNames;

	StateActionFunction* m_pFunction;
	size_t m_samplesPerDimension;
	vector<double> m_sampledValues;
	size_t m_numSamples;
	size_t m_numOutputs;
public:
	FunctionSampler(StateActionFunction* pFunction, size_t samplesPerDimension, size_t numDimensions
		, Descriptor& stateDescriptor, Descriptor& actionDescriptor);

	size_t getNumOutputs() const;
	virtual const vector<double>& sample(unsigned int outputIndex = 0) = 0;
};

class FunctionSampler3D : public FunctionSampler
{
public:
	FunctionSampler3D(StateActionFunction* pFunction, size_t samplesPerDimension
		, Descriptor& stateDescriptor, Descriptor& actionDescriptor);
	const vector<double>& sample(unsigned int outputIndex = 0);
};

class FunctionSampler2D : public FunctionSampler
{
public:
	FunctionSampler2D(StateActionFunction* pFunction, size_t samplesPerDimension
		, Descriptor& stateDescriptor, Descriptor& actionDescriptor);
	const vector<double>& sample(unsigned int outputIndex = 0);
};