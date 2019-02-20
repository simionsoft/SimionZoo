#pragma once

#include "CNTKWrapper.h"
#include "../Common/named-var-set.h"
#include <vector>
using namespace std;
class NetworkDefinition;

class Minibatch: public IMinibatch
{
	NetworkDefinition* m_pNetworkDefinition;
	size_t m_numTuples = 0;
	size_t m_size = 0;
	vector<double> m_inputState;
	vector<double> m_inputAction;

	size_t m_outputSize = 0;
	vector<double> m_output;
public:
	Minibatch(size_t size, NetworkDefinition* pNetworkDefinition, size_t outputSize= 0);
	virtual ~Minibatch();

	void clear();
	void addTuple(const State* s, const Action* a, const vector<double>& targetValues);
	void addTuple(const State* s, const Action* a, double targetValue);
	vector<double>& getInputState();
	vector<double>& getInputAction();
	vector<double>& getOutput();
	void destroy();
	bool isFull() const;
	size_t size() const;
	size_t outputSize() const;
};