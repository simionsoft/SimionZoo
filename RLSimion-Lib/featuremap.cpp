#include "stdafx.h"
#include "featuremap.h"
#include "parameters.h"

class CParameters;

CFeatureMap* CFeatureMap::getInstance(CParameters* pParameters)
{
	CParameters* child;
	child = pParameters->getChild("RBF-Grid");
	if (child)
		return new CGaussianRBFGridFeatureMap(child);
	return 0;
}