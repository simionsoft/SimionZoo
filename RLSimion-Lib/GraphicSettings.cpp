#include "stdafx.h"
#include "GraphicSettings.h"
#include "OpenGLGuiHelper.h"

#pragma comment(lib,"opengl32.lib")

#define GLEW_STATIC

int gSharedMemoryKey = -1;

void GraphicSettings::resetCamera() {
	float dist = 25;
	float pitch = 52;
	float yaw = 35;
	float targetPos[3] = { 0,0,0 };
	m_guiHelper->resetCamera(dist, pitch, yaw, targetPos[0], targetPos[1], targetPos[2]);
}

// Inicialization of physics
void GraphicSettings::initPhysics()
{
	m_guiHelper->setUpAxis(1);
	createEmptyDynamicsWorld();
	m_guiHelper->createPhysicsDebugDrawer(m_dynamicsWorld);
	if (m_dynamicsWorld->getDebugDrawer())
		m_dynamicsWorld->getDebugDrawer()->setDebugMode(btIDebugDraw::DBG_DrawWireframe + btIDebugDraw::DBG_DrawContactPoints
			+ btIDebugDraw::DBG_DrawNormals);
}

// create OpenGL window
void GraphicSettings::initializeGraphicProccess()
{
	//LessDummyGuiHelper gui(app);
	//DummyGUIHelper gui;
//	m_guiHelper = helper;
	initPhysics();
	resetCamera();
	//return app;
}

/// Create graphics after adding bodies to the world
void GraphicSettings::generateGraphics(GUIHelperInterface*	helper)
{
	helper->autogenerateGraphicsObjects(m_dynamicsWorld);
}

int GraphicSettings::deteleGraphicOptions(SimpleOpenGL3App* app)
{
	exitPhysics();
//	delete OpenGLInterface;
	delete app;
	return 0;
}

void GraphicSettings::updateGLApp(SimpleOpenGL3App* app)
{
	b3Clock clock;
	do
	{
		app->m_instancingRenderer->init();
		app->m_instancingRenderer->updateCamera(app->getUpAxis());

		btScalar dtSec = btScalar(clock.getTimeInSeconds());
		if (dtSec<0.1)
			dtSec = 0.1;

		stepSimulation(dtSec);
		clock.reset();

		renderScene();

		DrawGridData dg;
		dg.upAxis = app->getUpAxis();
		app->drawGrid(dg);

		app->swapBuffer();
	} while (!app->m_window->requestedExit());
}

btAlignedObjectArray<btCollisionShape*> GraphicSettings::getCollisionShape()
{
	return m_collisionShapes;
}

B3_STANDALONE_EXAMPLE(StandaloneExampleCreateFunc)

GUIHelperInterface* GraphicSettings::getGuiHelper()
{
	return m_guiHelper;
}

