#pragma once

#include "CNTKLibrary.h"
#include "ParameterValues.h"
#include <locale>
#include <codecvt>
#include <vector>
#include <string>

using namespace CNTK;
using namespace std;
class CLink;
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
		std::wstring string2wstring(std::string value);

		string wstring2string(const std::wstring& value);

		FunctionPtr applyActivationFunction(FunctionPtr pInput, ActivationFunction activationFunction);

		FunctionPtr FullyConnectedLinearLayer(Variable input, size_t outputDim, const DeviceDescriptor& device, const std::wstring& outputName = L"");

		FunctionPtr Convolution1D(Variable input, size_t kernelCount, size_t kernel, size_t stride, ActivationFunction activationFunction, double wScale, const DeviceDescriptor& device, const string& outputName = "");

		FunctionPtr Convolution2D(Variable input, size_t kernelCount, size_t kernelWidth, size_t kernelHeight, size_t strideWidth, size_t strideHeight, ActivationFunction activationFunction, double wScale, const DeviceDescriptor& device, const string& outputName = "");

		FunctionPtr Convolution3D(Variable input, size_t kernelCount, size_t kernelWidth, size_t kernelHeight, size_t kernelDepth, size_t strideWidth, size_t strideHeight, size_t strideDepth, ActivationFunction activationFunction, double wScale, const DeviceDescriptor& device, const string& outputName = "");

		FunctionPtr FullyConnectedDNNLayer(Variable input, size_t outputDim, const DeviceDescriptor& device, const std::function<FunctionPtr(const FunctionPtr&)>& nonLinearity, const std::wstring& outputName = L"");

		FunctionPtr FullyConnectedDNNLayer(Variable input, size_t outputDim, const DeviceDescriptor& device, ActivationFunction activationFunction, const std::wstring& outputName = L"");
	}
}