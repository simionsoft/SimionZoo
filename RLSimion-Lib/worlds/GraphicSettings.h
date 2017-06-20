#pragma once
#include "../../3rd-party/bullet3-2.86/src/btBulletDynamicsCommon.h"
#include "../../3rd-party/bullet3-2.86/examples/CommonInterfaces/CommonRigidBodyBase.h"

#include "../../3rd-party/bullet3-2.86/examples/Utils/b3Clock.h"
#include "../../3rd-party/bullet3-2.86/examples/OpenGLWindow/SimpleOpenGL3App.h"
#include "../../3rd-party/bullet3-2.86/examples/ExampleBrowser/OpenGLGuiHelper.h"
#include "../../3rd-party/bullet3-2.86/examples/CommonInterfaces/CommonExampleInterface.h"
#include "../../3rd-party/bullet3-2.86/examples/CommonInterfaces/CommonGUIHelperInterface.h"
#include "BulletSoftBody/btSoftRigidDynamicsWorld.h"
#include "BulletSoftBody/btSoftBodyHelpers.h"
#include "BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.h"
#include <stdio.h>
#include <math.h>
#pragma comment(lib,"opengl32.lib")

class BulletViewer : public CommonRigidBodyBase
{
	

public:
	SimpleOpenGL3App* m_pOpenGLApp;

	BulletViewer(struct GUIHelperInterface* helper)
		:CommonRigidBodyBase(helper)
	{
	}
	~BulletViewer();

	virtual void initBullet();
	virtual void initPhysics();

	virtual void initSoftBullet(btSoftBodyWorldInfo* m_sBodyWorldInfo);
	virtual void initSoftPhysics(btSoftBodyWorldInfo* m_sBodyWorldInfo);

	virtual void generateGraphics(GUIHelperInterface*	helper);
	void setOpenGLApp(SimpleOpenGL3App* app) { m_pOpenGLApp = app; }
	virtual void resetCamera();
	void deleteGraphicOptions();

	void simulate(double dt);
	void draw();
	void drawText3D(char text[], btVector3 &position);

	void connectWithRope(btRigidBody* body1, btRigidBody* body2, btSoftBodyWorldInfo* btInfo);
	btSoftRigidDynamicsWorld* getSoftDynamicsWorld();
	void drawSoftWorld();
	//btSoftBodyWorldInfo* getBtSoftBodyInfo();
	
	btAlignedObjectArray<btCollisionShape*> BulletViewer::getCollisionShape();
	GUIHelperInterface* BulletViewer::getGuiHelper();
};