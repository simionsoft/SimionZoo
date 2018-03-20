#include "deferred-load.h"
#include "SimGod.h"
#include "app.h"

DeferredLoad::DeferredLoad(unsigned int loadOrder)
{
	SimGod::registerDeferredLoadStep(this,loadOrder);
}

DeferredLoad::~DeferredLoad()
{}