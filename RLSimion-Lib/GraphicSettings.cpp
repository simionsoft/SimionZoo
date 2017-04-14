#include "stdafx.h"
#include "GraphicSettings.h"
#include "OpenGLGuiHelper.h"

#pragma comment(lib,"opengl32.lib")

#define GLEW_STATIC

int gSharedMemoryKey = -1;

void BulletBuilder::resetCamera() {
	float dist = 25;
	float pitch = 52;
	float yaw = 35;
	float targetPos[3] = { 0,0,0 };
	m_guiHelper->resetCamera(dist, pitch, yaw, targetPos[0], targetPos[1], targetPos[2]);
}

// Inicialization of physics
void BulletBuilder::initPhysics()
{
	m_guiHelper->setUpAxis(1);
	createEmptyDynamicsWorld();
	m_guiHelper->createPhysicsDebugDrawer(m_dynamicsWorld);
	if (m_dynamicsWorld->getDebugDrawer())
		m_dynamicsWorld->getDebugDrawer()->setDebugMode(btIDebugDraw::DBG_DrawWireframe + btIDebugDraw::DBG_DrawContactPoints
			+ btIDebugDraw::DBG_DrawNormals);
}

// create OpenGL window and Bullet World
void BulletBuilder::initializeBulletRequirements()
{
	initPhysics();
	resetCamera();
}

/// Create graphics after adding bodies to the world
void BulletBuilder::generateGraphics(GUIHelperInterface*	helper)
{
	helper->autogenerateGraphicsObjects(m_dynamicsWorld);
}

// Step the simulation 
void BulletBuilder::simulate(double dt)
{
	m_pOpenGLApp->m_instancingRenderer->init();
	m_pOpenGLApp->m_instancingRenderer->updateCamera(m_pOpenGLApp->getUpAxis());

	stepSimulation(dt);
}

// Draw the scene
void BulletBuilder::draw()
{
	renderScene();

	DrawGridData dg;
	dg.upAxis = m_pOpenGLApp->getUpAxis();
	m_pOpenGLApp->drawGrid(dg);

	m_pOpenGLApp->swapBuffer();
}

// Print 3D text in a determined possition
void BulletBuilder::drawText3D(char text[], btVector3 &position)
{
	m_pOpenGLApp->drawText3D(text, float(position.getX()), float(position.getY()), float(position.getZ()), 1);
}

// Needed getter in order to add shapes to the array of bullets collision shapes
btAlignedObjectArray<btCollisionShape*> BulletBuilder::getCollisionShape()
{
	return m_collisionShapes;
}

GUIHelperInterface* BulletBuilder::getGuiHelper()
{
	return m_guiHelper;
}

// Delete every graphic object from outside
void BulletBuilder::deleteGraphicOptions()
{
	exitPhysics();
}

BulletBuilder::~BulletBuilder()
{
	deleteGraphicOptions();
}

B3_STANDALONE_EXAMPLE(StandaloneExampleCreateFunc)
