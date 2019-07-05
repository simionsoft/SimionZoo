#pragma once

#include "parameters.h"
#include "parameters-numeric.h"
#include <string>
using namespace std;

class DeepLearner
{
	CHILD_OBJECT_FACTORY<NumericValue> m_learningRate;
public:
	DeepLearner(ConfigNode* pConfigNode);

	static std::shared_ptr<DeepLearner> getInstance(ConfigNode* pConfigNode);

	virtual string getDefinition()= 0;
	double getLearningRate();
};


class DeepLearnerSGD : public DeepLearner
{
public:
	DeepLearnerSGD(ConfigNode* pConfigNode);

	string getDefinition();

	const string Name = "SGD";
};

class DeepLearnerMomentumSGD : public DeepLearner
{
	DOUBLE_PARAM m_momentum;
public:
	DeepLearnerMomentumSGD(ConfigNode* pConfigNode);

	string getDefinition();

	const string Name = "MomentumSGD";
};

class DeepLearnerAdam : public DeepLearner
{
public:
	DeepLearnerAdam(ConfigNode* pConfigNode);

	string getDefinition();

	const string Name = "Adam";
};