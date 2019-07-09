/*
	SimionZoo: A framework for online model-free Reinforcement Learning on continuous
	control problems

	Copyright (c) 2016 SimionSoft. https://github.com/simionsoft

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/
#pragma once

#include <vector>
using namespace std;

#include "parameters-numeric.h"

class Noise;

class NamedVarSet;
using State = NamedVarSet;
using Action = NamedVarSet;

class INetwork;

class DiscreteDeepPolicy
{
protected:
	//We store in this vector a vector the discretized values for each output action
	vector<double> m_discretizedAction;
	vector<double> m_argMaxAction; //only declared to use in evaluation of Q(s,a)
	vector<double> m_stateVector;
	vector<double> m_argMaxQValues;

	vector<string> m_outputActionVariables;

	int m_numTotalActionSamples = 0;
	int m_numSamplesPerActionVariable = 0;
	int m_numActionVariables = 0;

	void randomActionSelection(INetwork* pNetwork, const State* s, Action* a);
	void greedyActionSelection(INetwork* pNetwork, const State* s, Action* a);

	size_t getActionVariableIndex(double value);
public:
	static std::shared_ptr<DiscreteDeepPolicy> getInstance(ConfigNode* pConfigNode);

	DiscreteDeepPolicy(ConfigNode* pConfigNode);

	void initialize(MULTI_VALUE<STATE_VARIABLE>& inputState, MULTI_VALUE<ACTION_VARIABLE>& outputActions, int numSamplesPerActionVariable);

	size_t getActionIndex(const vector<double>& action, int actionOffset);

	virtual void selectAction(INetwork* pNetwork, const State* s, Action* a) = 0;
};

class DiscreteEpsilonGreedyDeepPolicy : public DiscreteDeepPolicy
{
protected:
	CHILD_OBJECT_FACTORY<NumericValue> m_epsilon;
public:
	DiscreteEpsilonGreedyDeepPolicy(ConfigNode* pConfigNode);

	virtual void selectAction(INetwork* pNetwork, const State* s, Action* a);
};

class DiscreteSoftmaxDeepPolicy : public DiscreteDeepPolicy
{
protected:
	CHILD_OBJECT_FACTORY<NumericValue> m_temperature;
public:
	DiscreteSoftmaxDeepPolicy(ConfigNode* pConfigNode);

	virtual void selectAction(INetwork* pNetwork, const State* s, Action* a);
};

class DiscreteExplorationDeepPolicy : public DiscreteDeepPolicy
{
protected:
	int m_lastAction = 0;
	double m_epsilon = 0.98;
public:
	DiscreteExplorationDeepPolicy(ConfigNode* pConfigNode);

	virtual void selectAction(INetwork* pNetwork, const State* s, Action* a);
};

class NoisePlusGreedyDeepPolicy : public DiscreteDeepPolicy
{
	MULTI_VALUE_FACTORY<Noise> m_noiseSignals;
public:
	NoisePlusGreedyDeepPolicy(ConfigNode* pConfigNode);

	virtual void selectAction(INetwork* pNetwork, const State* s, Action* a);
};