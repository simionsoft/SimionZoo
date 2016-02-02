#include "stdafx.h"
#include "featuremap.h"
#include "parameters.h"

CFeatureMap* CFeatureMap::getInstance(CParameters* pParameters)
{
	if (!strcmp(pParameters->getName(), "RBF_GRID"))
		return new CGaussianRBFGridFeatureMap(pParameters);
	return 0;
}