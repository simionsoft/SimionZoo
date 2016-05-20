#include "stdafx.h"
#include "delayed-load.h"
#include "simgod.h"
#include "app.h"

CDeferredLoad::CDeferredLoad(unsigned int loadOrder)
{
	CApp::get()->SimGod.registerDelayedLoadObj(this,loadOrder);
}

CDeferredLoad::~CDeferredLoad()
{}