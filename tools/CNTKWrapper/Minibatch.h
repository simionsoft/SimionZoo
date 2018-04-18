#pragma once

#include "CNTKWrapper.h"
#include "../../RLSimion/named-var-set.h"
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
	vector<double> m_output;
public:
	Minibatch(size_t size, NetworkDefinition* pNetworkDefinition);
	virtual ~Minibatch();

	void clear();
	void addTuple(const State* s, const Action* a, vector<double>& targetValues);
	void addTuple(const State* s, const Action* a, double targetValue);
	vector<double>& getInputState();
	vector<double>& getInputAction();
	vector<double>& getOutput();
	void destroy();
	bool isFull();
};

