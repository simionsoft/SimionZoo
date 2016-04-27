#include "stdafx.h"
#include "delayed-load.h"
#include "simgod.h"
#include "app.h"

CDeferredLoad::CDeferredLoad()
{
	CApp::get()->SimGod.registerDelayedLoadObj(this);
}

CDeferredLoad::~CDeferredLoad()
{}