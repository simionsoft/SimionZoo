#pragma once

enum class Activation { Linear, ELU, ReLU, Sigmoid, SoftMax, SoftPlus, Tanh };
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

	static string activationFunctionName(Activation activation)
	{
		switch (activation)
		{
		case Activation::ELU: return "ELU";
		case Activation::ReLU: return "ReLU";
		case Activation::Sigmoid: return "Sigmoid";
		case Activation::SoftMax: return "SoftMax";
		case Activation::SoftPlus: return "SoftPlus";
		case Activation::Tanh: return "Tanh";
		case Activation::Linear:
		default:
			return "Linear";
		}
	}
	static Activation activationFromFunctionName(string name)
	{
		if (name == "ELU") return Activation::ELU;
		else if (name == "ReLU") return Activation::ReLU;
		else if (name == "Sigmoid") return Activation::Sigmoid;
		else if (name == "SoftMax") return Activation::SoftMax;
		else if (name == "SoftPlus") return Activation::SoftPlus;
		else if (name == "Tanh") return Activation::Tanh;
		return Activation::Linear; //default value
	}
};
