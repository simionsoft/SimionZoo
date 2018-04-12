#pragma once
#include "CNTKWrapper.h"
#include "../../3rd-party/tinyxml2/tinyxml2.h"
#include "../../RLSimion/named-var-set.h"
#include <vector>

class NetworkArchitecture;
class InputData;
class Network;
class LinkConnection;
class OptimizerSettings;

class NetworkDefinition: public INetworkDefinition
{
	wstring m_stateInputLayer;
	wstring m_actionInputLayer;
	wstring m_outputLayerName;
protected:
	vector<size_t> m_inputStateVars;
	vector<size_t> m_inputActionVars;

	size_t m_outputActionVar;
	vector<double> m_outputActionValues;

	NetworkArchitecture* m_pNetworkArchitecture;
	LinkConnection* m_pOutput;
	OptimizerSettings* m_pOptimizerSetting;
	
public:
	NetworkDefinition();
	~NetworkDefinition();
	NetworkDefinition(tinyxml2::XMLElement* pNode);

	void destroy();

	NetworkArchitecture* getNetworkArchitecture() { return m_pNetworkArchitecture; }
	const LinkConnection* getOutput() const { return m_pOutput; }
	const OptimizerSettings* getOptimizerSettings() const { return m_pOptimizerSetting; }

	void setStateInputLayer(wstring name);
	void setActionInputLayer(wstring name);
	wstring getStateInputLayer();
	wstring getActionInputLayer();

	void setOutputLayer(wstring name);
	wstring getOutputLayer();

	static NetworkDefinition* getInstance(tinyxml2::XMLElement* pNode);

	static NetworkDefinition* loadFromFile(std::string fileName);
	static NetworkDefinition* loadFromString(std::string content);

	void addInputStateVar(size_t stateVarId);
	const vector<size_t>& getInputStateVarIds();

	void addInputActionVar(size_t stateVarId);
	const vector<size_t>& getInputActionVarIds();

	size_t getInputSize();

	void setOutputActionVector(size_t actionVarId, size_t numOutputs, double minvalue, double maxvalue);
	size_t getClosestOutputIndex(double value);
	double getActionIndexOutput(size_t actionIndex);
	size_t getOutputActionVar();
	size_t getOutputSize();

	virtual IMinibatch* createMinibatch(size_t size);

	INetwork* createNetwork(double learningRate);
};

