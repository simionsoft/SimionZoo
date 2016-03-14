#include "stdafx.h"
#include "featuremap.h"
#include "parameters.h"
#include "globals.h"

CFeatureMap* CLASS_FACTORY(CFeatureMap)(CParameters* pParameters)
{
	CParameters* pChild = pParameters->getChild();
	const char* type = pChild->getName();

	CHOICE("Type");
	CHOICE_ELEMENT(type, "RBF-State-Grid", CGaussianRBFStateGridFeatureMap, pChild);
	CHOICE_ELEMENT(type, "RBF-Action-Grid", CGaussianRBFActionGridFeatureMap, pChild);
	END_CHOICE();

	END_CLASS();
	return 0;
}