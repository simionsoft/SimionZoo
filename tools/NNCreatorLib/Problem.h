#pragma once
#include "../../3rd-party/tinyxml2/tinyxml2.h"
#include <vector>

class CNetworkArchitecture;
class CInputData;
class CNetwork;
class CLinkConnection;
class COptimizerSetting;

class CProblem
{
protected:
	std::vector<CInputData*> m_inputs;
	CNetworkArchitecture* m_pNetworkArchitecture;
	CLinkConnection* m_pOutput;
	COptimizerSetting* m_pOptimizerSetting;
	
	
public:
	CProblem();
	~CProblem();
	CProblem(tinyxml2::XMLElement* pNode);

	CNetworkArchitecture* getNetworkArchitecture() { return m_pNetworkArchitecture; }
	const std::vector<CInputData*>& getInputs() const { return m_inputs; }
	const CLinkConnection* getOutput() const { return m_pOutput; }
	const COptimizerSetting* getOptimizerSetting() const { return m_pOptimizerSetting; }

	static CProblem* getInstance(tinyxml2::XMLElement* pNode);

	static CProblem* loadFromFile(std::string fileName);
	static CProblem* loadFromString(std::string content);

	CNetwork* createNetwork();
};

