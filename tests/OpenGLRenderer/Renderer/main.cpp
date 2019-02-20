// StandaloneOpenGLRenderer.cpp : Defines the entry point for the console application.
//

#include "../../../tools/System/Timer.h"

#include "../../../tools/OpenGLRenderer/renderer.h"
#include "../../../tools/OpenGLRenderer/text.h"
#include "../../../tools/OpenGLRenderer/camera.h"
#include "../../../tools/OpenGLRenderer/graphic-object-3d.h"
#include "../../../tools/OpenGLRenderer/graphic-object-2d.h"
#include "../../../tools/OpenGLRenderer/basic-shapes-2d.h"
#include "../../../tools/OpenGLRenderer/input-handler.h"
#include "../../../tools/OpenGLRenderer/material.h"

#include "../../../tools/GeometryLib/quaternion.h"
#include "../../../tools/GeometryLib/vector3d.h"
#include "../../../tools/GeometryLib/transform3d.h"
#include "../../../tools/GeometryLib/vector2d.h"

#include <math.h>
#include <iostream>
using namespace std;

#define LIVE_TEX_SIZE_X 64
#define LIVE_TEX_SIZE_Y 64

int main(int argc, char** argv)
{
	Renderer* pRenderer = 0;
	FreeCameraInputHandler* pInputHandler = 0;

	pRenderer = new Renderer();
	pRenderer->setVerbose(true);

	pRenderer->init(argc, argv, 600, 400);
	pRenderer->setDataFolder("../../../config/scenes/");
	pRenderer->loadScene("robot-control.scene");
	pInputHandler = new FreeCameraInputHandler();

	Timer timer;
	double dt = 0.0;

	timer.start();

	GraphicObject3D* pDynamicObject = pRenderer->get3DObjectByName("Target");

	Meter2D* pMeter1 = new Meter2D("Num.Objects drawn", Vector2D(0.1, 0.2), Vector2D(0.4, 0.04));
	pRenderer->add2DGraphicObject(pMeter1);
	pMeter1->setValueRange(Range(0.0, 10.0));

	Meter2D* pMeter2 = new Meter2D("test2", Vector2D(0.1, 0.25), Vector2D(0.4, 0.04));
	pRenderer->add2DGraphicObject(pMeter2);
	pMeter2->setValueRange(Range(-5.0, 5.0));

	vector<double> pLiveTextureValues = vector<double>(LIVE_TEX_SIZE_X*LIVE_TEX_SIZE_Y);

	FunctionViewer* pFunctionView = new FunctionViewer3D("function-view", Vector2D(0.5, 0.5), Vector2D(0.25, 0.25),LIVE_TEX_SIZE_X, 0.0);
	pRenderer->add2DGraphicObject(pFunctionView);

	double t = 0.0;

	Text2D* pFPSText = new Text2D(string("fps"), Vector2D(0.1, 0.9), 0);
	pRenderer->add2DGraphicObject(pFPSText);

	while (!pInputHandler->exitRequested())
	{
		//UPDATE////////////////////
		////////////////////////////
		string fpsText = string("FPS: ") + std::to_string(pRenderer->getFPS());
		pFPSText->set(string(fpsText));

		if (pDynamicObject)
		{
			pDynamicObject->addRotation(Quaternion(0.0, 0.0, dt));
			pDynamicObject->addWorldOffset(Vector3D(0.5*dt, 0.0, 0.0));
		}
		t += dt;
		if (pMeter1) pMeter1->setValue(pRenderer->getNum3DObjectsDrawn());
		if (pMeter2) pMeter2->setValue(fmod(t, 5.0));
		
		//update live texture
		for (unsigned int y = 0; y < LIVE_TEX_SIZE_Y; y++)
		{
			for (unsigned int x = 0; x < LIVE_TEX_SIZE_X; x++)
			{
				double normX = -1.0 + 2.0* ((double)x) / ((double)LIVE_TEX_SIZE_X);
				double normY = -1.0 + 2.0* ((double)y) / ((double)LIVE_TEX_SIZE_Y);
				normX = normX * fmod(t, 2.0);
				normY = normY * fmod(t, 3.0);
				pLiveTextureValues[y*LIVE_TEX_SIZE_X + x] = sqrt(abs(1 - pow((double)normX,2.0) - pow((double)normY,2.0)));//sin(t*0.01)*(normX*normY);
			}
		}

		pFunctionView->update(pLiveTextureValues);

		
		pInputHandler->handleInput();

		//RENDER////////////////////
		////////////////////////////
		pRenderer->draw();


		dt = timer.getElapsedTime(true);
	}
	pRenderer->logMessage("Shutting down: input handler");
	delete pInputHandler;
	pRenderer->logMessage("Shutting down: renderer");
	delete pRenderer;
	pRenderer->logMessage("Shutting down: returning");
	return 0;
}

