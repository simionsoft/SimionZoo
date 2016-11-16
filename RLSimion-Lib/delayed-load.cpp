#include "stdafx.h"
#include "delayed-load.h"
#include "SimGod.h"
#include "app.h"

CDeferredLoad::CDeferredLoad(unsigned int loadOrder)
{
	CSimGod::registerDelayedLoadObj(this,loadOrder);
}

CDeferredLoad::~CDeferredLoad()
{}