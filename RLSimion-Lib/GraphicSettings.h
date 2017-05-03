#pragma once
#include "../3rd-party/bullet3-2.86/src/btBulletDynamicsCommon.h"
#include "../3rd-party/bullet3-2.86/examples/CommonInterfaces/CommonRigidBodyBase.h"

#include "../3rd-party/bullet3-2.86/examples/Utils/b3Clock.h"
#include "../3rd-party/bullet3-2.86/examples/OpenGLWindow/SimpleOpenGL3App.h"
#include "../3rd-party/bullet3-2.86/examples/ExampleBrowser/OpenGLGuiHelper.h"
#include "../3rd-party/bullet3-2.86/examples/CommonInterfaces/CommonExampleInterface.h"
#include "../3rd-party/bullet3-2.86/examples/CommonInterfaces/CommonGUIHelperInterface.h"
#include <stdio.h>
#include <math.h>
#pragma comment(lib,"opengl32.lib")

class BulletBuilder : public CommonRigidBodyBase
{

public:
	SimpleOpenGL3App* m_pOpenGLApp;

	BulletBuilder(struct GUIHelperInterface* helper)
		:CommonRigidBodyBase(helper)
	{
	}
	~BulletBuilder();

	virtual void initializeBulletRequirements();
	virtual void initPhysics();

	virtual void generateGraphics(GUIHelperInterface*	helper);
	void setOpenGLApp(SimpleOpenGL3App* app) { m_pOpenGLApp = app; }
	virtual void resetCamera();
	void deleteGraphicOptions();

	void simulate(double dt);
	void draw();
	void drawText3D(char text[], btVector3 &position);
	
	btAlignedObjectArray<btCollisionShape*> BulletBuilder::getCollisionShape();
	GUIHelperInterface* BulletBuilder::getGuiHelper();	
};