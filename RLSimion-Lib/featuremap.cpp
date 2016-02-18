#include "stdafx.h"
#include "featuremap.h"

class tinyxml2::XMLElement;

CFeatureMap* CFeatureMap::getInstance(tinyxml2::XMLElement* pParameters)
{
	tinyxml2::XMLElement* child;
	child = pParameters->FirstChildElement("RBF-Grid");
	if (child)
		return new CGaussianRBFGridFeatureMap(child);
	return 0;
}