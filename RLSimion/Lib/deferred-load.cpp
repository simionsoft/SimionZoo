#include "deferred-load.h"
#include "simgod.h"
#include "app.h"

DeferredLoad::DeferredLoad(unsigned int loadOrder)
{
	SimGod::registerDeferredLoadStep(this,loadOrder);
}

DeferredLoad::~DeferredLoad()
{}