#include "deep-layer.h"

DeepLayer::DeepLayer(ConfigNode* pConfigNode)
{
	m_activationFunction = ENUM_PARAM<ActivationFunction>(pConfigNode, "Activation", "Activation function used by the layer", ActivationFunction::Sigmoid).get();
	m_numUnits = INT_PARAM(pConfigNode, "Num-Units", "Number of neural units in the layer", 100).get();
}

int DeepLayer::getNumUnits()
{
	return m_numUnits;
}

ActivationFunction DeepLayer::getActivationFunction()
{
	return m_activationFunction;
}