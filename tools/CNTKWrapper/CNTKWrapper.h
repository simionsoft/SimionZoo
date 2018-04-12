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

using namespace std;
namespace tinyxml2 { class XMLElement; }

#include "../../RLSimion/named-var-set.h"
class NetworkArchitecture;
class INetwork;
class IMinibatch;

//Network definition
class INetworkDefinition
{
public:
	virtual void destroy() = 0;

	virtual void addInputStateVar(size_t stateVarId)= 0;
	virtual const vector<size_t>& getInputStateVarIds()= 0;

	virtual void addInputActionVar(size_t stateVarId)= 0;
	virtual const vector<size_t>& getInputActionVarIds()= 0;

	virtual size_t getInputSize() = 0;

	virtual void setOutputActionVector(size_t actionVarId, size_t numOutputs, double minvalue, double maxvalue) = 0;
	virtual size_t getClosestOutputIndex(double value)=  0;
	virtual double getActionIndexOutput(size_t actionIndex) = 0;

	virtual IMinibatch* createMinibatch(size_t size) = 0;

	//for convenience we override the learning rate set in the network definition's parameters
	virtual INetwork* createNetwork(double learningRate) = 0;
};

class INetwork
{
public:

	virtual void destroy()= 0;

	virtual void buildNetwork(double learningRate)= 0;

	virtual void save(string fileName)= 0;

	virtual INetwork* getFrozenCopy() const= 0;

	virtual void train(IMinibatch* pMinibatch) = 0;
	virtual void get(const State* s, const Action* a, vector<double>& outputValues) = 0;
};

class IMinibatch
{
public:
	virtual void destroy() = 0;

	virtual void clear() = 0;
	virtual void addTuple(const State* s, const Action* a, vector<double>& targetValues) = 0;
	virtual vector<double>& getInputState() = 0;
	virtual vector<double>& getInputAction() = 0;
	virtual vector<double>& getOutput() = 0;
	virtual bool isFull() = 0;
};

namespace CNTKWrapper
{
	DLL_API INetworkDefinition* getProblemInstance(tinyxml2::XMLElement* pNode);
}

#endif