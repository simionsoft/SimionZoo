#include "deferred-load.h"
#include "SimGod.h"
#include "app.h"

CDeferredLoad::CDeferredLoad(unsigned int loadOrder)
{
	CSimGod::registerDeferredLoadStep(this,loadOrder);
}

CDeferredLoad::~CDeferredLoad()
{}