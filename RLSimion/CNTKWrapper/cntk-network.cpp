#include "cntk-network.h"
#include "../Lib/deep-functions.h"
#include "CNTKLibrary.h"
#include "CNTKWrapperInternals.h"
#include "../../tools/System/CrossPlatform.h"
#include <vector>

// CntkNetwork

unsigned int CntkNetwork::getNumOutputs()
{
	return m_pDeepFunction->getNumOutputs();
}

const vector<string>& CntkNetwork::getInputStateVariables()
{
	return m_pDeepFunction->getInputStateVariables();
}

const vector<string>& CntkNetwork::getInputActionVariables()
{
	return m_pDeepFunction->getInputActionVariables();
}

CNTK::FunctionPtr CntkNetwork::activationFunction(CNTK::FunctionPtr input, Activation activationFunction)
{
	switch (activationFunction)
	{
	case Activation::ELU:
		return CNTK::ELU(input, L"ELU");
	case Activation::ReLU:
		return CNTK::ReLU(input, L"ReLu");
	case Activation::Sigmoid:
		return CNTK::Sigmoid(input, L"Sigmoid");
	case Activation::SoftMax:
		return CNTK::Softmax(input, L"Softmax");
	case Activation::SoftPlus:
		return CNTK::Softplus(input, L"Softplus");
	case Activation::Tanh:
		return CNTK::Tanh(input, L"Tanh");
	case Activation::Linear:
	default:
		return input;
	}
}

CNTK::FunctionPtr CntkNetwork::denseLayer(CNTK::FunctionPtr layerInput, size_t outputDim)
{
	size_t inputDim = layerInput->Output().Shape()[0];

	auto timesParam = Parameter({ outputDim, inputDim }
		, DataType::Double, GlorotUniformInitializer(DefaultParamInitScale
			, SentinelValueForInferParamInitRank, SentinelValueForInferParamInitRank, 1));
	auto timesFunction = Times(timesParam, layerInput, L"times");

	auto plusParam = Parameter({ outputDim }, 0.0);
	return Plus(plusParam, timesFunction);
}

CNTK::FunctionPtr CntkNetwork::initNetworkLayers(CNTK::FunctionPtr networkInput, string layersDefinition)
{
	CNTK::FunctionPtr output = networkInput;
	vector<string> layerDefinitions = CrossPlatform::split(layersDefinition, DeepNetworkDefinition::layerDefinitionDelimiter);
	for (int i = 0; i < layerDefinitions.size(); i++)
	{
		vector<string> layerParameters = CrossPlatform::split(layerDefinitions[i], DeepNetworkDefinition::layerParameterDelimiter);
		if (layerParameters.size() == 2) //for now, only 2 parameters: activation and #units
		{
			Activation activation = DeepLayer::activationFromFunctionName(layerParameters[0].c_str());
			int numUnits = std::atoi(layerParameters[1].c_str());
			output = activationFunction(denseLayer(output, numUnits), activation);
		}
	}
	return output;
}

CNTK::FunctionPtr CntkNetwork::mergeLayer(CNTK::Variable var1, CNTK::Variable var2)
{
	return Splice({ var1, var2 },CNTK::Axis(0));
}

CNTK::TrainerPtr CntkNetwork::trainer(CNTK::FunctionPtr networkOutput, CNTK::FunctionPtr lossFunction, CNTK::FunctionPtr evalFunction, string learnerDefinition)
{
	LearnerPtr learner;
	vector<string> learnerParameters = CrossPlatform::split(learnerDefinition, DeepNetworkDefinition::learnerParameterDelimiter);
	if (learnerParameters.size() >= 1) //at least, the algorithm used to train the network
	{
		//learning rates must be set before training the net, so no need it
		if (learnerParameters[0] == "SGD")
			learner = CNTK::SGDLearner(networkOutput->Parameters()
				, CNTK::TrainingParameterPerSampleSchedule(0.0001));
		else if (learnerParameters[0] == "MomentumSGD")
			learner = CNTK::MomentumSGDLearner(networkOutput->Parameters()
				, CNTK::TrainingParameterPerSampleSchedule(0.0001)
				, CNTK::TrainingParameterPerSampleSchedule(0.9));
		else //"Adam"
			learner = CNTK::AdamLearner(networkOutput->Parameters()
				, CNTK::TrainingParameterPerSampleSchedule(0.0001)
				, CNTK::TrainingParameterPerSampleSchedule(0.9));

		return CreateTrainer(networkOutput, lossFunction, evalFunction, { learner });
	}
}

CNTK::FunctionPtr CntkNetwork::initNetworkLearner(CNTK::FunctionPtr networkOutput, string learnerDefinition)
{
	m_targetVariable = CNTK::InputVariable({ m_pDeepFunction->getNumOutputs() }
										, CNTK::DataType::Double, m_targetVariableId);
	CNTK::FunctionPtr lossFunctionPtr = CNTK::SquaredError(m_networkOutput, m_targetVariable, m_lossVariableId);
	CNTK::FunctionPtr evalFunctionPtr = CNTK::SquaredError(m_networkOutput, m_targetVariable, m_lossVariableId);
	m_fullNetworkLearnerFunction = CNTK::Combine({ lossFunctionPtr, m_networkOutput }, m_fullNetworLearnerFunctionId);

	//Create the trainer for the network
	m_trainer = trainer(m_networkOutput, lossFunctionPtr, evalFunctionPtr, learnerDefinition);
}

CNTK::FunctionPtr CntkNetwork::initNetworkFromInputLayer(CNTK::FunctionPtr inputLayer)
{
	CNTK::FunctionPtr lastLayer = initNetworkLayers(m_inputState, m_pDeepFunction->getLayersDefinition());
	m_networkOutput = denseLayer(lastLayer, getNumOutputs());
	m_fullNetworkLearnerFunction = initNetworkLearner(m_networkOutput, m_pDeepFunction->getLearnerDefinition());
}

// CntkDiscreteQFunctionNetwork

CntkDiscreteQFunctionNetwork::CntkDiscreteQFunctionNetwork(DeepNetworkDefinition* pNetworkDefinition)
	: CntkNetwork(pNetworkDefinition)
{
	m_inputState = CNTK::InputVariable({ pNetworkDefinition->getInputStateVariables().size() }
		, CNTK::DataType::Double, false, m_stateInputVariableId);
	m_fullNetworkLearnerFunction = initNetworkFromInputLayer(m_inputState);
}
void CntkDiscreteQFunctionNetwork::destroy() { delete this;}
IDeepNetwork* CntkDiscreteQFunctionNetwork::clone(bool bFreezeWeights) { return nullptr; }

// CntkContinuousQFunctionNetwork

CntkContinuousQFunctionNetwork::CntkContinuousQFunctionNetwork(DeepNetworkDefinition* pNetworkDefinition)
	: CntkNetwork(pNetworkDefinition)
{
	m_inputState = CNTK::InputVariable({ pNetworkDefinition->getInputStateVariables().size() }
		, CNTK::DataType::Double, false, m_stateInputVariableId);
	m_inputAction = CNTK::InputVariable({ pNetworkDefinition->getInputActionVariables().size() }
		, CNTK::DataType::Double, false, m_actionInputVariableId);
	CNTK::FunctionPtr inputMergeLayer = mergeLayer(m_inputState, m_inputAction);

	m_fullNetworkLearnerFunction = initNetworkFromInputLayer(m_inputState);
}

void CntkContinuousQFunctionNetwork::destroy() { delete this; }
IDeepNetwork* CntkContinuousQFunctionNetwork::clone(bool bFreezeWeights) { return nullptr; }

// CntkVFunctionNetwork

CntkVFunctionNetwork::CntkVFunctionNetwork(DeepNetworkDefinition* pNetworkDefinition)
	: CntkNetwork(pNetworkDefinition)
{
	m_inputState = CNTK::InputVariable({ pNetworkDefinition->getInputStateVariables().size() }
		, CNTK::DataType::Double, false, m_stateInputVariableId);
	m_fullNetworkLearnerFunction = initNetworkFromInputLayer(m_inputState);
}

void CntkVFunctionNetwork::destroy() { delete this; }
IDeepNetwork* CntkVFunctionNetwork::clone(bool bFreezeWeightse) { return nullptr; }

// CntkDeterministicPolicyNetwork

CntkDeterministicPolicyNetwork::CntkDeterministicPolicyNetwork(DeepNetworkDefinition* pNetworkDefinition)
	: CntkNetwork(pNetworkDefinition)
{
	m_inputState = CNTK::InputVariable({ pNetworkDefinition->getInputStateVariables().size() }
		, CNTK::DataType::Double, false, m_stateInputVariableId);
	m_fullNetworkLearnerFunction = initNetworkFromInputLayer(m_inputState);
}
void CntkDeterministicPolicyNetwork::destroy() { delete this; }
IDeepNetwork* CntkDeterministicPolicyNetwork::clone(bool bFreezeWeights) { return nullptr; }