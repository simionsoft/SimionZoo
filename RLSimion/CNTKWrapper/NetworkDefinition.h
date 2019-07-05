#pragma once

#include "CNTKWrapper.h"
#include "../../3rd-party/tinyxml2/tinyxml2.h"
#include "../Common/named-var-set.h"
#include <vector>

class NetworkArchitecture;
class InputData;
class Network;
class LinkConnection;
class OptimizerSettings;

enum OutputType {Unset, Scalar, Vector, DiscretizedActionVector};

class NetworkDefinition: public INetworkDefinition
{
	bool m_bInputsNeedGradient = false;

	wstring m_stateInputLayer;
	wstring m_actionInputLayer;
	wstring m_outputLayerName;
protected:
	vector<string> m_inputStateVariables;
	vector<string> m_inputActionVariables;

	OutputType m_outputType = Unset;
	size_t m_outputSize;

	//used only if m_outputType= DiscretizedActionVector
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
	const LinkConnection* output() const { return m_pOutput; }
	const OptimizerSettings* getOptimizerSettings() const { return m_pOptimizerSetting; }

	void setStateInputLayer(wstring name);
	void setActionInputLayer(wstring name);
	wstring getStateInputLayer();
	wstring getActionInputLayer();

	void setOutputLayer(wstring name);
	wstring getOutputLayer();
	bool inputsNeedGradient() { return m_bInputsNeedGradient; }

	static NetworkDefinition* getInstance(tinyxml2::XMLElement* pNode);

	static NetworkDefinition* loadFromFile(std::string fileName);
	static NetworkDefinition* loadFromString(std::string content);

	void addInputStateVar(string name);
	const vector<string>& getInputStateVariables();

	void addInputActionVar(string name);
	const vector<string>& getInputActionVariables();

	void inputStateVariablesToVector(const State* s, vector<double>& outState, size_t offset);
	void inputActionVariablesToVector(const Action* a, vector<double>& outAction, size_t offset);

	size_t getInputSize();

	void setDiscretizedActionVectorOutput(size_t numSteps);

	size_t getOutputSize();
	OutputType getOutputType() { return m_outputType; }
	void setScalarOutput();
	void setVectorOutput(size_t dimension);

	IMinibatch* createMinibatch(size_t numSamples, size_t outputSize, size_t numActionVariables);

	INetwork* createNetwork(double learningRate, bool inputsNeedGradient);

	string getDeviceName();
};
