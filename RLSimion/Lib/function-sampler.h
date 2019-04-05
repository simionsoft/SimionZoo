#pragma once

#include <vector>
using namespace std;

class StateActionFunction;
class NamedVarSet;
using State = NamedVarSet;
using Action = NamedVarSet;
class Descriptor;

enum VariableSource {StateSource, ActionSource};

class FunctionSampler
{
protected:
	size_t m_outputIndex = 0;
	size_t m_numInputs = 2;
	State* m_pState = nullptr;
	Action* m_pAction = nullptr;

	StateActionFunction* m_pFunction;
	size_t m_samplesPerDimension;
	vector<double> m_sampledValues;
	size_t m_numSamples;
	size_t m_numOutputs;
	string m_functionId;

	NamedVarSet* Source(VariableSource source);
public:
	FunctionSampler(string functionId, StateActionFunction* pFunction, size_t outputIndex, size_t samplesPerDimension, size_t numDimensions
		, Descriptor& stateDescriptor, Descriptor& actionDescriptor);
	virtual ~FunctionSampler();

	size_t getNumOutputs() const;
	virtual const vector<double>& sample() = 0;

	virtual string getFunctionId() const = 0;
	virtual size_t getNumSamplesX() = 0;
	virtual size_t getNumSamplesY() = 0;
};

class FunctionSampler3D : public FunctionSampler
{
	NamedVarSet* m_xVarSource;
	string m_xVarName;
	NamedVarSet* m_yVarSource;
	string m_yVarName;
public:
	FunctionSampler3D(string functionId, StateActionFunction* pFunction, size_t outputIndex, size_t samplesPerDimension
		, Descriptor& stateDescriptor, Descriptor& actionDescriptor
		, VariableSource xVarSource, string xVarName, VariableSource yVarSource, string yVarName);
	
	const vector<double>& sample();

	string getFunctionId() const;
	size_t getNumSamplesX();
	size_t getNumSamplesY();
};

class FunctionSampler2D : public FunctionSampler
{
	NamedVarSet* m_xVarSource;
	string m_xVarName;
public:
	FunctionSampler2D(string functionId, StateActionFunction* pFunction, size_t outputIndex, size_t samplesPerDimension
		, Descriptor& stateDescriptor, Descriptor& actionDescriptor, VariableSource xVarSource, string xVarName);

	const vector<double>& sample();

	string getFunctionId() const;
	size_t getNumSamplesX();
	size_t getNumSamplesY();
};