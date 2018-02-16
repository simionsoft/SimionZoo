// CNTKLoadTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>

#define CNTK_HEADERONLY_DEFINITIONS
#include "CNTKLibrary.h"
class CLink;
#include <vector>
using namespace std;

class CProblem;
class COptimizerSetting;
namespace tinyxml2 { class XMLElement; }


typedef CNTK::FunctionPtr (__stdcall *InputLayerDLL)(const CLink* pLink, vector<const CLink*> dependencies, CNTK::DeviceDescriptor& device);
typedef CNTK::FunctionPtr (__stdcall *ActivationLayerDLL)(const CLink* pLink, vector<const CLink*> dependencies, CNTK::DeviceDescriptor& device);
typedef CNTK::FunctionPtr (__stdcall * Convolution1DLayerDLL)(const CLink* pLink, vector<const CLink*> dependencies, CNTK::DeviceDescriptor& device);
typedef CNTK::FunctionPtr (__stdcall * Convolution2DLayerDLL)(const CLink* pLink, vector<const CLink*> dependencies, CNTK::DeviceDescriptor& device);
typedef CNTK::FunctionPtr (__stdcall * Convolution3DLayerDLL)(const CLink* pLink, vector<const CLink*> dependencies, CNTK::DeviceDescriptor& device);
typedef CNTK::FunctionPtr (__stdcall * DenseLayerDLL)(const CLink* pLink, vector<const CLink*> dependencies, CNTK::DeviceDescriptor& device);
typedef CNTK::FunctionPtr (__stdcall * DropoutLayerDLL)(const CLink* pLink, vector<const CLink*> dependencies, CNTK::DeviceDescriptor& device);
typedef CNTK::FunctionPtr (__stdcall * FlattenLayerDLL)(const CLink* pLink, vector<const CLink*> dependencies, CNTK::DeviceDescriptor& device);
typedef CNTK::FunctionPtr (__stdcall * ReshapeLayerDLL)(const CLink* pLink, vector<const CLink*> dependencies, CNTK::DeviceDescriptor& device);
typedef CNTK::FunctionPtr (__stdcall * MergeLayerDLL)(const CLink* pLink, vector<const CLink*> dependencies, CNTK::DeviceDescriptor& device);
typedef CNTK::TrainerPtr (__stdcall * CreateOptimizerDLL)(const COptimizerSetting* pOptimizerSetting, CNTK::FunctionPtr& model, CNTK::FunctionPtr& lossFunction);
typedef CProblem* (__stdcall * getProblemInstanceDLL)(tinyxml2::XMLElement* pNode);

getProblemInstanceDLL getProblemInstance;
InputLayerDLL InputLayer;
ActivationLayerDLL ActivationLayer;
Convolution1DLayerDLL Convolution1DLayer;
Convolution2DLayerDLL Convolution2DLayer;
Convolution3DLayerDLL Convolution3DLayer;
DenseLayerDLL DenseLayer;
DropoutLayerDLL DropoutLayer;
FlattenLayerDLL FlattenLayer;
ReshapeLayerDLL ReshapeLayer;
MergeLayerDLL MergeLayer;
CreateOptimizerDLL CreateOptimizer;

int main()
{
	HMODULE hLibrary;
#ifdef _DEBUG
	hLibrary= LoadLibrary(L".\\..\\..\\Debug\\x64\\CNTKWrapper.dll");
#else
	hLibrary = LoadLibrary(L".\\..\\..\\bin\\x64\\CNTKWrapper.dll");
#endif


	DWORD error= 0;
	if (hLibrary)
	{
		//foo(12);
		getProblemInstance = (getProblemInstanceDLL)GetProcAddress(hLibrary, "CNTKWrapper::getProblemInstance");
		CProblem* pProblem = getProblemInstance(0);
		InputLayer = (InputLayerDLL)GetProcAddress(hLibrary, "CNTKWrapper::InputLayer");
		ActivationLayer= (ActivationLayerDLL)GetProcAddress(hLibrary, "CNTKWrapper::ActivationLayer");
		Convolution1DLayer = (Convolution1DLayerDLL)GetProcAddress(hLibrary, "CNTKWrapper::Convolution1DLayer");
		Convolution2DLayer = (Convolution2DLayerDLL)GetProcAddress(hLibrary, "CNTKWrapper::Convolution2DLayer");
		Convolution3DLayer = (Convolution3DLayerDLL)GetProcAddress(hLibrary, "CNTKWrapper::Convolution3DLayer");
		DenseLayer = (DenseLayerDLL)GetProcAddress(hLibrary, "CNTKWrapper::DenseLayer");
		DropoutLayer = (DropoutLayerDLL)GetProcAddress(hLibrary, "CNTKWrapper::DropoutLayer");
		FlattenLayer = (FlattenLayerDLL)GetProcAddress(hLibrary, "CNTKWrapper::FlattenLayer");
		ReshapeLayer = (ReshapeLayerDLL)GetProcAddress(hLibrary, "CNTKWrapper::ReshapeLayer");
		MergeLayer = (MergeLayerDLL)GetProcAddress(hLibrary, "CNTKWrapper::MergeLayer");
		CreateOptimizer = (CreateOptimizerDLL)GetProcAddress(hLibrary, "CNTKWrapper::CreateOptimizer");

		if (InputLayer)
		{
			printf("successssssss");
		}
		else
			error = GetLastError();

		FreeLibrary(hLibrary);
	}
	else
		error= GetLastError();
    return 0;
}

