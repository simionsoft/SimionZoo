#pragma once
#include "../tools/CNTKWrapper/CNTKWrapper.h"

typedef CNTK::FunctionPtr(__stdcall *InputLayerDLL)(const CLink* pLink, vector<const CLink*> dependencies, CNTK::DeviceDescriptor& device);
typedef CNTK::FunctionPtr(__stdcall *ActivationLayerDLL)(const CLink* pLink, vector<const CLink*> dependencies, CNTK::DeviceDescriptor& device);
typedef CNTK::FunctionPtr(__stdcall * Convolution1DLayerDLL)(const CLink* pLink, vector<const CLink*> dependencies, CNTK::DeviceDescriptor& device);
typedef CNTK::FunctionPtr(__stdcall * Convolution2DLayerDLL)(const CLink* pLink, vector<const CLink*> dependencies, CNTK::DeviceDescriptor& device);
typedef CNTK::FunctionPtr(__stdcall * Convolution3DLayerDLL)(const CLink* pLink, vector<const CLink*> dependencies, CNTK::DeviceDescriptor& device);
typedef CNTK::FunctionPtr(__stdcall * DenseLayerDLL)(const CLink* pLink, vector<const CLink*> dependencies, CNTK::DeviceDescriptor& device);
typedef CNTK::FunctionPtr(__stdcall * DropoutLayerDLL)(const CLink* pLink, vector<const CLink*> dependencies, CNTK::DeviceDescriptor& device);
typedef CNTK::FunctionPtr(__stdcall * FlattenLayerDLL)(const CLink* pLink, vector<const CLink*> dependencies, CNTK::DeviceDescriptor& device);
typedef CNTK::FunctionPtr(__stdcall * ReshapeLayerDLL)(const CLink* pLink, vector<const CLink*> dependencies, CNTK::DeviceDescriptor& device);
typedef CNTK::FunctionPtr(__stdcall * MergeLayerDLL)(const CLink* pLink, vector<const CLink*> dependencies, CNTK::DeviceDescriptor& device);
typedef CNTK::TrainerPtr(__stdcall * CreateOptimizerDLL)(const COptimizerSetting* pOptimizerSetting, CNTK::FunctionPtr& model, CNTK::FunctionPtr& lossFunction);

extern InputLayerDLL InputLayer;
extern ActivationLayerDLL ActivationLayer;
extern Convolution1DLayerDLL Convolution1DLayer;
extern Convolution2DLayerDLL Convolution2DLayer;
extern Convolution3DLayerDLL Convolution3DLayer;
extern DenseLayerDLL DenseLayer;
extern DropoutLayerDLL DropoutLayer;
extern FlattenLayerDLL FlattenLayer;
extern ReshapeLayerDLL ReshapeLayer;
extern MergeLayerDLL MergeLayer;
extern CreateOptimizerDLL CreateOptimizer;

namespace CNTKWrapper
{
	void Init();
	void Close();
}

