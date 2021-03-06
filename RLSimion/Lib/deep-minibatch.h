#pragma once


#include "../Common/named-var-set.h"
#include <vector>
using namespace std;

class DeepNetworkDefinition;

class DeepMinibatch
{
	DeepNetworkDefinition* m_pDeepFunction = nullptr;

	size_t m_numTuples = 0;
	size_t m_size = 0;

	vector<double> m_s;
	vector<double> m_a;
	vector<double> m_s_p;
	vector<double> m_r;
	vector<double> m_target;

public:
	DeepMinibatch(size_t minibatchSize, DeepNetworkDefinition* pDeepFunction);
	virtual ~DeepMinibatch();

	void clear();
	void addTuple(const State* s, const Action* a, const State* s_p, double r);
	void addTuple(const State* s, const Action* a, const State* s_p, double r, size_t index);

	vector<double>& s() { return m_s; }
	vector<double>& a() { return m_a; }
	vector<double>& s_p() { return m_s_p; }
	vector<double>& r() { return m_r; }
	vector<double>& target() { return m_target; }

	void copyElement(vector<double>& src, vector<double>&dst, size_t index);

	bool isFull() const;
	size_t size() const { return m_size; }
	size_t numTuples() const;
};