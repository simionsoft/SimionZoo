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
	vector<wstring> m_inputLayerNames;
	wstring m_outputLayerName;
protected:
	vector<size_t> m_inputStateVars;
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

	void setInputLayerName(wstring name);
	bool isInputLayer(wstring name);
	void setOutputLayerName(wstring name);
	wstring getOutputLayerName();

	static NetworkDefinition* getInstance(tinyxml2::XMLElement* pNode);

	static NetworkDefinition* loadFromFile(std::string fileName);
	static NetworkDefinition* loadFromString(std::string content);

	void addInputStateVar(size_t stateVarId);
	size_t getNumInputStateVars();
	size_t getInputStateVar(size_t i);
	void setOutputAction(size_t actionVarId, size_t numOutputs, double minvalue, double maxvalue);
	size_t getClosestOutputIndex(double value);
	double getActionIndexOutput(size_t actionIndex);
	size_t getOutputActionVar();
	size_t getNumOutputs();

	virtual IMinibatch* createMinibatch(size_t size);

	INetwork* createNetwork();
};

