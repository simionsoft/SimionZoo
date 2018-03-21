#include "stdafx.h"

#ifdef _WIN64

#include "CNTKWrapperInternals.h"
#include "Parameter.h"
#include "Link.h"
#include "Chain.h"
#include "NetworkArchitecture.h"
#include "Problem.h"
#include "InputData.h"
#include "OptimizerSetting.h"
#include "Chain.h"
#include "Exceptions.h"
#include "Network.h"


CNTK::FunctionPtr CNTKWrapper::InputLayer(const Link * pLink, vector<const Link*> dependencies, CNTK::DeviceDescriptor & device)
{
	//determine the linked input data
	string inputID = pLink->getParameterByName<InputDataParameter>("Input Data")->getValue();
	auto inputList = pLink->getParentChain()->getParentNetworkArchitecture()->getParentProblem()->getInputs();

	CNTK::FunctionPtr pInput;
	for each (InputData* pItem in inputList)
	{
		if (!strcmp(pItem->getId().c_str(), inputID.c_str()))
		{
			pItem->setIsUsed(true);
			return pItem->getInputVariable();
		}
	}
	return nullptr;
}

CNTK::FunctionPtr CNTKWrapper::ActivationLayer(const Link * pLink, vector<const Link*> dependencies, CNTK::DeviceDescriptor & device)
{
	auto activationFunction = pLink->getParameterByName<ActivationFunctionParameter>("Activation")->getValue();
	return CNTKWrapper::Internal::applyActivationFunction(dependencies.at(0)->getFunctionPtr(), activationFunction);
}

CNTK::FunctionPtr CNTKWrapper::Convolution1DLayer(const Link * pLink, vector<const Link*> dependencies, CNTK::DeviceDescriptor & device)
{

	FunctionPtr input = dependencies.at(0)->getFunctionPtr();

	string name = pLink->getName();

	size_t filters = pLink->getParameterByName<IntParameter>("Filters")->getValue();
	CIntTuple1D kernelShape = pLink->getParameterByName<IntTuple1DParameter>("Kernel Size")->getValue();
	CIntTuple1D strideShape = pLink->getParameterByName<IntTuple1DParameter>("Stride")->getValue();
	ActivationFunction activationFunction = pLink->getParameterByName<ActivationFunctionParameter>("Activation")->getValue();

	size_t kernel = (size_t)kernelShape.getX1();

	size_t stride = (size_t)strideShape.getX1();

	return CNTKWrapper::Internal::Convolution1D(input, filters, kernel, stride, activationFunction, 1.0, device, name);
}

CNTK::FunctionPtr CNTKWrapper::Convolution2DLayer(const Link * pLink, vector<const Link*> dependencies, CNTK::DeviceDescriptor & device)
{

	FunctionPtr input = dependencies.at(0)->getFunctionPtr();

	string name = pLink->getName();

	size_t filters = pLink->getParameterByName<IntParameter>("Filters")->getValue();
	CIntTuple2D kernelShape = pLink->getParameterByName<IntTuple2DParameter>("Kernel Size")->getValue();
	CIntTuple2D strideShape = pLink->getParameterByName<IntTuple2DParameter>("Stride")->getValue();
	ActivationFunction activationFunction = pLink->getParameterByName<ActivationFunctionParameter>("Activation")->getValue();

	size_t kernelWidth = (size_t)kernelShape.getX1();
	size_t kernelHeight = (size_t)kernelShape.getX2();

	size_t strideWidth = (size_t)strideShape.getX1();
	size_t strideHeight = (size_t)strideShape.getX2();

	return CNTKWrapper::Internal::Convolution2D(input, filters, kernelWidth, kernelHeight, strideWidth, strideHeight, activationFunction, 1.0, device, name);
}

CNTK::FunctionPtr CNTKWrapper::Convolution3DLayer(const Link * pLink, vector<const Link*> dependencies, CNTK::DeviceDescriptor & device)
{

	FunctionPtr input = dependencies.at(0)->getFunctionPtr();

	string name = pLink->getName();

	size_t filters = pLink->getParameterByName<IntParameter>("Filters")->getValue();
	CIntTuple3D kernelShape = pLink->getParameterByName<IntTuple3DParameter>("Kernel Size")->getValue();
	CIntTuple3D strideShape = pLink->getParameterByName<IntTuple3DParameter>("Stride")->getValue();
	ActivationFunction activationFunction = pLink->getParameterByName<ActivationFunctionParameter>("Activation")->getValue();

	size_t kernelWidth = (size_t)kernelShape.getX1();
	size_t kernelHeight = (size_t)kernelShape.getX2();
	size_t kernelDepth = (size_t)kernelShape.getX3();

	size_t strideWidth = (size_t)strideShape.getX1();
	size_t strideHeight = (size_t)strideShape.getX2();
	size_t strideDepth = (size_t)strideShape.getX3();

	return CNTKWrapper::Internal::Convolution3D(input, filters, kernelWidth, kernelHeight, kernelDepth, strideWidth, strideHeight, strideDepth, activationFunction, 1.0, device, name);
}

CNTK::FunctionPtr CNTKWrapper::DenseLayer(const Link * pLink, vector<const Link*> dependencies, CNTK::DeviceDescriptor& device)
{

	int output_nodes = pLink->getParameterByName<IntParameter>("Units")->getValue();
	wstring name = CNTKWrapper::Internal::string2wstring(pLink->getName());
	auto activationFunction = pLink->getParameterByName<ActivationFunctionParameter>("Activation")->getValue();

	return CNTKWrapper::Internal::FullyConnectedDNNLayer(dependencies.at(0)->getFunctionPtr(), output_nodes, device, activationFunction, name);
}

CNTK::FunctionPtr CNTKWrapper::DropoutLayer(const Link * pLink, vector<const Link*> dependencies, CNTK::DeviceDescriptor & device)
{

	return CNTK::FunctionPtr();
}

CNTK::FunctionPtr CNTKWrapper::FlattenLayer(const Link * pLink, vector<const Link*> dependencies, CNTK::DeviceDescriptor & device)
{

	wstring name = Internal::string2wstring(pLink->getName());
	NDShape inputShape = dependencies.at(0)->getFunctionPtr()->Output().Shape();

	return Reshape(dependencies.at(0)->getFunctionPtr(), { inputShape.TotalSize() }, name);
}

CNTK::FunctionPtr CNTKWrapper::ReshapeLayer(const Link * pLink, vector<const Link*> dependencies, CNTK::DeviceDescriptor & device)
{

	wstring name = Internal::string2wstring(pLink->getName());
	CIntTuple4D shapeTuple = pLink->getParameterByName<IntTuple4DParameter>("4D Shape")->getValue();
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

CNTK::FunctionPtr CNTKWrapper::MergeLayer(const Link * pLink, vector<const Link*> dependencies, CNTK::DeviceDescriptor & device)
{

	vector<Variable> inputs = vector<Variable>();
	if (dependencies.size() > 1)
	{
		for each(auto pItem in dependencies)
			inputs.push_back((CNTK::Variable)pItem->getFunctionPtr());

		auto axisIndex = pLink->getParameterByName<IntParameter>("Axis")->getValue();
		auto axis = CNTK::Axis::Axis(axisIndex);
		return Splice(inputs, axis);
	}

	return dependencies.at(0)->getFunctionPtr();
}

CNTK::FunctionPtr CNTKWrapper::BatchNormalizationLayer(const Link * pLink, vector<const Link*> dependencies, CNTK::DeviceDescriptor & device)
{

	wstring name = CNTKWrapper::Internal::string2wstring(pLink->getName());

	auto biasParams = Parameter({ NDShape::InferredDimension }, (float)0.0, device);

	auto scaleParams = Parameter({ NDShape::InferredDimension }, (float)0.0, device);

	auto runningMean = Parameter({ NDShape::InferredDimension }, (float)0.0, device);

	auto runningInvStd = Parameter({ NDShape::InferredDimension }, (float)0.0, device);
	
	auto runningCount = Constant::Scalar(0.0f, device);

	//TODO: check if spatial=false and 5000 are good values.
	return BatchNormalization(dependencies[0]->getFunctionPtr(), scaleParams, biasParams, runningMean, runningInvStd, runningCount, false, 5000, 0, 1e-05, true, name);

}


CNTK::FunctionPtr CNTKWrapper::LinearTransformationLayer(const Link * pLink, vector<const Link*> dependencies, CNTK::DeviceDescriptor & device)
{
	wstring name = Internal::string2wstring(pLink->getName());
	double offset = pLink->getParameterByName<DoubleParameter>("Offset")->getValue();
	double scale = pLink->getParameterByName<DoubleParameter>("Scale")->getValue();

	auto timesParam = Constant::Scalar(DataType::Float, (float)scale, device);

	auto timesFunction = ElementTimes(timesParam, dependencies.at(0)->getFunctionPtr());

	auto plusParam = Constant::Constant(dependencies.at(0)->getFunctionPtr()->Output().Shape(), (float)offset, device);
	
	return Plus(plusParam, timesFunction, name);

}

CNTK::TrainerPtr CNTKWrapper::CreateOptimizer(const OptimizerSetting * pOptimizerSetting, CNTK::FunctionPtr& output, CNTK::FunctionPtr& lossFunction)
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
		return CNTK::Softmax(pInput);
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
	return FunctionPtr(nullptr);
}

std::wstring CNTKWrapper::Internal::string2wstring(std::string value)
{
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::wstring wide = converter.from_bytes(value);
	return wide;
}

string CNTKWrapper::Internal::wstring2string(const std::wstring& value)
{
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::string wide = converter.to_bytes(value);

	return wide;
}

CNTK::FunctionPtr CNTKWrapper::Internal::FullyConnectedLinearLayer(CNTK::Variable input, size_t outputDim, const DeviceDescriptor& device, const std::wstring& outputName)
{
	assert(input.Shape().Rank() == 1);
	size_t inputDim = input.Shape()[0];

	auto timesParam = Parameter({ outputDim, inputDim }, DataType::Double, GlorotUniformInitializer(DefaultParamInitScale, SentinelValueForInferParamInitRank, SentinelValueForInferParamInitRank, 1), device, L"timesParam");
	auto timesFunction = Times(timesParam, input, L"times");

	auto plusParam = Parameter({ outputDim }, 0.0, device, L"plusParam");
	return Plus(plusParam, timesFunction, outputName);
}

FunctionPtr CNTKWrapper::Internal::Convolution1D(Variable input, size_t kernelCount, size_t kernel, size_t stride, ActivationFunction activationFunction, double wScale, const DeviceDescriptor& device, const string& outputName)
{
	assert(input.Shape().Rank() == 2);
	size_t numInputChannels = input.Shape()[input.Shape().Rank() - 1];

	auto convParams = Parameter({ kernel, numInputChannels, kernelCount }, DataType::Double, GlorotUniformInitializer(wScale, -1, 2), device);
	return applyActivationFunction(
		Convolution(convParams, input, { stride, numInputChannels }, { true }, { true }, 0Ui64, string2wstring(outputName)),
		activationFunction
	);
}

FunctionPtr CNTKWrapper::Internal::Convolution2D(Variable input, size_t kernelCount, size_t kernelWidth, size_t kernelHeight, size_t strideWidth, size_t strideHeight, ActivationFunction activationFunction, double wScale, const DeviceDescriptor& device, const string& outputName)
{
	assert(input.Shape().Rank() == 3);
	size_t numInputChannels = input.Shape()[input.Shape().Rank() - 1];

	auto convParams = Parameter({ kernelWidth, kernelHeight, numInputChannels, kernelCount }, DataType::Double, GlorotUniformInitializer(wScale, -1, 2), device);
	return applyActivationFunction(
		Convolution(convParams, input, { strideWidth, strideHeight, numInputChannels }, { true }, { true }, 0Ui64, string2wstring(outputName)),
		activationFunction
	);
}

FunctionPtr CNTKWrapper::Internal::Convolution3D(Variable input, size_t kernelCount, size_t kernelWidth, size_t kernelHeight, size_t kernelDepth, size_t strideWidth, size_t strideHeight, size_t strideDepth, ActivationFunction activationFunction, double wScale, const DeviceDescriptor& device, const string& outputName)
{
	assert(input.Shape().Rank() == 4);
	size_t numInputChannels = input.Shape()[input.Shape().Rank() - 1];

	auto convParams = Parameter({ kernelWidth, kernelHeight, kernelDepth, numInputChannels, kernelCount }, DataType::Double, GlorotUniformInitializer(wScale, -1, 2), device);
	return applyActivationFunction(
		Convolution(convParams, input, { strideWidth, strideHeight, strideDepth, numInputChannels }, { true }, { true }, 0Ui64, string2wstring(outputName)),
		activationFunction
	);
}

FunctionPtr CNTKWrapper::Internal::FullyConnectedDNNLayer(Variable input, size_t outputDim, const DeviceDescriptor& device, const std::function<FunctionPtr(const FunctionPtr&)>& nonLinearity, const std::wstring& outputName)
{
	return nonLinearity(FullyConnectedLinearLayer(input, outputDim, device, outputName));
}

FunctionPtr CNTKWrapper::Internal::FullyConnectedDNNLayer(Variable input, size_t outputDim, const DeviceDescriptor& device, ActivationFunction activationFunction, const std::wstring& outputName)
{
	return applyActivationFunction(FullyConnectedLinearLayer(input, outputDim, device, outputName), activationFunction);
}
#endif