#pragma once

#include "parameters.h"

class DeepLayer
{
	ENUM_PARAM<Activation> m_activationFunction;
	INT_PARAM m_numUnits;
public:
	DeepLayer(ConfigNode* pConfigNode);

	int getNumUnits();
	Activation getActivationFunction();
};
