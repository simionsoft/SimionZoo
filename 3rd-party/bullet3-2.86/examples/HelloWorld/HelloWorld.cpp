
///-----includes_start-----
#include "btBulletDynamicsCommon.h"
#include <stdio.h>

#define DT 1.f / 60.f

/// This is a Hello World program for running a basic Bullet physics simulation

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

int main(int argc, char** argv)
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

	///create a few basic rigid bodies

	//Creating the ground the ground is a box of side 1000x1000 at position y = 0.
	{
		btCollisionShape* groundShape = new btBoxShape(btVector3(btScalar(1000.), btScalar(1.), btScalar(1000.)));
		collisionShapes.push_back(groundShape);

		btTransform groundTransform;
		groundTransform.setIdentity();
		groundTransform.setOrigin(btVector3(0, 0, 0));

		btScalar mass(0.);
		btVector3 localInertia(0, 0, 0);

		//using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
		btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
		btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, groundShape, localInertia);
		btRigidBody* body = new btRigidBody(rbInfo);

		//add the body to the dynamics world
		dynamicsWorld->addRigidBody(body);
	}
	//creating a kinematic robot obj1 
	{

		btCollisionShape* robot1Shape = new btSphereShape(btScalar(1.));
		collisionShapes.push_back(robot1Shape);

		btTransform robot1startTransform;
		robot1startTransform.setIdentity();

		// if mass != 0.f object is dynamic
		btScalar	robot1mass(0.f);

		btVector3 robot1localInertia(0, 0, 0);
		robot1Shape->calculateLocalInertia(robot1mass, robot1localInertia);

		robot1startTransform.setOrigin(btVector3(0, 0, 0));

		//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
		pRobot1Controller = new Controller(robot1startTransform);
		btRigidBody::btRigidBodyConstructionInfo Robot1rbInfo(robot1mass, pRobot1Controller
			, robot1Shape, robot1localInertia);
		btRigidBody* Robot1body = new btRigidBody(Robot1rbInfo);
		Robot1body->setCollisionFlags(Robot1body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
		Robot1body->setActivationState(DISABLE_DEACTIVATION);
		Robot1body->activate();

		dynamicsWorld->addRigidBody(Robot1body);
	}
	//creating a dynamic box obj2
	{
		btCollisionShape* boxShape = new btBoxShape(btVector3(btScalar(1.), btScalar(1.), btScalar(1.)));
		collisionShapes.push_back(boxShape);

		btTransform boxTransform;
		boxTransform.setIdentity();
		boxTransform.setOrigin(btVector3(5.0, 4.0, 0.0));

		btScalar massBox(1.);
		btVector3 BoxlocalInertia(0, 0, 0);
		boxShape->calculateLocalInertia(massBox, BoxlocalInertia);

		//using motionstate is optional, it provides interpolation capabilities, and only synchronizes 'active' objects
		btDefaultMotionState* myMotionState = new btDefaultMotionState(boxTransform);
		btRigidBody::btRigidBodyConstructionInfo BoxrbInfo(massBox, myMotionState, boxShape, BoxlocalInertia);
		btRigidBody* Boxbody = new btRigidBody(BoxrbInfo);

		//add the body to the dynamics world
		dynamicsWorld->addRigidBody(Boxbody);
	}

	/// Simulation
	///-----stepsimulation_start-----
	int numCollision;
	btVector3 linealVel(0.5, 0, 0);

	for (i = 0; i < 5000; i++)
	{
		dynamicsWorld->stepSimulation(DT, 10);

		numCollision = dynamicsWorld->getDispatcher()->getNumManifolds();
		//print positions of all objects
		for (int j = dynamicsWorld->getNumCollisionObjects() - 1; j >= 0; j--)
		{

			btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[j];
			btRigidBody* body = btRigidBody::upcast(obj);
			btTransform trans;
			if (body && body->getMotionState())
			{
				switch (j)
				{
				case 0: //floor, no motion state needed because it won't move
					trans = obj->getWorldTransform(); break;
				case 1: //box, we have to take the transformation from the motion state
						//because it may have been moved
					pRobot1Controller->calculatePosition(DT);
					body->getMotionState()->getWorldTransform(trans); break;
					
				case 2:	//robot, we have to set ourselves the transformation and communicate it to Bullet
						//through the motion state
					body->getMotionState()->getWorldTransform(trans); break;
				}
			}


			if ((i % 10 == 0) && j!=0)
			{
				printf("\n numOfCollisions =  %d", numCollision); printf("\n");
				printf("world pos object %d = %f,%f,%f\n", j, float(trans.getOrigin().getX()), float(trans.getOrigin().getY()), float(trans.getOrigin().getZ()));
			}
		}
	}
	getchar();

	///-----stepsimulation_end-----

	//cleanup in the reverse order of creation/initialization

	///-----cleanup_start-----

	//remove the rigidbodies from the dynamics world and delete them
	for (i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
	{
		btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState())
		{
			delete body->getMotionState();
		}
		dynamicsWorld->removeCollisionObject(obj);
		delete obj;
	}

	//delete collision shapes
	for (int j = 0; j < collisionShapes.size(); j++)
	{
		btCollisionShape* shape = collisionShapes[j];
		collisionShapes[j] = 0;
		delete shape;
	}

	//delete dynamics world
	delete dynamicsWorld;

	//delete solver
	delete solver;

	//delete broadphase
	delete overlappingPairCache;

	//delete dispatcher
	delete dispatcher;

	delete collisionConfiguration;

	//next line is optional: it will be cleared by the destructor when the array goes out of scope
	collisionShapes.clear();
}

