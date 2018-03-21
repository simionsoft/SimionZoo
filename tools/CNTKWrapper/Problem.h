#pragma once
#include "CNTKWrapper.h"
#include "../../3rd-party/tinyxml2/tinyxml2.h"
#include <vector>

class NetworkArchitecture;
class InputData;
class Network;
class LinkConnection;
class OptimizerSetting;

class CProblem: public IProblem
{
protected:
	std::vector<InputData*> m_inputs;
	NetworkArchitecture* m_pNetworkArchitecture;
	LinkConnection* m_pOutput;
	OptimizerSetting* m_pOptimizerSetting;
	
	
public:
	CProblem();
	~CProblem();
	CProblem(tinyxml2::XMLElement* pNode);

	void destroy();

	NetworkArchitecture* getNetworkArchitecture() { return m_pNetworkArchitecture; }
	const std::vector<InputData*>& getInputs() const { return m_inputs; }
	const LinkConnection* getOutput() const { return m_pOutput; }
	const OptimizerSetting* getOptimizerSetting() const { return m_pOptimizerSetting; }

	static CProblem* getInstance(tinyxml2::XMLElement* pNode);

	static CProblem* loadFromFile(std::string fileName);
	static CProblem* loadFromString(std::string content);

	INetwork* createNetwork();
};

