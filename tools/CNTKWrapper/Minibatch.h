#pragma once

#include "CNTKWrapper.h"
#include "../../RLSimion/named-var-set.h"
#include <vector>
using namespace std;
class INetworkDefinition;

class Minibatch: public IMinibatch
{
	INetworkDefinition* m_pNetworkDefinition;
	size_t m_numTuples = 0;
	size_t m_size = 0;
	vector<double> m_inputs;
	vector<double> m_targetOutputs;
public:
	Minibatch(size_t size, INetworkDefinition* pNetworkDefinition);
	virtual ~Minibatch();

	void clear();
	void addTuple(const State* s, vector<double>& targetValues);
	vector<double>& getInputVector();
	vector<double>& getTargetOutputVector();
	void destroy();
	bool isFull();
};

