#pragma once
#include <vector>
#include "../CNTKWrapper/CNTKWrapper.h"
#include "CNTKLibrary.h"

using namespace std;

class OptimizerSettings;
class InputData;
class NetworkDefinition;

class Network:public INetwork
{
	vector<double> m_output;
	const wstring m_lossName = L"Loss";
	const wstring m_targetName = L"Target";
	const wstring m_networkName = L"Network";

	void findInputsAndOutputs();
protected:
	bool m_bInputStateUsed = false;
	CNTK::Variable m_inputState;
	bool m_bInputActionUsed = false;
	CNTK::Variable m_inputAction;

	CNTK::FunctionPtr m_networkFunctionPtr;

	CNTK::FunctionPtr m_FunctionPtr;
	CNTK::FunctionPtr m_lossFunctionPtr;
	CNTK::Variable m_targetVariable;

	CNTK::TrainerPtr m_trainer;

	NetworkDefinition *m_pNetworkDefinition;

	unordered_map<CNTK::Parameter, CNTK::FunctionPtr> m_weightTransitions;

	void stateToVector(const State* s, vector<double>& stateVector);
	void actionToVector(const Action* a, vector<double>& actionVector);
public:
	Network(NetworkDefinition* pNetworkDefinition);
	~Network();

	void destroy();

	void setOutputLayer(CNTK::FunctionPtr outputLayer);
	void buildNetwork(double learningRate);

	void save(string fileName);

	INetwork* clone(bool bFreezeWeights= true) const;

	void initSoftUpdate(double u, INetwork* pTargetNetwork);
	void softUpdate(INetwork* pTargetNetwork);

	void train(IMinibatch* pMinibatch);

	void gradientWrtAction(const State* s, const Action* a, vector<double>& outputValues);
	void applyGradient(IMinibatch* pMinibatch);

	//StateActionFunction interface
	unsigned int getNumOutputs();
	vector<double>& evaluate(const State* s, const Action* a);
	const vector<string>& getInputStateVariables();
	const vector<string>& getInputActionVariables();
};