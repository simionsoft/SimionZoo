#include "stdafx.h"
#include "featuremap.h"
#include "parameters.h"
#include "globals.h"

CFeatureMap* CLASS_FACTORY(CFeatureMap)(CParameters* pParameters)
{
	const char* name = pParameters->getName();
	CHOICE("Type");
	CHOICE_ELEMENT(name, "RBF-State-Grid", CGaussianRBFStateGridFeatureMap, pParameters);
	CHOICE_ELEMENT(name, "RBF-Action-Grid", CGaussianRBFActionGridFeatureMap, pParameters);
	END_CHOICE();

	END_CLASS();
	return 0;
}