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
	//LessDummyGuiHelper gui(m_pOpenGLApp);
	//DummyGUIHelper gui;
//	m_guiHelper = helper;
	initPhysics();
	resetCamera();
	//return m_pOpenGLApp;
}

/// Create graphics after adding bodies to the world
void GraphicSettings::generateGraphics(GUIHelperInterface*	helper)
{
	helper->autogenerateGraphicsObjects(m_dynamicsWorld);
}

void GraphicSettings::deleteGraphicOptions()
{
	exitPhysics();
}

void GraphicSettings::simulate(double dt)
{
	m_pOpenGLApp->m_instancingRenderer->init();
	m_pOpenGLApp->m_instancingRenderer->updateCamera(m_pOpenGLApp->getUpAxis());

	stepSimulation(dt);
}

void GraphicSettings::draw()
{
	renderScene();

	DrawGridData dg;
	dg.upAxis = m_pOpenGLApp->getUpAxis();
	m_pOpenGLApp->drawGrid(dg);

	m_pOpenGLApp->swapBuffer();
}

void GraphicSettings::drawText3D(char text[], btVector3 &position)
{
	m_pOpenGLApp->drawText3D(text, float(position.getX()), float(position.getY()), float(position.getZ()), 1);
	//m_pOpenGLApp->drawText(text, float(position.getX()), float(position.getY()), float(position.getZ()));
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

GraphicSettings::~GraphicSettings()
{
	deleteGraphicOptions();
}

