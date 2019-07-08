#include "deep-layer.h"

DeepLayer::DeepLayer(ConfigNode* pConfigNode)
{
	m_activationFunction = ENUM_PARAM<Activation>(pConfigNode, "Activation", "Activation function used by the layer", Activation::Sigmoid);
	m_numUnits = INT_PARAM(pConfigNode, "Num-Units", "Number of neural units in the layer", 100);
}

int DeepLayer::getNumUnits()
{
	return m_numUnits.get();
}

Activation DeepLayer::getActivationFunction()
{
	return m_activationFunction.get();
}