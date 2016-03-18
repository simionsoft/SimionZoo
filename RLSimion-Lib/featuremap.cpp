#include "stdafx.h"
#include "featuremap.h"
#include "parameters.h"
#include "globals.h"

CLASS_FACTORY(CFeatureMap)
{

	CHOICE("Feature-Map");
	CHOICE_ELEMENT("RBF-State-Grid", CGaussianRBFStateGridFeatureMap);
	CHOICE_ELEMENT("RBF-Action-Grid", CGaussianRBFActionGridFeatureMap);
	END_CHOICE();

	END_CLASS();
	return 0;
}