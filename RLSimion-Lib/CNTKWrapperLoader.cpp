#include "CNTKWrapperLoader.h"
#include <windows.h>

HMODULE hCNTKWrapperDLL= 0;
InputLayerDLL InputLayer = 0;
ActivationLayerDLL ActivationLayer = 0;
Convolution1DLayerDLL Convolution1DLayer = 0;
Convolution2DLayerDLL Convolution2DLayer = 0;
Convolution3DLayerDLL Convolution3DLayer = 0;
DenseLayerDLL DenseLayer = 0;
DropoutLayerDLL DropoutLayer = 0;
FlattenLayerDLL FlattenLayer = 0;
ReshapeLayerDLL ReshapeLayer = 0;
MergeLayerDLL MergeLayer = 0;
CreateOptimizerDLL CreateOptimizer = 0;

void CNTKWrapper::Init()
{
	if (hCNTKWrapperDLL)
	{
		InputLayer = (InputLayerDLL)GetProcAddress(hCNTKWrapperDLL, "CNTKWrapper::InputLayer");
		ActivationLayer = (ActivationLayerDLL)GetProcAddress(hCNTKWrapperDLL, "CNTKWrapper::ActivationLayer");
		Convolution1DLayer = (Convolution1DLayerDLL)GetProcAddress(hCNTKWrapperDLL, "CNTKWrapper::Convolution1DLayer");
		Convolution2DLayer = (Convolution2DLayerDLL)GetProcAddress(hCNTKWrapperDLL, "CNTKWrapper::Convolution2DLayer");
		Convolution3DLayer = (Convolution3DLayerDLL)GetProcAddress(hCNTKWrapperDLL, "CNTKWrapper::Convolution3DLayer");
		DenseLayer = (DenseLayerDLL)GetProcAddress(hCNTKWrapperDLL, "CNTKWrapper::DenseLayer");
		DropoutLayer = (DropoutLayerDLL)GetProcAddress(hCNTKWrapperDLL, "CNTKWrapper::DropoutLayer");
		FlattenLayer = (FlattenLayerDLL)GetProcAddress(hCNTKWrapperDLL, "CNTKWrapper::FlattenLayer");
		ReshapeLayer = (ReshapeLayerDLL)GetProcAddress(hCNTKWrapperDLL, "CNTKWrapper::ReshapeLayer");
		MergeLayer = (MergeLayerDLL)GetProcAddress(hCNTKWrapperDLL, "CNTKWrapper::MergeLayer");
		CreateOptimizer = (CreateOptimizerDLL)GetProcAddress(hCNTKWrapperDLL, "CNTKWrapper::CreateOptimizer");
	}
}

void CNTKWrapper::Close()
{
	if (hCNTKWrapperDLL)
		FreeLibrary(hCNTKWrapperDLL);
}