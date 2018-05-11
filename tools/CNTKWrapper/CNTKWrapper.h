#pragma once
#ifdef _WIN64

#ifdef BUILD_CNTK_WRAPPER
//we are building the CNTK Wrapper
#define DLL_API __declspec(dllexport)
#else
#define DLL_API
#endif

#include <vector>

using namespace std;
namespace tinyxml2 { class XMLElement; }

#include "../../RLSimion/named-var-set.h"
#include "../../RLSimion/state-action-function.h"
class NetworkArchitecture;
class INetwork;
class IMinibatch;

//Network definition
class INetworkDefinition
{
public:
	virtual void destroy() = 0;

	virtual void addInputStateVar(string name, size_t id)= 0;
	virtual const vector<size_t>& getInputStateVarIds()= 0;

	virtual void addInputActionVar(string name, size_t id)= 0;
	virtual const vector<size_t>& getInputActionVarIds()= 0;

	virtual void setScalarOutput() = 0;
	virtual void setVectorOutput(size_t dimension) = 0;
	virtual void setDiscretizedActionVectorOutput(size_t actionVarId, size_t numOutputs, double minvalue, double maxvalue) = 0;
	//These can only be used after calling setDiscretizedActionVectorOutput()
	virtual size_t getClosestOutputIndex(double value)=  0;
	virtual double getActionIndexOutput(size_t actionIndex) = 0;

	virtual IMinibatch* createMinibatch(size_t size, size_t outputSize= 0) = 0;

	//for convenience we override the learning rate set in the network definition's parameters
	virtual INetwork* createNetwork(double learningRate, bool inputsNeedGradient = false) = 0;


	//returns the name of the device used by CNTK: either CPU or GPU
	virtual string getDeviceName() = 0;
};

class INetwork : public StateActionFunction
{
public:
	virtual void destroy()= 0;

	virtual void buildNetwork(double learningRate)= 0;

	virtual void save(string fileName)= 0;

	virtual INetwork* clone(bool bFreezeWeights= true) const= 0;

	virtual void initSoftUpdate(double u, INetwork* pTargetNetwork) = 0;
	virtual void softUpdate(INetwork* pTargetNetwork) = 0;

	virtual void train(IMinibatch* pMinibatch) = 0;

	virtual void gradientWrtAction(const State* s, const Action* a, vector<double>& outputValues) = 0;
	virtual void applyGradient(IMinibatch* pMinibatch) = 0;

	//StateActionFunction interface
	virtual unsigned int getNumOutputs() = 0;
	virtual vector<double>& evaluate(const State* s, const Action* a) = 0;
	virtual const vector<string>& getInputStateVariables() = 0;
	virtual const vector<string>& getInputActionVariables() = 0;
};

class IMinibatch
{
public:
	virtual void destroy() = 0;

	virtual void clear() = 0;
	virtual void addTuple(const State* s, const Action* a, const vector<double>& targetValues) = 0;
	virtual void addTuple(const State* s, const Action* a, double targetValue) = 0;
	virtual vector<double>& getInputState() = 0;
	virtual vector<double>& getInputAction() = 0;
	virtual vector<double>& getOutput() = 0;
	virtual bool isFull() const = 0;
	virtual size_t size() const = 0;
	virtual size_t outputSize() const = 0;
};

namespace CNTKWrapper
{
	DLL_API INetworkDefinition* getProblemInstance(tinyxml2::XMLElement* pNode);
}

#endif