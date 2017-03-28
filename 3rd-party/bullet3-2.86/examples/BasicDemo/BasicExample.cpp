#include "BasicExample.h"
#include "btBulletDynamicsCommon.h"
#include "LinearMath/btVector3.h"
#include "LinearMath/btAlignedObjectArray.h"
#include "../CommonInterfaces/CommonRigidBodyBase.h"

#define ARRAY_SIZE_Y 5
#define ARRAY_SIZE_X 5
#define ARRAY_SIZE_Z 5
#define DT 1.f / 60.f

class Controller : public btMotionState {
public:
	Controller(const btTransform &initialpos)
	{
		m_kinematicPos = initialpos; m_velocity = btVector3(10.0, 0.0, 0.0);
	}
	virtual ~Controller() { }

	virtual void getWorldTransform(btTransform &worldTrans) const { worldTrans = m_kinematicPos; }
	void calculatePosition(double dt)
	{
		m_kinematicPos.setOrigin(m_kinematicPos.getOrigin() + m_velocity*dt);
	}
	void setKinematicPos(btTransform &currentPos) { m_kinematicPos = currentPos; }
	void getLinealVelocity(btVector3 &l_vel) {
		l_vel = m_velocity;
	}
	virtual void setWorldTransform(const btTransform &worldTrans)
	{}

protected:
	btTransform m_kinematicPos;
	btVector3 m_velocity;
};

struct BasicExample : public CommonRigidBodyBase
{
	Controller* pRobot1Controller;
	BasicExample(struct GUIHelperInterface* helper)
		:CommonRigidBodyBase(helper)
	{
	}
	virtual ~BasicExample() {}
	virtual void initPhysics();
	virtual void renderScene();
	void resetCamera()
	{
		float dist = 4;
		float pitch = 52;
		float yaw = 35;
		float targetPos[3] = { 0,0,0 };
		m_guiHelper->resetCamera(dist, pitch, yaw, targetPos[0], targetPos[1], targetPos[2]);
	}
};

void BasicExample::initPhysics()
{
	m_guiHelper->setUpAxis(1);

	///Con este método hacemos la inicialización de la configuración de colisiones, del dispacher y del dynamicsWorld
	createEmptyDynamicsWorld();
	m_guiHelper->createPhysicsDebugDrawer(m_dynamicsWorld);

	if (m_dynamicsWorld->getDebugDrawer())
		m_dynamicsWorld->getDebugDrawer()->setDebugMode(btIDebugDraw::DBG_DrawWireframe + btIDebugDraw::DBG_DrawContactPoints);

	/// inicializamos las formas, la primera el suelo
	{
		btBoxShape* groundShape = createBoxShape(btVector3(btScalar(50.), btScalar(50.), btScalar(50.)));
		m_collisionShapes.push_back(groundShape);
		btTransform groundTransform;
		groundTransform.setIdentity();
		groundTransform.setOrigin(btVector3(0, -50, 0));
		btScalar mass(0.);
		createRigidBody(mass, groundTransform, groundShape, btVector4(0, 0, 1, 1));
	}

	///creamos la box dinámica obj1
	{

		btBoxShape* colShape = createBoxShape(btVector3(1, 1, 1));
		m_collisionShapes.push_back(colShape);
		btTransform startTransform;
		startTransform.setIdentity();
		btScalar	mass(1.f);
		bool isDynamic = (mass != 0.f);
		btVector3 localInertia(0, 0, 0);
		if (isDynamic)
			colShape->calculateLocalInertia(mass, localInertia);
		startTransform.setOrigin(btVector3(3.0, 0.5, 0.0));
		createRigidBody(mass, startTransform, colShape);
	}

	//creating a kinematic robot obj2 
	{

		btCollisionShape* robot1Shape = new btSphereShape(btScalar(1));
		m_collisionShapes.push_back(robot1Shape);

		btTransform robot1startTransform;
		robot1startTransform.setIdentity();

		// if mass != 0.f object is dynamic
		btScalar	robot1mass(0.f);

		btVector3 robot1localInertia(0, 0, 0);
		robot1Shape->calculateLocalInertia(robot1mass, robot1localInertia);

		robot1startTransform.setOrigin(btVector3(2, 1.0, 0));

		//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
		pRobot1Controller = new Controller(robot1startTransform);
		btRigidBody::btRigidBodyConstructionInfo Robot1rbInfo(robot1mass, pRobot1Controller
			, robot1Shape, robot1localInertia);
		btRigidBody* Robot1body = new btRigidBody(Robot1rbInfo);
		Robot1body->setCollisionFlags(Robot1body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
		Robot1body->setActivationState(DISABLE_DEACTIVATION);
		Robot1body->activate();

		m_dynamicsWorld->addRigidBody(Robot1body);
	}


	m_guiHelper->autogenerateGraphicsObjects(m_dynamicsWorld);
}


void BasicExample::renderScene()
{

	m_dynamicsWorld->stepSimulation(DT);
	int numCollision = m_dynamicsWorld->getDispatcher()->getNumManifolds();
	//print positions of all objects
	for (int j = m_dynamicsWorld->getNumCollisionObjects() - 1; j >= 0; j--)
	{
		btCollisionObject* obj = m_dynamicsWorld->getCollisionObjectArray()[j];
		btRigidBody* body = btRigidBody::upcast(obj);
		btTransform trans;
		btVector3 l_vel;
		if (body && body->getMotionState())
		{
			switch (j)
			{
			case 0: //floor, no motion state needed because it won't move
				trans = obj->getWorldTransform(); break;
			case 1: //box, we have to take the transformation from the motion state
					//because it may have been moved
				body->getMotionState()->getWorldTransform(trans); break;

			case 2:	//robot, we have to set ourselves the transformation and communicate it to Bullet
					//through the motion state
					///SETLINEARVELOCITY IRIA AQUI?
				pRobot1Controller->getLinealVelocity(l_vel);
				pRobot1Controller->calculatePosition(DT);
				body->setLinearVelocity(l_vel);
				pRobot1Controller->getWorldTransform(trans);
				body->getMotionState()->setWorldTransform(trans); break;
			}
		}
	}

	CommonRigidBodyBase::renderScene();

}

CommonExampleInterface*    BasicExampleCreateFunc(CommonExampleOptions& options)
{
	return new BasicExample(options.m_guiHelper);

}

B3_STANDALONE_EXAMPLE(BasicExampleCreateFunc)



