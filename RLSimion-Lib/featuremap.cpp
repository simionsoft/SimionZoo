#include "stdafx.h"
#include "featuremap.h"
#include "parameters.h"


CFeatureMap* CFeatureMap::getInstance(CParameters* pParameters)
{
	CParameters* child;
	child = pParameters->getChild("RBF-State-Grid");
	if (child)
		return new CGaussianRBFGridFeatureMap(child);
	child = pParameters->getChild("RBF-Action-Grid");
	if (child)
		return new CGaussianRBFGridFeatureMap(child);
	return 0;
}