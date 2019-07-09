#pragma once

#include "../Common/state-action-function.h"
class DeepMinibatch;

#include <vector>
using namespace std;

class IDeepNetwork: public StateActionFunction
{
public:
	virtual void destroy() = 0;

	virtual IDeepNetwork* clone(bool bFreezeWeights = true) const = 0;

	virtual void train(DeepMinibatch* pMinibatch, const vector<double>& target) = 0;

	virtual vector<double>& evaluate(const State* s, const Action* a) = 0;
	virtual void evaluate(const vector<double>& s, const vector<double>& a, vector<double>& output) = 0;
};