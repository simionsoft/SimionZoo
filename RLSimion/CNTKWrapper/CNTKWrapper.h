#pragma once


#ifdef BUILD_CNTK_WRAPPER
	//we are building the CNTK Wrapper
	#ifdef __linux__
		#define DLL_API
	#else
		#define DLL_API __declspec(dllexport)
	#endif
#else
	#define DLL_API
#endif

#include <vector>

using namespace std;
namespace tinyxml2 { class XMLElement; }


#include "../Common/state-action-function.h"

class NetworkArchitecture;
class INetwork;
class IMinibatch;


//Network definition
class INetworkDefinition
{
public:
	virtual void destroy() = 0;

	virtual void addInputStateVar(string name)= 0;
	virtual const vector<string>& getInputStateVariables()= 0;

	virtual void addInputActionVar(string name)= 0;
	virtual const vector<string>& getInputActionVariables()= 0;

	virtual void inputStateVariablesToVector(const State* s, vector<double>& outState, size_t offset = 0) = 0;
	virtual void inputActionVariablesToVector(const Action* a, vector<double>& outAction, size_t offset = 0) = 0;

	virtual void setScalarOutput() = 0;
	virtual void setVectorOutput(size_t dimension) = 0;
	virtual void setDiscretizedActionVectorOutput(size_t numSteps) = 0;

	virtual IMinibatch* createMinibatch(size_t size, size_t outputSize= 0, size_t numActionVariables= 0) = 0;

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

	virtual INetworkDefinition* getDefinition() = 0;

	virtual void save(string fileName)= 0;

	virtual INetwork* clone(bool bFreezeWeights= true) const= 0;

	virtual void initSoftUpdate(double u, INetwork* pTargetNetwork) = 0;
	virtual void softUpdate(INetwork* pTargetNetwork) = 0;

	virtual void train(IMinibatch* pMinibatch, const vector<double>& target) = 0;

	virtual void gradientWrtAction(const vector<double>& s, const vector<double>& a, vector<double>& outputValues) = 0;
	virtual void applyGradient(IMinibatch* pMinibatch, const vector<double>& target) = 0;

	//StateActionFunction interface
	virtual unsigned int getNumOutputs() = 0;
	virtual vector<double>& evaluate(const State* s, const Action* a) = 0;
	virtual void evaluate(const vector<double>& s, const vector<double>& a, vector<double>& output) = 0;
	virtual const vector<string>& getInputStateVariables() = 0;
	virtual const vector<string>& getInputActionVariables() = 0;
};

class IMinibatch
{
public:
	virtual void destroy() = 0;

	virtual void clear() = 0;
	virtual void addTuple(const State* s, const Action* a, const State* s_p, double r) = 0;

	virtual vector<double>& s() = 0;
	virtual vector<double>& a() = 0;
	virtual vector<double>& s_p() = 0;
	virtual vector<double>& r() = 0;

	virtual bool isFull() const = 0;
	virtual size_t size() const = 0;
	virtual size_t outputSize() const = 0;
};

#include "./cntk-network.h"

namespace CNTKWrapper
{
	extern "C" {DLL_API INetworkDefinition* getNetworkDefinition(tinyxml2::XMLElement* pNode); }
	extern "C" {DLL_API void setDevice(bool useGPU); }

	extern "C" {DLL_API IDiscreteQFunctionNetwork* getDiscreteQFunctionNetwork(DeepNetworkDefinition* pNetworkDefinition); }
	extern "C" {DLL_API IContinuousQFunctionNetwork* getContinuousQFunctionNetwork(DeepNetworkDefinition* pNetworkDefinition); }
	extern "C" {DLL_API IVFunctionNetwork* getVFunctionNetwork(DeepNetworkDefinition* pNetworkDefinition); }
	extern "C" {DLL_API IDeterministicPolicyNetwork* getDeterministicPolicyNetwork(DeepNetworkDefinition* pNetworkDefinition); }
}

