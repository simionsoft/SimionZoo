#include "../stdafx.h"
#include "GraphicSettings.h"
#include "OpenGLGuiHelper.h"


#pragma comment(lib,"opengl32.lib")

#define GLEW_STATIC

void BulletViewer::resetCamera() {
	float dist = 25;
	float pitch = 52;
	float yaw = 35;
	float targetPos[3] = { 0,0,0 };
	m_guiHelper->resetCamera(dist, pitch, yaw, targetPos[0], targetPos[1], targetPos[2]);
}

// Inicialization of physics
void BulletViewer::initPhysics()
{
	m_guiHelper->setUpAxis(1);
	createEmptyDynamicsWorld();
	m_guiHelper->createPhysicsDebugDrawer(m_dynamicsWorld);
	if (m_dynamicsWorld->getDebugDrawer())
		m_dynamicsWorld->getDebugDrawer()->setDebugMode(btIDebugDraw::DBG_DrawWireframe + btIDebugDraw::DBG_DrawContactPoints
			+ btIDebugDraw::DBG_DrawNormals);
}

// create OpenGL window and Bullet World
void BulletViewer::initBullet()
{
	initPhysics();
	resetCamera();
}


void BulletViewer::initSoftPhysics(btSoftBodyWorldInfo* m_sBodyWorldInfo)
{
	m_guiHelper->setUpAxis(1);
	{
		m_collisionConfiguration = new btSoftBodyRigidBodyCollisionConfiguration();
		m_dispatcher = new	btCollisionDispatcher(m_collisionConfiguration);
		m_broadphase = new btDbvtBroadphase();
		///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
		btSequentialImpulseConstraintSolver* sol = new btSequentialImpulseConstraintSolver;
		m_solver = sol;
		m_dynamicsWorld = new btSoftRigidDynamicsWorld(m_dispatcher, m_broadphase, m_solver, m_collisionConfiguration);
		m_dynamicsWorld->setGravity(btVector3(0, -9.8, 0));

		m_sBodyWorldInfo->m_broadphase = m_broadphase;
		m_sBodyWorldInfo->m_dispatcher = m_dispatcher;
		m_sBodyWorldInfo->m_gravity = m_dynamicsWorld->getGravity();
		m_sBodyWorldInfo->m_sparsesdf.Initialize();

	}
	m_guiHelper->createPhysicsDebugDrawer(m_dynamicsWorld);
	if (m_dynamicsWorld->getDebugDrawer())
		m_dynamicsWorld->getDebugDrawer()->setDebugMode(btIDebugDraw::DBG_DrawWireframe + btIDebugDraw::DBG_DrawContactPoints
			+ btIDebugDraw::DBG_DrawNormals);
}

void BulletViewer::initSoftBullet(btSoftBodyWorldInfo* m_sBodyWorldInfo)
{
	initSoftPhysics(m_sBodyWorldInfo);
	resetCamera();
}


/// Create graphics after adding bodies to the world
void BulletViewer::generateGraphics(GUIHelperInterface*	helper)
{
	helper->autogenerateGraphicsObjects(m_dynamicsWorld);
}

// Step the simulation 
void BulletViewer::simulate(double dt)
{
	m_pOpenGLApp->m_instancingRenderer->init();
	m_pOpenGLApp->m_instancingRenderer->updateCamera(m_pOpenGLApp->getUpAxis());

	stepSimulation((float)dt);
}

// Draw the scene
void BulletViewer::draw()
{
	renderScene();

	DrawGridData dg;
	dg.upAxis = m_pOpenGLApp->getUpAxis();
	m_pOpenGLApp->drawGrid(dg);

	m_pOpenGLApp->swapBuffer();
}

// Print 3D text in a determined possition
void BulletViewer::drawText3D(char text[], btVector3 &position)
{
	m_pOpenGLApp->drawText3D(text, float(position.getX()), float(position.getY()), float(position.getZ()), 1);
}

// Needed getter in order to add shapes to the array of bullets collision shapes
btAlignedObjectArray<btCollisionShape*> BulletViewer::getCollisionShape()
{
	return m_collisionShapes;
}

GUIHelperInterface* BulletViewer::getGuiHelper()
{
	return m_guiHelper;
}

// Delete every graphic object from outside
void BulletViewer::deleteGraphicOptions()
{
	exitPhysics();
}

BulletViewer::~BulletViewer()
{
	deleteGraphicOptions();
}

B3_STANDALONE_EXAMPLE(StandaloneExampleCreateFunc)


///////////////////////////////////////////
//ONLY CAN BE USED WITH SOFTDYNAMICSWORLD//
//////////////////////////////////////////

void BulletViewer::connectWithRope(btRigidBody* body1, btRigidBody* body2, btSoftBodyWorldInfo* btInfo)
{
	//btSoftBodyWorldInfo btInf = btInfo;
	btSoftBody*	softBodyRope0 = btSoftBodyHelpers::CreateRope(*btInfo, body1->getWorldTransform().getOrigin(), body2->getWorldTransform().getOrigin(), 4, 0);
	softBodyRope0->setTotalMass(0.1f);

	softBodyRope0->appendAnchor(0, body1);
	softBodyRope0->appendAnchor(softBodyRope0->m_nodes.size() - 1, body2);

	softBodyRope0->m_cfg.piterations = 5;
	softBodyRope0->m_cfg.kDP = 0.005f;
	softBodyRope0->m_cfg.kSHR = 1;
	softBodyRope0->m_cfg.kCHR = 1;
	softBodyRope0->m_cfg.kKHR = 1;
	getSoftDynamicsWorld()->addSoftBody(softBodyRope0);
}

btSoftRigidDynamicsWorld* BulletViewer::getSoftDynamicsWorld()
{
	return (btSoftRigidDynamicsWorld*)m_dynamicsWorld;
}
//
//btSoftBodyWorldInfo* BulletViewer::getBtSoftBodyInfo()
//{
//	return m_sBodyWorldInfo;
//}

void BulletViewer::drawSoftWorld()
{
	CommonRigidBodyBase::renderScene();
	btSoftRigidDynamicsWorld* softWorld = getSoftDynamicsWorld();

	for (int i = 0; i < softWorld->getSoftBodyArray().size(); i++)
	{
		btSoftBody*	psb = (btSoftBody*)softWorld->getSoftBodyArray()[i];
		{
			btSoftBodyHelpers::DrawFrame(psb, softWorld->getDebugDrawer());
			btSoftBodyHelpers::Draw(psb, softWorld->getDebugDrawer(), softWorld->getDrawFlags());
		}
	}

	DrawGridData dg;
	dg.upAxis = m_pOpenGLApp->getUpAxis();
	m_pOpenGLApp->drawGrid(dg);
	m_pOpenGLApp->swapBuffer();
}