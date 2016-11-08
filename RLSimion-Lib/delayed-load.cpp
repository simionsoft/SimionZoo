#include "stdafx.h"
#include "delayed-load.h"
#include "simgod.h"
#include "app.h"

CDeferredLoad::CDeferredLoad(unsigned int loadOrder)
{
	CApp::get()->pSimGod->registerDelayedLoadObj(this,loadOrder);
}

CDeferredLoad::~CDeferredLoad()
{}