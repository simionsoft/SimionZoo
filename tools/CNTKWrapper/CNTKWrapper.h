#pragma once
#ifdef _WIN64

#ifdef BUILD_CNTK_WRAPPER
//we are building the CNTK Wrapper
#define DLL_API __declspec(dllexport)
#else
#define DLL_API
//#define CNTK_HEADERONLY_DEFINITIONS
#endif

namespace tinyxml2 { class XMLElement; }
class CNetworkArchitecture;
class CInputData;
class CLinkConnection;
class COptimizerSetting;

//Interface class
class IProblem
{
public:
	//virtual IProblem(tinyxml2::XMLElement* pNode)= 0;
	virtual void destroy() = 0;

	virtual CNetworkArchitecture* getNetworkArchitecture() = 0;
	virtual const std::vector<CInputData*>& getInputs() const = 0;
	virtual const CLinkConnection* getOutput() const = 0;
	virtual const COptimizerSetting* getOptimizerSetting() const = 0;

	//static CProblem* getInstance(tinyxml2::XMLElement* pNode);

	//static CProblem* loadFromFile(std::string fileName);
	//static CProblem* loadFromString(std::string content);

	virtual CNetwork* createNetwork() = 0;
};


namespace CNTKWrapper
{
	DLL_API IProblem* getProblemInstance(tinyxml2::XMLElement* pNode);
}

#endif