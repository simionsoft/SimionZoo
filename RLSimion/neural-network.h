#pragma once
#include <string>
using namespace std;

//In progress:: NOT USABLE YET
/*
class NeuralNetwork : public StateActionFunction
{
	vector<string> m_inputStateVariables;
	vector<string> m_inputActionVariables;

	size_t m_inputSize = 0;
protected:
	INetwork * m_pNetwork = nullptr;
	NeuralNetwork() = default;
	NeuralNetwork(INetwork* pNetwork, vector<string>& inputStateVariables, vector<string>& inputActionVariables, int outputSize);

	size_t m_outputSize = 0;

	vector<double> m_inputStateBuffer;
	vector<double> m_inputActionBuffer;
	vector<double> m_outputBuffer;
public:
	virtual ~NeuralNetwork();

	size_t getInputSize();
	size_t getOutputSize();
	unsigned int getNumOutputs();
	vector<double>& evaluate(const State* s, const Action* a);

	const vector<string>& getInputStateVariables() { return m_inputStateVariables; }
	const vector<string>& getInputActionVariables() { return m_inputActionVariables; }
};

class NNQFunctionDiscreteActions : public NeuralNetwork
{
	vector<double> m_outputActionValues;

	NNQFunctionDiscreteActions() = default;
public:
	NNQFunctionDiscreteActions(INetwork* pNetwork, vector<string> &inputStateVariables, string outputAction, size_t numSteps, double min, double max);

	double getActionIndexOutput(size_t actionIndex);
	size_t getClosestOutputIndex(double value);

	NNQFunctionDiscreteActions* clone();
};

class NNQFunction : public NeuralNetwork
{
	NNQFunction() = default;
public:
	NNQFunction(INetwork* pNetwork, vector<string> &inputStateVariables, vector<string> &inputActionVariables);

	NNQFunction* clone();
};

class NNStateFunction : public NeuralNetwork
{
	NNStateFunction() = default;
public:
	NNStateFunction(INetwork* pNetwork, vector<string> &inputStateVariables, vector<string>& outputActionVariables);

	NNStateFunction* clone();
};

*/