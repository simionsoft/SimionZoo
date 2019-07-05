#pragma once

#include "deep-activation-functions.h"
#include "parameters.h"

class DeepLayer
{
	ActivationFunction m_activationFunction;
	int m_numUnits;
public:
	DeepLayer(ConfigNode* pConfigNode);

	int getNumUnits();
	ActivationFunction getActivationFunction();
};
