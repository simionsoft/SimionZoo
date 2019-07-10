#pragma once

#include "parameters-numeric.h"
#include <string>
using namespace std;

class ConfigNode;

class DeepLearner
{
	CHILD_OBJECT_FACTORY<NumericValue> m_learningRate;
public:
	DeepLearner(ConfigNode* pConfigNode);
	virtual ~DeepLearner();

	static std::shared_ptr<DeepLearner> getInstance(ConfigNode* pConfigNode);

	double getLearningRate();
	virtual string to_string()= 0;
};

class DeepLearnerSGD: public DeepLearner
{
public:
	DeepLearnerSGD(ConfigNode* pConfigNode);
	string to_string() { return "SGD"; }
};

class DeepLearnerMomentumSGD : public DeepLearner
{
public:
	DeepLearnerMomentumSGD(ConfigNode* pConfigNode);
	string to_string() { return "MomentumSGD"; }
};

class DeepLearnerAdam : public DeepLearner
{
public:
	DeepLearnerAdam(ConfigNode* pConfigNode);
	string to_string() { return "Adam"; }
};