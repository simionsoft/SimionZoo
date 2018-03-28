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
protected:
	vector<unsigned int> m_inputStateVars;
	unsigned int m_outputActionVar;
	vector<double> m_outputActionValues;

	vector<InputData*> m_inputs;
	NetworkArchitecture* m_pNetworkArchitecture;
	LinkConnection* m_pOutput;
	OptimizerSettings* m_pOptimizerSetting;
	
public:
	NetworkDefinition();
	~NetworkDefinition();
	NetworkDefinition(tinyxml2::XMLElement* pNode);

	void destroy();

	NetworkArchitecture* getNetworkArchitecture() { return m_pNetworkArchitecture; }
	const std::vector<InputData*>& getInputs() const { return m_inputs; }
	const LinkConnection* getOutput() const { return m_pOutput; }
	const OptimizerSettings* getOptimizerSettings() const { return m_pOptimizerSetting; }

	static NetworkDefinition* getInstance(tinyxml2::XMLElement* pNode);

	static NetworkDefinition* loadFromFile(std::string fileName);
	static NetworkDefinition* loadFromString(std::string content);

	void addInputStateVar(unsigned int stateVarId);
	size_t getNumInputStateVars();
	size_t getInputStateVar(size_t i);
	void setOutputAction(Action* a, unsigned int actionVarId, unsigned int numOutputs);
	size_t getClosestOutputIndex(double value);
	size_t getOutputActionVar();
	size_t getNumOutputs();

	virtual IMinibatch* createMinibatch(size_t size);

	INetwork* createNetwork();
};

