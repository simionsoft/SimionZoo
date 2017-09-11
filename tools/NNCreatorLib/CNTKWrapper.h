#pragma once
#include "CNTKLibrary.h"

//#include "ParameterValues.h"
#include <locale>
#include <codecvt>
#include <string>

#include "Link.h"
//class CLink;
//enum class LinkType;
enum class ActivationFunction;

using namespace CNTK;

class COptimizerSetting;


namespace CNTKWrapper
{
	CNTK::FunctionPtr InputLayer(const CLink* pLink, vector<const CLink*> dependencies, CNTK::DeviceDescriptor& device);
	CNTK::FunctionPtr ActivationLayer(const CLink* pLink, vector<const CLink*> dependencies, CNTK::DeviceDescriptor& device);
	CNTK::FunctionPtr Convolution1DLayer(const CLink* pLink, vector<const CLink*> dependencies, CNTK::DeviceDescriptor& device);
	CNTK::FunctionPtr Convolution2DLayer(const CLink* pLink, vector<const CLink*> dependencies, CNTK::DeviceDescriptor& device);
	CNTK::FunctionPtr Convolution3DLayer(const CLink* pLink, vector<const CLink*> dependencies, CNTK::DeviceDescriptor& device);
	CNTK::FunctionPtr DenseLayer(const CLink* pLink, vector<const CLink*> dependencies, CNTK::DeviceDescriptor& device);
	CNTK::FunctionPtr DropoutLayer(const CLink* pLink, vector<const CLink*> dependencies, CNTK::DeviceDescriptor& device);
	CNTK::FunctionPtr FlattenLayer(const CLink* pLink, vector<const CLink*> dependencies, CNTK::DeviceDescriptor& device);
	CNTK::FunctionPtr ReshapeLayer(const CLink* pLink, vector<const CLink*> dependencies, CNTK::DeviceDescriptor& device);
	CNTK::FunctionPtr MergeLayer(const CLink* pLink, vector<const CLink*> dependencies, CNTK::DeviceDescriptor& device);

	CNTK::TrainerPtr CreateOptimizer(const COptimizerSetting* pOptimizerSetting, CNTK::FunctionPtr& model, CNTK::FunctionPtr& lossFunction);

	namespace Internal
	{
		inline std::wstring string2wstring(std::string value)
		{
			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
			std::wstring wide = converter.from_bytes(value);
			return wide;
		}

		inline FunctionPtr applyActivationFunction(FunctionPtr pInput, ActivationFunction activationFunction);

		inline FunctionPtr FullyConnectedLinearLayer(Variable input, size_t outputDim, const DeviceDescriptor& device, const std::wstring& outputName = L"")
		{
			assert(input.Shape().Rank() == 1);
			size_t inputDim = input.Shape()[0];

			auto timesParam = Parameter({ outputDim, inputDim }, DataType::Float, GlorotUniformInitializer(DefaultParamInitScale, SentinelValueForInferParamInitRank, SentinelValueForInferParamInitRank, 1), device, L"timesParam");
			auto timesFunction = Times(timesParam, input, L"times");

			auto plusParam = Parameter({ outputDim }, 0.0f, device, L"plusParam");
			return Plus(plusParam, timesFunction, outputName);
		}

		inline FunctionPtr Convolution1D(Variable input, size_t kernelCount, size_t kernel, size_t stride, ActivationFunction activationFunction, double wScale, const DeviceDescriptor& device, const string& outputName = "")
		{
			assert(input.Shape().Rank() == 2);
			size_t numInputChannels = input.Shape()[input.Shape().Rank() - 1];

			auto convParams = Parameter({ kernel, numInputChannels, kernelCount }, DataType::Float, GlorotUniformInitializer(wScale, -1, 2), device);
			return  applyActivationFunction(
				Convolution(convParams, input, { stride, numInputChannels }, { true }, { true }, 0Ui64, string2wstring(outputName)),
				activationFunction
			);
		}

		inline FunctionPtr Convolution2D(Variable input, size_t kernelCount, size_t kernelWidth, size_t kernelHeight, size_t strideWidth, size_t strideHeight, ActivationFunction activationFunction, double wScale, const DeviceDescriptor& device, const string& outputName = "")
		{
			assert(input.Shape().Rank() == 3);
			size_t numInputChannels = input.Shape()[input.Shape().Rank() - 1];

			auto convParams = Parameter({ kernelWidth, kernelHeight, numInputChannels, kernelCount }, DataType::Float, GlorotUniformInitializer(wScale, -1, 2), device);
			return applyActivationFunction(
				Convolution(convParams, input, { strideWidth, strideHeight, numInputChannels }, { true }, { true }, 0Ui64, string2wstring(outputName)),
				activationFunction
			);
		}

		inline FunctionPtr Convolution3D(Variable input, size_t kernelCount, size_t kernelWidth, size_t kernelHeight, size_t kernelDepth, size_t strideWidth, size_t strideHeight, size_t strideDepth, ActivationFunction activationFunction, double wScale, const DeviceDescriptor& device, const string& outputName = "")
		{
			assert(input.Shape().Rank() == 4);
			size_t numInputChannels = input.Shape()[input.Shape().Rank() - 1];

			auto convParams = Parameter({ kernelWidth, kernelHeight, kernelDepth, numInputChannels, kernelCount }, DataType::Float, GlorotUniformInitializer(wScale, -1, 2), device);
			return applyActivationFunction(
				Convolution(convParams, input, { strideWidth, strideHeight, strideDepth, numInputChannels }, { true }, { true }, 0Ui64, string2wstring(outputName)),
				activationFunction
			);
		}

		inline FunctionPtr FullyConnectedDNNLayer(Variable input, size_t outputDim, const DeviceDescriptor& device, const std::function<FunctionPtr(const FunctionPtr&)>& nonLinearity, const std::wstring& outputName = L"")
		{
			return nonLinearity(FullyConnectedLinearLayer(input, outputDim, device, outputName));
		}

		inline FunctionPtr FullyConnectedDNNLayer(Variable input, size_t outputDim, const DeviceDescriptor& device, ActivationFunction activationFunction, const std::wstring& outputName = L"")
		{
			return applyActivationFunction(FullyConnectedLinearLayer(input, outputDim, device, outputName), activationFunction);
		}
	}
}