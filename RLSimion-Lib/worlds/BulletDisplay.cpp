#include "../stdafx.h"
#include "BulletDisplay.h"
#include "OpenGLGuiHelper.h"
#include "BulletPhysics.h"
#pragma comment(lib,"opengl32.lib")
#define GLEW_STATIC
int gSharedMemoryKey = -1;

GUIHelperInterface* BulletGraphic::getGuiHelper()
{
	return m_pHelper;
}

/// Set of world's debugger. Must be called after InitPhysics
void BulletGraphic::setDebugger(btDiscreteDynamicsWorld* m_dynamicsWorld)
{
	m_pHelper->setUpAxis(1);
	m_pHelper->createPhysicsDebugDrawer(m_dynamicsWorld);
	if (m_dynamicsWorld->getDebugDrawer())
		m_dynamicsWorld->getDebugDrawer()->setDebugMode(btIDebugDraw::DBG_DrawWireframe + btIDebugDraw::DBG_DrawContactPoints
			+ btIDebugDraw::DBG_DrawNormals);
	resetCamera();
}

/// Set of world's debugger. Must be called after InitSoftPhysics
void BulletGraphic::setSoftDebugger(btSoftRigidDynamicsWorld* m_dynamicsWorld)
{
	m_pHelper->setUpAxis(1);
	m_pHelper->createPhysicsDebugDrawer(m_dynamicsWorld);
	if (m_dynamicsWorld->getDebugDrawer())
		m_dynamicsWorld->getDebugDrawer()->setDebugMode(btIDebugDraw::DBG_DrawWireframe + btIDebugDraw::DBG_DrawContactPoints
			+ btIDebugDraw::DBG_DrawNormals);
	resetCamera();
}

/// Create graphics after adding bodies to the world
void BulletGraphic::generateGraphics(btDiscreteDynamicsWorld* m_dynamicsWorld)
{
	m_pHelper->autogenerateGraphicsObjects(m_dynamicsWorld);
}

/// Update the camera, AFTER SIMULATE
void BulletGraphic::updateCamera()
{
	m_pOpenGLApp->m_instancingRenderer->init();
	m_pOpenGLApp->m_instancingRenderer->updateCamera(m_pOpenGLApp->getUpAxis());
}

/// Reset camera. Values can be changed to modify the position of the camera
void BulletGraphic::resetCamera() {
	float dist = 25;
	float pitch = 52;
	float yaw = 35;
	float targetPos[3] = { 0,0,0 };
	m_pHelper->resetCamera(dist, pitch, yaw, targetPos[0], targetPos[1], targetPos[2]);
}

void BulletGraphic::renderScene(btDiscreteDynamicsWorld* m_dynamicsWorld)
{
	m_pHelper->syncPhysicsToGraphics(m_dynamicsWorld);
	m_pHelper->render(m_dynamicsWorld);
}

///Promotes the contents of the back buffer of the layer in use of the current window to become the contents of the front buffer.
void BulletGraphic::swapBuffers()
{
	DrawGridData dg;
	dg.upAxis = m_pOpenGLApp->getUpAxis();
	m_pOpenGLApp->drawGrid(dg);

	m_pOpenGLApp->swapBuffer();
}

/// Draw the scene - only for discrete dynamics worlds
void BulletGraphic::drawDynamicWorld(btDiscreteDynamicsWorld* m_dynamicsWorld)
{
	renderScene(m_dynamicsWorld);
	swapBuffers();
}

/// Draw the scene - only for soft worlds
void BulletGraphic::drawSoftWorld(btSoftRigidDynamicsWorld* m_softWorld)
{
	renderScene((btDiscreteDynamicsWorld*)m_softWorld);

	for (int i = 0; i < m_softWorld->getSoftBodyArray().size(); i++)
	{
		double size = m_softWorld->getSoftBodyArray().size();
		btSoftBody*	psb = (btSoftBody*)m_softWorld->getSoftBodyArray()[i];
		{
			btSoftBodyHelpers::DrawFrame(psb, m_softWorld->getDebugDrawer());
			btSoftBodyHelpers::Draw(psb, m_softWorld->getDebugDrawer(), m_softWorld->getDrawFlags());
		}
	}

	swapBuffers();
}

/// Print 3D text in a determined possition
void BulletGraphic::drawText3D(char text[], btVector3 &position)
{
	m_pOpenGLApp->drawText3D(text, float(position.getX()), float(position.getY()), float(position.getZ()), 1);
}

BulletGraphic::~BulletGraphic()
{
}

B3_STANDALONE_EXAMPLE(StandaloneExampleCreateFunc)

