#pragma once
#include <vector>
#include "CNTKWrapper.h"
#include "CNTKLibrary.h"

using namespace std;

class COptimizerSetting;
class CInputData;

class CNetwork:public INetwork
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

	size_t getTotalSize();
	void destroy();

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

	INetwork* cloneNonTrainable() const;

	void train(std::unordered_map<std::string, std::vector<double>&>& inputDataMap, std::vector<double>& targetOutputData);
	void predict(std::unordered_map<std::string, std::vector<double>&>& inputDataMap, std::vector<double>& predictionData);
};