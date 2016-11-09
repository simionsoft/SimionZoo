#include "stdafx.h"
#include "featuremap.h"
#include "config.h"
#include "globals.h"

CStateFeatureMap::CStateFeatureMap(CConfigNode* pParameters) : CParamObject(pParameters)
{}

CLASS_FACTORY(CStateFeatureMap)
{
	CHOICE("Type","Feature map type");
	CHOICE_ELEMENT("RBF-State-Grid", CGaussianRBFStateGridFeatureMap,"Grid on the state-space");
	END_CHOICE();

	END_CLASS();
	return 0;
}

CActionFeatureMap::CActionFeatureMap(CConfigNode* pParameters) : CParamObject(pParameters)
{}

CLASS_FACTORY(CActionFeatureMap)
{
	CHOICE("Type", "Feature map type");
	CHOICE_ELEMENT("RBF-Action-Grid", CGaussianRBFActionGridFeatureMap, "Grid on the action-space");
	END_CHOICE();

	END_CLASS();
	return 0;
}
