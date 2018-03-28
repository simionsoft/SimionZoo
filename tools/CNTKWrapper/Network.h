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
	CNTK::Variable m_inputVariable;
	CNTK::NDShape getInputShape();
	CNTK::NDShape getOutputShape();
protected:
	vector<InputData*> m_inputs;
	vector<CNTK::FunctionPtr> m_functionPtrs;
	vector<CNTK::FunctionPtr> m_outputsFunctionPtr;
	CNTK::FunctionPtr m_networkFunctionPtr;
	CNTK::FunctionPtr m_lossFunctionPtr;
	CNTK::TrainerPtr m_trainer;
	CNTK::Variable m_targetOutput;
	INetworkDefinition *m_pParent;
public:
	Network();
	~Network();

	size_t getTotalSize();
	void destroy();

	vector<CNTK::FunctionPtr>& getFunctionPtrs() { return m_functionPtrs; }
	vector<InputData*>& getInputs() { return m_inputs; }
	vector<CNTK::FunctionPtr>& getOutputsFunctionPtr() { return m_outputsFunctionPtr; }

	void buildQNetwork();
	void save(string fileName);
	static Network load(string fileName, CNTK::DeviceDescriptor &device);

	void setParent(INetworkDefinition* pParent);

	INetwork* clone() const;

	void train(IMinibatch* pMinibatch);
	void get(State* s, vector<double>& outputValues);

	void train(std::unordered_map<std::string, std::vector<double>&>& inputDataMap, std::vector<double>& targetOutputData);
	void predict(std::unordered_map<std::string, std::vector<double>&>& inputDataMap, std::vector<double>& predictionData);
	
	void gradients(std::unordered_map<std::string, std::vector<double>&>& inputDataMap, std::unordered_map<CNTK::Variable, CNTK::ValuePtr>& gradients);
	void gradients(std::unordered_map<std::string, std::vector<double>&>& inputDataMap, std::vector<double>& targetOutputData, std::unordered_map<CNTK::Variable, CNTK::ValuePtr>& gradients);
};