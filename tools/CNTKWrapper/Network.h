#pragma once
#include <vector>
#include "CNTKWrapper.h"
#include "CNTKLibrary.h"

using namespace std;

class OptimizerSetting;
class InputData;
class Problem;

class Network:public INetwork
{
protected:
	vector<InputData*> m_inputs;
	vector<CNTK::FunctionPtr> m_functionPtrs;
	vector<CNTK::FunctionPtr> m_outputsFunctionPtr;
	CNTK::FunctionPtr m_networkFunctionPtr;
	CNTK::FunctionPtr m_lossFunctionPtr;
	CNTK::TrainerPtr m_trainer;
	CNTK::Variable m_targetOutput;
	Problem *m_pParent;
public:
	Network();
	~Network();

	size_t getTotalSize();
	void destroy();

	vector<InputData*>& getInputs() { return m_inputs; }
	vector<CNTK::FunctionPtr>& getOutputsFunctionPtr() { return m_outputsFunctionPtr; }
	vector<CNTK::FunctionPtr>& getFunctionPtrs() { return m_functionPtrs; }
	CNTK::FunctionPtr getNetworkFunctionPtr() { return m_networkFunctionPtr; }
	CNTK::FunctionPtr getLossFunctionPtr() { return m_lossFunctionPtr; }
	CNTK::TrainerPtr getTrainer() { return m_trainer; }
	CNTK::Variable getTargetOutput() { return m_targetOutput; }

	void buildNetworkFunctionPtr(const OptimizerSetting* optimizer);
	void save(string fileName);
	static Network load(string fileName, CNTK::DeviceDescriptor &device);

	virtual void setParent(IProblem* pParent);

	INetwork* cloneNonTrainable() const;

	void train(std::unordered_map<std::string, std::vector<double>&>& inputDataMap, std::vector<double>& targetOutputData);
	void predict(std::unordered_map<std::string, std::vector<double>&>& inputDataMap, std::vector<double>& predictionData);
	
	void gradients(std::unordered_map<std::string, std::vector<double>&>& inputDataMap, std::unordered_map<CNTK::Variable, CNTK::ValuePtr>& gradients);
	void gradients(std::unordered_map<std::string, std::vector<double>&>& inputDataMap, std::vector<double>& targetOutputData, std::unordered_map<CNTK::Variable, CNTK::ValuePtr>& gradients);
};