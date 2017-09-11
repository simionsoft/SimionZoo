#include "stdafx.h"
#include "CNTKWrapper.h"
#include "Parameter.h"
#include "Link.h"
#include "ParameterValues.h"
#include "Chain.h"
#include "NetworkArchitecture.h"
#include "Problem.h"
#include "InputData.h"
#include "OptimizerSetting.h"

CNTK::FunctionPtr CNTKWrapper::InputLayer(const CLink * pLink, vector<const CLink*> dependencies, CNTK::DeviceDescriptor & device)
{
	//determine the linked input data
	string inputID = pLink->getParameterByName<CInputDataParameter>("Input Data")->getValue();
	auto inputList = pLink->getParentChain()->getParentNetworkArchitecture()->getParentProblem()->getInputs();

	CNTK::FunctionPtr pInput;
	for each (CInputData* pItem in inputList)
		if (!strcmp(pItem->getId().c_str(), inputID.c_str()))
			return pItem->getInputFunctionPtr();

	return nullptr;
}

CNTK::FunctionPtr CNTKWrapper::ActivationLayer(const CLink * pLink, vector<const CLink*> dependencies, CNTK::DeviceDescriptor & device)
{
	auto activationFunction = pLink->getParameterByName<CActivationFunctionParameter>("Activation")->getValue();
	return CNTKWrapper::Internal::applyActivationFunction(dependencies.at(0)->getFunctionPtr(), activationFunction);
}

CNTK::FunctionPtr CNTKWrapper::Convolution1DLayer(const CLink * pLink, vector<const CLink*> dependencies, CNTK::DeviceDescriptor & device)
{
	FunctionPtr input = dependencies.at(0)->getFunctionPtr();

	string name = pLink->getName();

	size_t filters = pLink->getParameterByName<CIntParameter>("Filters")->getValue();
	CIntTuple1D kernelShape = pLink->getParameterByName<CIntTuple1DParameter>("Kernel Size")->getValue();
	CIntTuple1D strideShape = pLink->getParameterByName<CIntTuple1DParameter>("Stride")->getValue();
	ActivationFunction activationFunction = pLink->getParameterByName<CActivationFunctionParameter>("Activation")->getValue();

	size_t kernel = (size_t)kernelShape.getX1();

	size_t stride = (size_t)strideShape.getX1();

	return CNTKWrapper::Internal::Convolution1D(input, filters, kernel, stride, activationFunction, 1.0, device, name);
}

CNTK::FunctionPtr CNTKWrapper::Convolution2DLayer(const CLink * pLink, vector<const CLink*> dependencies, CNTK::DeviceDescriptor & device)
{
	FunctionPtr input = dependencies.at(0)->getFunctionPtr();

	string name = pLink->getName();

	size_t filters = pLink->getParameterByName<CIntParameter>("Filters")->getValue();
	CIntTuple2D kernelShape = pLink->getParameterByName<CIntTuple2DParameter>("Kernel Size")->getValue();
	CIntTuple2D strideShape = pLink->getParameterByName<CIntTuple2DParameter>("Stride")->getValue();
	ActivationFunction activationFunction = pLink->getParameterByName<CActivationFunctionParameter>("Activation")->getValue();

	size_t kernelWidth = (size_t)kernelShape.getX1();
	size_t kernelHeight = (size_t)kernelShape.getX2();

	size_t strideWidth = (size_t)strideShape.getX1();
	size_t strideHeight = (size_t)strideShape.getX2();

	return CNTKWrapper::Internal::Convolution2D(input, filters, kernelWidth, kernelHeight, strideWidth, strideHeight, activationFunction, 1.0, device, name);
}

CNTK::FunctionPtr CNTKWrapper::Convolution3DLayer(const CLink * pLink, vector<const CLink*> dependencies, CNTK::DeviceDescriptor & device)
{
	FunctionPtr input = dependencies.at(0)->getFunctionPtr();

	string name = pLink->getName();

	size_t filters = pLink->getParameterByName<CIntParameter>("Filters")->getValue();
	CIntTuple3D kernelShape = pLink->getParameterByName<CIntTuple3DParameter>("Kernel Size")->getValue();
	CIntTuple3D strideShape = pLink->getParameterByName<CIntTuple3DParameter>("Stride")->getValue();
	ActivationFunction activationFunction = pLink->getParameterByName<CActivationFunctionParameter>("Activation")->getValue();

	size_t kernelWidth = (size_t)kernelShape.getX1();
	size_t kernelHeight = (size_t)kernelShape.getX2();
	size_t kernelDepth = (size_t)kernelShape.getX3();

	size_t strideWidth = (size_t)strideShape.getX1();
	size_t strideHeight = (size_t)strideShape.getX2();
	size_t strideDepth = (size_t)strideShape.getX3();

	return CNTKWrapper::Internal::Convolution3D(input, filters, kernelWidth, kernelHeight, kernelDepth, strideWidth, strideHeight, strideDepth, activationFunction, 1.0, device, name);
}

CNTK::FunctionPtr CNTKWrapper::DenseLayer(const CLink * pLink, vector<const CLink*> dependencies, CNTK::DeviceDescriptor& device)
{
	int output_nodes = pLink->getParameterByName<CIntParameter>("Units")->getValue();
	wstring name = CNTKWrapper::Internal::string2wstring(pLink->getName());
	auto activationFunction = pLink->getParameterByName<CActivationFunctionParameter>("Activation")->getValue();

	//return CNTKWrapper::Internal::FullyConnectedDNNLayer(pInput, output_nodes, device, nonLinearity, name);
	return CNTKWrapper::Internal::FullyConnectedDNNLayer(dependencies.at(0)->getFunctionPtr(), output_nodes, device, activationFunction, name);
}

CNTK::FunctionPtr CNTKWrapper::DropoutLayer(const CLink * pLink, vector<const CLink*> dependencies, CNTK::DeviceDescriptor & device)
{
	return CNTK::FunctionPtr();
}

CNTK::FunctionPtr CNTKWrapper::FlattenLayer(const CLink * pLink, vector<const CLink*> dependencies, CNTK::DeviceDescriptor & device)
{
	wstring name = Internal::string2wstring(pLink->getName());
	NDShape inputShape = dependencies.at(0)->getFunctionPtr()->Output().Shape();

	return Reshape(dependencies.at(0)->getFunctionPtr(), { inputShape.TotalSize() }, name);
}

CNTK::FunctionPtr CNTKWrapper::ReshapeLayer(const CLink * pLink, vector<const CLink*> dependencies, CNTK::DeviceDescriptor & device)
{
	wstring name = Internal::string2wstring(pLink->getName());
	CIntTuple4D shapeTuple = pLink->getParameterByName<CIntTuple4DParameter>("4D Shape")->getValue();
	NDShape shape = {};
	if (shapeTuple.getX1() != 0)
		shape = shape.AppendShape({ (size_t)shapeTuple.getX1() });
	if (shapeTuple.getX2() != 0)
		shape = shape.AppendShape({ (size_t)shapeTuple.getX2() });
	if (shapeTuple.getX3() != 0)
		shape = shape.AppendShape({ (size_t)shapeTuple.getX3() });
	if (shapeTuple.getX4() != 0)
		shape = shape.AppendShape({ (size_t)shapeTuple.getX4() });

	return Reshape(dependencies.at(0)->getFunctionPtr(), shape, name);

}

CNTK::FunctionPtr CNTKWrapper::MergeLayer(const CLink * pLink, vector<const CLink*> dependencies, CNTK::DeviceDescriptor & device)
{
	vector<Variable> inputs = vector<Variable>();
	if (dependencies.size() > 1)
	{
		for each(auto pItem in dependencies)
			inputs.push_back((CNTK::Variable)pItem->getFunctionPtr());

		auto axisIndex = pLink->getParameterByName<CIntParameter>("Axis")->getValue();
		auto axis = CNTK::Axis::Axis(axisIndex);
		return Splice(inputs, axis);
	}

	return dependencies.at(0)->getFunctionPtr();
}

CNTK::TrainerPtr CNTKWrapper::CreateOptimizer(const COptimizerSetting * pOptimizerSetting, CNTK::FunctionPtr& output, CNTK::FunctionPtr& lossFunction)
{
	CNTK::LearnerPtr pLearner;
	switch (pOptimizerSetting->getOptimizerType())
	{
	case OptimizerType::SGD:
		pLearner = CNTK::SGDLearner(output->Parameters(),
			LearningRatePerMinibatchSchedule(pOptimizerSetting->getParameterByKey("Learning rate")->getValue())
		);
		break;

	case OptimizerType::MomentumSGD:
		pLearner = CNTK::MomentumSGDLearner(output->Parameters(),
			LearningRatePerMinibatchSchedule(pOptimizerSetting->getParameterByKey("Learning rate")->getValue()),
			LearningRatePerMinibatchSchedule(pOptimizerSetting->getParameterByKey("Momentum")->getValue())
		);
		break;

	case OptimizerType::Nesterov:
		pLearner = CNTK::NesterovLearner(output->Parameters(),
			LearningRatePerMinibatchSchedule(pOptimizerSetting->getParameterByKey("Learning rate")->getValue()),
			LearningRatePerMinibatchSchedule(pOptimizerSetting->getParameterByKey("Momentum")->getValue())
		); break;

	case OptimizerType::FSAdaGrad:
		pLearner = CNTK::FSAdaGradLearner(output->Parameters(),
			LearningRatePerMinibatchSchedule(pOptimizerSetting->getParameterByKey("Learning rate")->getValue()),
			LearningRatePerMinibatchSchedule(pOptimizerSetting->getParameterByKey("Momentum")->getValue()),
			DefaultUnitGainValue(),
			LearningRatePerMinibatchSchedule(pOptimizerSetting->getParameterByKey("Variance momentum")->getValue())
		); break;

	case OptimizerType::Adam:
		pLearner = CNTK::AdamLearner(output->Parameters(),
			LearningRatePerMinibatchSchedule(pOptimizerSetting->getParameterByKey("Learning rate")->getValue()),
			LearningRatePerMinibatchSchedule(pOptimizerSetting->getParameterByKey("Momentum")->getValue()),
			DefaultUnitGainValue(),
			LearningRatePerMinibatchSchedule(pOptimizerSetting->getParameterByKey("Variance momentum")->getValue()),
			pOptimizerSetting->getParameterByKey("Epsilon")->getValue()
		); break;

	case OptimizerType::AdaGrad:
		pLearner = CNTK::AdaGradLearner(output->Parameters(),
			LearningRatePerMinibatchSchedule(pOptimizerSetting->getParameterByKey("Learning rate")->getValue())
		); break;

	case OptimizerType::RMSProp:
		pLearner = CNTK::RMSPropLearner(output->Parameters(),
			LearningRatePerMinibatchSchedule(pOptimizerSetting->getParameterByKey("Learning rate")->getValue()),
			pOptimizerSetting->getParameterByKey("Gamma")->getValue(),
			pOptimizerSetting->getParameterByKey("Inc")->getValue(),
			pOptimizerSetting->getParameterByKey("Dec")->getValue(),
			pOptimizerSetting->getParameterByKey("Max")->getValue(),
			pOptimizerSetting->getParameterByKey("Min")->getValue()
		); break;

	case OptimizerType::AdaDelta:
		pLearner = CNTK::AdaDeltaLearner(output->Parameters(),
			LearningRatePerMinibatchSchedule(pOptimizerSetting->getParameterByKey("Learning rate")->getValue()),
			pOptimizerSetting->getParameterByKey("Rho")->getValue(),
			pOptimizerSetting->getParameterByKey("Epsilon")->getValue()
		); break;
	}

	return CreateTrainer(output, lossFunction, lossFunction, { pLearner });
}


FunctionPtr CNTKWrapper::Internal::applyActivationFunction(FunctionPtr pInput, ActivationFunction activationFunction)
{
	switch (activationFunction)
	{
	case ActivationFunction::elu:
		return CNTK::ELU(pInput);
	case ActivationFunction::hard_sigmoid:
		//TODO: is this the correct function?
		return CNTK::Hardmax(pInput);
	case ActivationFunction::relu:
		return CNTK::ReLU(pInput);
	case ActivationFunction::selu:
		return CNTK::SELU(pInput);
	case ActivationFunction::sigmoid:
		return CNTK::Sigmoid(pInput);
	case ActivationFunction::softmax:
		throw "not supported at the moment";
		//TODO: solve this problem
		//return std::bind<FunctionPtr(const Variable&, const std::wstring&)>(Softmax, std::placeholders::_1, L"");
	case ActivationFunction::softplus:
		return CNTK::Softplus(pInput);
	case ActivationFunction::softsign:
		throw "not supported at the moment";
	case ActivationFunction::tanh:
		return CNTK::Tanh(pInput);
	case ActivationFunction::linear:
		return pInput;
	}
}