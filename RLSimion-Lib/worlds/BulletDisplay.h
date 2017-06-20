#pragma once
#pragma once
#include "../../3rd-party/bullet3-2.86/src/btBulletDynamicsCommon.h"
#include "../../3rd-party/bullet3-2.86/examples/CommonInterfaces/CommonRigidBodyBase.h"

#include "../../3rd-party/bullet3-2.86/examples/Utils/b3Clock.h"
#include "../../3rd-party/bullet3-2.86/examples/OpenGLWindow/SimpleOpenGL3App.h"
#include "../../3rd-party/bullet3-2.86/examples/ExampleBrowser/OpenGLGuiHelper.h"
#include "../../3rd-party/bullet3-2.86/examples/CommonInterfaces/CommonExampleInterface.h"
#include "../../3rd-party/bullet3-2.86/examples/CommonInterfaces/CommonGUIHelperInterface.h"
#include "BulletSoftBody\btSoftRigidDynamicsWorld.h"
#include "BulletSoftBody\btSoftBodyHelpers.h"
#include "BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.h"
#include <stdio.h>
#include <math.h>
#pragma comment(lib,"opengl32.lib")

class BulletGraphic
{

public:
	SimpleOpenGL3App* m_pOpenGLApp;
	OpenGLGuiHelper* m_pHelper;
	// Another option might be to pass the dimensions and text to the constructor
	BulletGraphic()
	{
		m_pOpenGLApp = new SimpleOpenGL3App("Bullet Graphic Interface", 1024, 768, true);
		m_pHelper = new OpenGLGuiHelper(m_pOpenGLApp, false);
	}
	~BulletGraphic();

	virtual void setDebugger(btDiscreteDynamicsWorld* m_dynamicsWorld);
	virtual void setSoftDebugger(btSoftRigidDynamicsWorld* m_dynamicsWorld);

	virtual void renderScene(btDiscreteDynamicsWorld* m_dynamicsWorld);

	virtual void generateGraphics(btDiscreteDynamicsWorld* m_dynamicsWorld);
	
	virtual void updateCamera();
	virtual void resetCamera();
	virtual void swapBuffers();

	void drawDynamicWorld(btDiscreteDynamicsWorld* m_dynamicsWorld);
	void drawSoftWorld(btSoftRigidDynamicsWorld* m_softWorld);
	void drawText3D(char text[], btVector3 &position);

	GUIHelperInterface* BulletGraphic::getGuiHelper();
};