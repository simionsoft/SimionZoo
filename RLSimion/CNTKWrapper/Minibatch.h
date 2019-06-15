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
	vector<double> m_s;
	vector<double> m_a;
	vector<double> m_s_p;
	vector<double> m_r;

	vector<double> m_stateActionBuffer;

	size_t m_outputSize = 0;

public:
	Minibatch(size_t size, NetworkDefinition* pNetworkDefinition, size_t outputSize= 0);
	virtual ~Minibatch();

	void clear();
	void addTuple(const State* s, const Action* a, const State* s_p, double r);

	vector<double>& s();
	vector<double>& a();
	vector<double>& s_p();
	vector<double>& r();

	vector<double>& interleavedStateAction();

	void destroy();
	bool isFull() const;
	size_t size() const;
	size_t outputSize() const;
};