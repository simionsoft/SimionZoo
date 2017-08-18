#include "stdafx.h"
#include "featuremap.h"
#include "config.h"
#include "app.h"


CStateFeatureMap::CStateFeatureMap(CConfigNode* pConfigNode)
{
	m_pParameters = pConfigNode;
}

std::shared_ptr<CStateFeatureMap> CStateFeatureMap::getInstance(CConfigNode* pConfigNode)
{
	return CHOICE<CStateFeatureMap>(pConfigNode, "Type", "Feature map type",
	{
		{"Linear-State-Grid", CHOICE_ELEMENT_NEW<CLinearStateFeatureMap>},
		{"RBF-State-Grid", CHOICE_ELEMENT_NEW<CGaussianRBFStateGridFeatureMap>},
		{"Tile-Coding-State-Grid", CHOICE_ELEMENT_NEW<CTileCodingStateFeatureMap>}
	});
}


CActionFeatureMap::CActionFeatureMap(CConfigNode* pConfigNode)
{
	m_pParameters = pConfigNode;
}

std::shared_ptr<CActionFeatureMap> CActionFeatureMap::getInstance(CConfigNode* pConfigNode)
{
	return CHOICE<CActionFeatureMap>(pConfigNode, "Type", "Feature map type",
	{
		{"Linear-Action-Grid", CHOICE_ELEMENT_NEW<CLinearActionFeatureMap>},
		{"RBF-Action-Grid", CHOICE_ELEMENT_NEW<CGaussianRBFActionGridFeatureMap>},
		{"Tile-Coding-Action-Grid", CHOICE_ELEMENT_NEW<CTileCodingActionFeatureMap>}
	});
}

