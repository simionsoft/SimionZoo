#pragma once

#include "parameters.h"
#include "parameters-numeric.h"
#include "deep-layer.h"
#include "deferred-load.h"

class DeepLearner;

class DeepFunction: public DeferredLoad
{

	MULTI_VALUE<DeepLayer> m_layers;
	CHILD_OBJECT_FACTORY<DeepLearner> m_learner;
	BOOL_PARAM m_useMinibatchNormalization;
	INT_PARAM m_minibatchSize;

public:
	DeepFunction(ConfigNode* pConfigNode);

	void DeferredLoadStep();
};