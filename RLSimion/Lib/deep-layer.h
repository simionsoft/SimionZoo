#pragma once

#include "parameters.h"
#include <string>
using namespace std;

enum class Activation { ELU, ReLU, Sigmoid, SoftMax, SoftPlus, Tanh };

class DeepLayer
{
	Activation m_activationFunction;
	int m_numUnits;
public:
	DeepLayer() {} //so that we can use vector<DeepLayer>
	DeepLayer(ConfigNode* pConfigNode);

	int getNumUnits();
	Activation getActivationFunction();

	string to_string();

	static string activationFunctionName(Activation activation);
	static Activation activationFromFunctionName(string name);
};
