#include "stdafx.h"
#include "BulletDisplay.h"
#include "OpenGLGuiHelper.h"
#include "BulletPhysics.h"
#pragma comment(lib,"opengl32.lib")
#define GLEW_STATIC
int gSharedMemoryKey = -1;

GUIHelperInterface* BulletViewer::getGuiHelper()
{
	return m_pHelper;
}

/// Set of world's debugger. Must be called after InitPhysics
void BulletViewer::setDebugger(btDiscreteDynamicsWorld* m_dynamicsWorld)
{
	m_pHelper->setUpAxis(1);
	m_pHelper->createPhysicsDebugDrawer(m_dynamicsWorld);
	if (m_dynamicsWorld->getDebugDrawer())
		m_dynamicsWorld->getDebugDrawer()->setDebugMode(btIDebugDraw::DBG_DrawWireframe + btIDebugDraw::DBG_DrawContactPoints
			+ btIDebugDraw::DBG_DrawNormals);
	resetCamera();
}

/// Set of world's debugger. Must be called after InitSoftPhysics
void BulletViewer::setSoftDebugger(btSoftRigidDynamicsWorld* m_dynamicsWorld)
{
	m_pHelper->setUpAxis(1);
	m_pHelper->createPhysicsDebugDrawer(m_dynamicsWorld);
	if (m_dynamicsWorld->getDebugDrawer())
		m_dynamicsWorld->getDebugDrawer()->setDebugMode(btIDebugDraw::DBG_DrawWireframe + btIDebugDraw::DBG_DrawContactPoints
			+ btIDebugDraw::DBG_DrawNormals);
	resetCamera();
}

/// Create graphics after adding bodies to the world
void BulletViewer::generateGraphics(btDiscreteDynamicsWorld* m_dynamicsWorld)
{
	m_pHelper->autogenerateGraphicsObjects(m_dynamicsWorld);
}

/// Update the camera, AFTER SIMULATE
void BulletViewer::updateCamera()
{
	m_pOpenGLApp->m_instancingRenderer->init();
	m_pOpenGLApp->m_instancingRenderer->updateCamera(m_pOpenGLApp->getUpAxis());
}

/// Reset camera. Values can be changed to modify the position of the camera
void BulletViewer::resetCamera() {
	float dist = 25;
	float pitch = 52;
	float yaw = 35;
	float targetPos[3] = { 0,0,0 };
	m_pHelper->resetCamera(dist, pitch, yaw, targetPos[0], targetPos[1], targetPos[2]);
}

void BulletViewer::renderScene(btDiscreteDynamicsWorld* m_dynamicsWorld)
{
	m_pHelper->syncPhysicsToGraphics(m_dynamicsWorld);
	m_pHelper->render(m_dynamicsWorld);
}

///Promotes the contents of the back buffer of the layer in use of the current window to become the contents of the front buffer.
void BulletViewer::swapBuffers()
{
	DrawGridData dg;
	dg.upAxis = m_pOpenGLApp->getUpAxis();
	m_pOpenGLApp->drawGrid(dg);

	m_pOpenGLApp->swapBuffer();
}

/// Draw the scene - only for discrete dynamics worlds
void BulletViewer::drawDynamicWorld(btDiscreteDynamicsWorld* m_dynamicsWorld)
{
	renderScene(m_dynamicsWorld);
	swapBuffers();
}

/// Draw the scene - only for soft worlds
void BulletViewer::drawSoftWorld(btSoftRigidDynamicsWorld* m_softWorld)
{
	renderScene((btDiscreteDynamicsWorld*)m_softWorld);

	for (int i = 0; i < m_softWorld->getSoftBodyArray().size(); i++)
	{
		btSoftBody*	psb = (btSoftBody*)m_softWorld->getSoftBodyArray()[i];
		{
			btSoftBodyHelpers::DrawFrame(psb, m_softWorld->getDebugDrawer());
			btSoftBodyHelpers::Draw(psb, m_softWorld->getDebugDrawer(), m_softWorld->getDrawFlags());
		}
	}

	swapBuffers();
}

/// Print 3D text in a determined possition
void BulletViewer::drawText3D(char text[], btVector3 &position)
{
	m_pOpenGLApp->drawText3D(text, float(position.getX()), float(position.getY()), float(position.getZ()), 1);
}

BulletViewer::~BulletViewer()
{
}

B3_STANDALONE_EXAMPLE(StandaloneExampleCreateFunc)

