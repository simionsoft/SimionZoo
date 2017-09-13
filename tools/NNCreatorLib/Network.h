#pragma once
#include <vector>
#include "InputData.h"

using namespace std;

class COptimizerSetting;

class CNetwork
{
protected:
	vector<CInputData*> m_inputs;
	vector<CNTK::FunctionPtr> m_functionPtrs;
	vector<CNTK::FunctionPtr> m_outputsFunctionPtr;
	CNTK::FunctionPtr m_networkFunctionPtr;
	CNTK::FunctionPtr m_lossFunctionPtr;
	CNTK::TrainerPtr m_trainer;
	CNTK::Variable m_targetOutput;

public:
	CNetwork();
	~CNetwork();

	vector<CInputData*>& getInputs() { return m_inputs; }
	vector<CNTK::FunctionPtr>& getOutputsFunctionPtr() { return m_outputsFunctionPtr; }
	vector<CNTK::FunctionPtr>& getFunctionPtrs() { return m_functionPtrs; }
	CNTK::FunctionPtr getNetworkFunctionPtr() { return m_networkFunctionPtr; }
	CNTK::FunctionPtr getLossFunctionPtr() { return m_lossFunctionPtr; }
	CNTK::TrainerPtr getTrainer() { return m_trainer; }
	CNTK::Variable getTargetOutput() { return m_targetOutput; }

	void buildNetworkFunctionPtr(const COptimizerSetting* optimizer);
	void save(string fileName);
	static CNetwork load(string fileName, CNTK::DeviceDescriptor &device);

	void train(std::vector<float>& inputData, std::vector<float>& targetOutputData, size_t numSamples);
	void predict(std::unordered_map<std::string, std::vector<float>&>& inputDataMap, std::vector<float>& predictionData, size_t numSamples);

	void train(std::unordered_map<std::string, CNTK::ValuePtr> inputDataMap, CNTK::ValuePtr targetOutputData);
	void predict(std::unordered_map<std::string, CNTK::ValuePtr> inputDataMap, std::vector<float>& predictionData);
};