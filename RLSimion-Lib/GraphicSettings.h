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

class GraphicSettings : public CommonRigidBodyBase
{

public:


	SimpleOpenGL3App* m_pOpenGLApp;

	GraphicSettings(struct GUIHelperInterface* helper)
		:CommonRigidBodyBase(helper)
	{
	}
	~GraphicSettings();
	void setOpenGLApp(SimpleOpenGL3App* app) { m_pOpenGLApp = app; }
	virtual void initPhysics();
	virtual void generateGraphics(GUIHelperInterface*	helper);
	void simulate(double dt);
	void draw();
	void drawText3D(char text[], btVector3 &position);
	btAlignedObjectArray<btCollisionShape*> GraphicSettings::getCollisionShape();
	virtual void initializeGraphicProccess();
	GUIHelperInterface* GraphicSettings::getGuiHelper();
	void deleteGraphicOptions();
	virtual void resetCamera();
};