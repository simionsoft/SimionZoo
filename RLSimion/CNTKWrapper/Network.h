#pragma once
#include <vector>
#include "CNTKWrapper.h"
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

	vector<double> m_stateVector;
	vector<double> m_actionVector;
protected:
	bool m_bInputStateUsed = false;
	CNTK::Variable m_s;
	bool m_bInputActionUsed = false;
	CNTK::Variable m_a;

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

	INetworkDefinition* getDefinition();

	void save(string fileName);

	INetwork* clone(bool bFreezeWeights= true) const;

	void initSoftUpdate(double u, INetwork* pTargetNetwork);
	void softUpdate(INetwork* pTargetNetwork);

	void train(IMinibatch* pMinibatch, const vector<double>& target);

	void gradientWrtAction(const vector<double>& s, const vector<double>& a, vector<double>& outputGradient);
	void applyGradient(IMinibatch* pMinibatch, const vector<double>& target);

	//StateActionFunction interface
	unsigned int getNumOutputs();
	vector<double>& evaluate(const State* s, const Action* a);

	void evaluate(const vector<double>& s, const vector<double>& a, vector<double>& output);

	const vector<string>& getInputStateVariables();
	const vector<string>& getInputActionVariables();
};