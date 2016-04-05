#include "stdafx.h"
#include "featuremap.h"
#include "parameters.h"
#include "globals.h"

CStateFeatureMap::CStateFeatureMap(CParameters* pParameters) : CParamObject(pParameters)
{}

CLASS_FACTORY(CStateFeatureMap)
{
	CHOICE("Type","Feature map type");
	CHOICE_ELEMENT("RBF-State-Grid", CGaussianRBFStateGridFeatureMap,"Grid on the state-space");
	END_CHOICE();

	END_CLASS();
	return 0;
}

CActionFeatureMap::CActionFeatureMap(CParameters* pParameters) : CParamObject(pParameters)
{}

CLASS_FACTORY(CActionFeatureMap)
{
	CHOICE("Type", "Feature map type");
	CHOICE_ELEMENT("RBF-Action-Grid", CGaussianRBFActionGridFeatureMap, "Grid on the action-space");
	END_CHOICE();

	END_CLASS();
	return 0;
}
