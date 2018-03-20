// StandaloneOpenGLRenderer.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "../../../tools/WindowsUtils/Timer.h"

#include "../../../tools/OpenGLRenderer/Renderer.h"
#include "../../../tools/OpenGLRenderer/renderer.h"
#include "../../../tools/OpenGLRenderer/text.h"
#include "../../../tools/OpenGLRenderer/camera.h"
#include "../../../tools/OpenGLRenderer/graphic-object.h"
#include "../../../tools/OpenGLRenderer/graphic-object-2d.h"
#include "../../../tools/OpenGLRenderer/input-handler.h"
#include "../../../tools/GeometryLib/quaternion.h"
#include "../../../tools/GeometryLib/vector3d.h"
#include "../../../tools/GeometryLib/transform3d.h"
#include "../../../tools/GeometryLib/vector2d.h"

int main(int argc, char** argv)
{
	Renderer* pRenderer = 0;
	IInputHandler* pInputHandler = 0;

	pRenderer = new Renderer();
	pRenderer->init(argc, argv, 600, 400);
	pRenderer->setDataFolder("../../../config/scenes/");
	pRenderer->loadScene("test-scene.scene");
	pInputHandler = new FreeCameraInputHandler();

	Timer timer;
	double dt = 0.0;

	timer.start();

	GraphicObject* pDynamicObject = pRenderer->getObjectByName("Robot");

	Meter2D* pMeter1 = new Meter2D("Num.Objects drawn", Vector2D(0.1, 0.2), Vector2D(0.4, 0.04));
	pRenderer->add2DGraphicObject(pMeter1);
	pMeter1->setValueRange(Range(0.0, 10.0));

	Meter2D* pMeter2 = new Meter2D("test2", Vector2D(0.1, 0.25), Vector2D(0.4, 0.04));
	pRenderer->add2DGraphicObject(pMeter2);
	pMeter2->setValueRange(Range(-5.0, 5.0));
	double t = 0.0;

	Text2D* pFPSText = new Text2D(string("fps"), Vector2D(0.1, 0.9), 0);
	pRenderer->add2DGraphicObject(pFPSText);

	while (1)
	{
		//UPDATE////////////////////
		////////////////////////////
		string fpsText = string("FPS: ") + std::to_string(pRenderer->getFPS());
		pFPSText->set(string(fpsText));

		//if (pDynamicObject) pDynamicObject->addRotation(Quaternion(0.1*dt, 0.0, 0.0));
		t += dt;
		if (pMeter1) pMeter1->setValue(pRenderer->getNum3DObjectsDrawn());
		if (pMeter2) pMeter2->setValue(fmod(t, 5.0));
		//queued events?
		
		pInputHandler->handleInput();

		//RENDER////////////////////
		////////////////////////////
		pRenderer->redraw();


		dt = timer.getElapsedTime(true);
	}

	delete pRenderer;
	delete pInputHandler;
	return 0;
}

