#include "stdafx.h"
#include "featuremap.h"
#include "parameters.h"
#include "globals.h"

CFeatureMap::CFeatureMap(CParameters* pParameters) : CParamObject(pParameters)
{}

CLASS_FACTORY(CFeatureMap)
{
	CHOICE("Type","Feature map type");
	CHOICE_ELEMENT("RBF-State-Grid", CGaussianRBFStateGridFeatureMap,"Grid on the state-space");
	CHOICE_ELEMENT("RBF-Action-Grid", CGaussianRBFActionGridFeatureMap,"Grid on the action-space");
	END_CHOICE();

	END_CLASS();
	return 0;
}