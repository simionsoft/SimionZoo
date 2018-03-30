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
	CNTK::NDShape getInputShape();
	CNTK::NDShape getOutputShape();
protected:

	vector<CNTK::FunctionPtr> m_functionPtrs;
	vector<CNTK::FunctionPtr> m_outputsFunctionPtr;
	CNTK::FunctionPtr m_networkFunctionPtr;
	CNTK::FunctionPtr m_lossFunctionPtr;
	CNTK::TrainerPtr m_trainer;
	CNTK::Variable m_targetOutput;
	INetworkDefinition *m_pNetworkDefinition;
public:
	Network(INetworkDefinition* pNetworkDefinition);
	~Network();

	size_t getTotalSize();
	void destroy();

	vector<CNTK::FunctionPtr>& getFunctionPtrs() { return m_functionPtrs; }
	vector<CNTK::FunctionPtr>& getOutputsFunctionPtr() { return m_outputsFunctionPtr; }

	void buildQNetwork();
	void save(string fileName);

	INetwork* clone() const;

	void train(IMinibatch* pMinibatch);
	void get(State* s, vector<double>& outputValues);

	void train(std::unordered_map<std::string, std::vector<double>&>& inputDataMap, std::vector<double>& targetOutputData);
	void predict(std::unordered_map<std::string, std::vector<double>&>& inputDataMap, std::vector<double>& predictionData);
	
	void gradients(std::unordered_map<std::string, std::vector<double>&>& inputDataMap, std::unordered_map<CNTK::Variable, CNTK::ValuePtr>& gradients);
	void gradients(std::unordered_map<std::string, std::vector<double>&>& inputDataMap, std::vector<double>& targetOutputData, std::unordered_map<CNTK::Variable, CNTK::ValuePtr>& gradients);
};