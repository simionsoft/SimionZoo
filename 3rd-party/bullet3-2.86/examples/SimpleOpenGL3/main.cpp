#include "OpenGLWindow/SimpleOpenGL3App.h"
#include "Bullet3Common/b3Quaternion.h"
#include "assert.h"
#include <stdio.h>
#include "../src/btBulletDynamicsCommon.h"

#define DT 1.f / 60.f

static b3WheelCallback sOldWheelCB = 0;
static b3ResizeCallback sOldResizeCB = 0;

float gWidth = 1024;
float gHeight = 768;

void MyWheelCallback(float deltax, float deltay)
{
	if (sOldWheelCB)
		sOldWheelCB(deltax,deltay);
}
void MyResizeCallback( float width, float height)
{
    gWidth = width;
    gHeight = height;
    
	if (sOldResizeCB)
		sOldResizeCB(width,height);
}

class Controller : public btMotionState {
public:
	Controller(const btTransform &initialpos)
	{
		m_kinematicPos = initialpos; m_velocity = btVector3(0.8, 0.0, 0.0);
	}
	virtual ~Controller() { }

	virtual void getWorldTransform(btTransform &worldTrans) const { worldTrans = m_kinematicPos; }
	void calculatePosition(double dt)
	{
		m_kinematicPos.setOrigin(m_kinematicPos.getOrigin() + m_velocity*dt);
	}
	void setKinematicPos(btTransform &currentPos) { m_kinematicPos = currentPos; }
	virtual void setWorldTransform(const btTransform &worldTrans)
	{}

protected:
	btTransform m_kinematicPos;
	btVector3 m_velocity;
};

int main(int argc, char* argv[])
{
	///-----includes_end-----

	int i;
	///-----initialization_start-----

	///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
	btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();

	///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	btCollisionDispatcher* dispatcher = new	btCollisionDispatcher(collisionConfiguration);

	///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
	btBroadphaseInterface* overlappingPairCache = new btDbvtBroadphase();

	///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;

	btDiscreteDynamicsWorld* dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);

	dynamicsWorld->setGravity(btVector3(0, -9.8, 0));

	///-----initialization_end-----

	//keep track of the shapes, we release memory at exit.
	//make sure to re-use collision shapes among rigid bodies whenever possible!
	btAlignedObjectArray<btCollisionShape*> collisionShapes;

	Controller* pRobot1Controller;

	{
		SimpleOpenGL3App* app = new SimpleOpenGL3App("SimpleOpenGL3App", gWidth, gHeight, true);

		app->m_instancingRenderer->getActiveCamera()->setCameraDistance(13);
		app->m_instancingRenderer->getActiveCamera()->setCameraPitch(0);
		app->m_instancingRenderer->getActiveCamera()->setCameraTargetPosition(0, 0, 0);
		sOldWheelCB = app->m_window->getWheelCallback();
		app->m_window->setWheelCallback(MyWheelCallback);
		sOldResizeCB = app->m_window->getResizeCallback();
		app->m_window->setResizeCallback(MyResizeCallback);


		char fileName[1024];
		int textureWidth = 128;
		int textureHeight = 128;


		int cubeIndex = app->registerCubeShape(1, 1, 1);
		b3Vector3 posCube = b3MakeVector3(0, 0, 0);
		b3Quaternion ornCube(0, 0, 0, 1);
		b3Vector3 colorCube = b3MakeVector3(1, 0, 0);
		b3Vector3 scalingCube = b3MakeVector3 (1, 1, 1);
		app->m_renderer->registerGraphicsInstance(cubeIndex, posCube, ornCube, colorCube, scalingCube);
		app->m_renderer->writeTransforms();

		do
		{
			app->m_instancingRenderer->init();
			app->m_instancingRenderer->updateCamera();
			app->m_renderer->renderScene();
			app->drawGrid();
			app->swapBuffer();


		} while (!app->m_window->requestedExit());

		delete app;
	}
	return 0;
}
