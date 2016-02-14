#include "stdafx.h"
#include "featuremap.h"
#include "parameters.h"

CFeatureMap* CFeatureMap::getInstance(tinyxml2::XMLElement* pParameters)
{
	tinyxml2::XMLElement* child;
	child = pParameters->FirstChildElement("RBF_GRID");
	if (child)
		return new CGaussianRBFGridFeatureMap(child);
	return 0;
}