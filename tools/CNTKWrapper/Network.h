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
	vector<CNTK::Variable> m_inputs;

	CNTK::FunctionPtr m_networkFunctionPtr;

	CNTK::FunctionPtr m_QFunctionPtr;
	CNTK::FunctionPtr m_lossFunctionPtr;
	CNTK::Variable m_targetVariable;

	CNTK::TrainerPtr m_trainer;

	NetworkDefinition *m_pNetworkDefinition;
public:
	Network(NetworkDefinition* pNetworkDefinition);
	~Network();

	void destroy();

	void setOutputLayer(CNTK::FunctionPtr outputLayer);
	void buildQNetwork(double learningRate);

	void save(string fileName);

	INetwork* getFrozenCopy() const;

	void train(IMinibatch* pMinibatch);
	void get(const State* s, vector<double>& outputValues);

	void train(std::unordered_map<std::string, std::vector<double>&>& inputDataMap, std::vector<double>& targetOutputData);
	void predict(std::unordered_map<std::string, std::vector<double>&>& inputDataMap, std::vector<double>& predictionData);
	
	void gradients(std::unordered_map<std::string, std::vector<double>&>& inputDataMap, std::unordered_map<CNTK::Variable, CNTK::ValuePtr>& gradients);
	void gradients(std::unordered_map<std::string, std::vector<double>&>& inputDataMap, std::vector<double>& targetOutputData, std::unordered_map<CNTK::Variable, CNTK::ValuePtr>& gradients);
};