#pragma once

enum class Activation { ELU, ReLU, Sigmoid, SoftMax, SoftPlus, Tanh };
class ConfigNode;

#include <string>
using namespace std;

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
