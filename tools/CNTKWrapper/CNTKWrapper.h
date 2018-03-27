#pragma once
#ifdef _WIN64

#ifdef BUILD_CNTK_WRAPPER
//we are building the CNTK Wrapper
#define DLL_API __declspec(dllexport)
#else
#define DLL_API
//#define CNTK_HEADERONLY_DEFINITIONS
#endif

#include <vector>
#include <unordered_map>
#include <memory>

using namespace std;
namespace tinyxml2 { class XMLElement; }
namespace CNTK
{ 
	class Function;
	typedef std::shared_ptr<Function> FunctionPtr;

	class Trainer;
	typedef std::shared_ptr<Trainer> TrainerPtr;

	class Value;
	typedef std::shared_ptr<Value> ValuePtr;

	class Variable;
	class NDShape;
}
class NetworkArchitecture;
class InputData;
class LinkConnection;
class OptimizerSettings;
class INetwork;

//Interface class
class INetworkDefinition
{
public:
	virtual void destroy() = 0;

	virtual NetworkArchitecture* getNetworkArchitecture() = 0;
	virtual const std::vector<InputData*>& getInputs() const = 0;
	virtual const LinkConnection* getOutput() const = 0;
	virtual const OptimizerSettings* getOptimizerSettings() const = 0;

	virtual INetwork* createNetwork() = 0;
};

class INetwork
{
public:

	virtual void destroy()= 0;

	virtual size_t getTotalSize() = 0;

	virtual vector<CNTK::FunctionPtr>& getFunctionPtrs() = 0;
	virtual vector<InputData*>& getInputs()= 0;
	virtual vector<CNTK::FunctionPtr>& getOutputsFunctionPtr()= 0;

	virtual void buildQNetwork()= 0;
	virtual void save(string fileName)= 0;

	virtual INetwork* clone() const= 0;

	virtual void train(std::unordered_map<std::string, std::vector<double>&>& inputDataMap
		, std::vector<double>& targetOutputData)= 0;
	virtual void predict(std::unordered_map<std::string, std::vector<double>&>& inputDataMap
		, std::vector<double>& predictionData)= 0;

	virtual void setParent(INetworkDefinition* pParent) = 0;
};


namespace CNTKWrapper
{
	DLL_API INetworkDefinition* getProblemInstance(tinyxml2::XMLElement* pNode);
}

#endif