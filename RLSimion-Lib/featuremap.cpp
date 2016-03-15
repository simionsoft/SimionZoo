#include "stdafx.h"
#include "featuremap.h"
#include "parameters.h"
#include "globals.h"

CFeatureMap* CLASS_FACTORY(CFeatureMap)(CParameters* pParameters)
{

	CHOICE("Feature-Map");
	CHOICE_ELEMENT("RBF-State-Grid", CGaussianRBFStateGridFeatureMap);
	CHOICE_ELEMENT("RBF-Action-Grid", CGaussianRBFActionGridFeatureMap);
	END_CHOICE();

	END_CLASS();
	return 0;
}