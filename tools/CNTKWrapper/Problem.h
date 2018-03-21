#pragma once
#include "CNTKWrapper.h"
#include "../../3rd-party/tinyxml2/tinyxml2.h"
#include <vector>

class NetworkArchitecture;
class InputData;
class Network;
class LinkConnection;
class OptimizerSetting;

class Problem: public IProblem
{
protected:
	std::vector<InputData*> m_inputs;
	NetworkArchitecture* m_pNetworkArchitecture;
	LinkConnection* m_pOutput;
	OptimizerSetting* m_pOptimizerSetting;
	
	
public:
	Problem();
	~Problem();
	Problem(tinyxml2::XMLElement* pNode);

	void destroy();

	NetworkArchitecture* getNetworkArchitecture() { return m_pNetworkArchitecture; }
	const std::vector<InputData*>& getInputs() const { return m_inputs; }
	const LinkConnection* getOutput() const { return m_pOutput; }
	const OptimizerSetting* getOptimizerSetting() const { return m_pOptimizerSetting; }

	static Problem* getInstance(tinyxml2::XMLElement* pNode);

	static Problem* loadFromFile(std::string fileName);
	static Problem* loadFromString(std::string content);

	INetwork* createNetwork();
};

