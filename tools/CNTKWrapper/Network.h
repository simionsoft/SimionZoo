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
public:
	Network(NetworkDefinition* pNetworkDefinition);
	~Network();

	void destroy();

	void setOutputLayer(CNTK::FunctionPtr outputLayer);
	void buildNetwork(double learningRate);

	void save(string fileName);

	INetwork* clone(bool bFreezeWeights= true) const;

	void initWeightTransition(double u, INetwork* pTargetNetwork);
	void performWeightTransition(INetwork* pTargetNetwork);

	void train(IMinibatch* pMinibatch);
	void get(const State* s, const Action* a, vector<double>& outputValues);
	double get(const State* s, const Action* a);
	/*
	void train(std::unordered_map<std::string, std::vector<double>&>& inputDataMap, std::vector<double>& targetOutputData);
	void predict(std::unordered_map<std::string, std::vector<double>&>& inputDataMap, std::vector<double>& predictionData);
	
	void gradients(std::unordered_map<std::string, std::vector<double>&>& inputDataMap, std::unordered_map<CNTK::Variable, CNTK::ValuePtr>& gradients);
	void gradients(std::unordered_map<std::string, std::vector<double>&>& inputDataMap, std::vector<double>& targetOutputData, std::unordered_map<CNTK::Variable, CNTK::ValuePtr>& gradients);
	*/
};