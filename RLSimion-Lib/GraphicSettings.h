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

//	CommonExampleInterface* OpenGLInterface;

public:

	GraphicSettings(struct GUIHelperInterface* helper)
		:CommonRigidBodyBase(helper)
	{
	}
	 ~GraphicSettings() {}
	virtual void initPhysics();
	virtual void generateGraphics(GUIHelperInterface*	helper);
	virtual void updateGLApp(SimpleOpenGL3App* app);
	btAlignedObjectArray<btCollisionShape*> GraphicSettings::getCollisionShape();
	virtual void initializeGraphicProccess();
	GUIHelperInterface* GraphicSettings::getGuiHelper();
	int deteleGraphicOptions(SimpleOpenGL3App* app);
	virtual void resetCamera();
};