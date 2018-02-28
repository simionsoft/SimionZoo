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
		{"Discrete-State-Grid", CHOICE_ELEMENT_NEW<CDiscreteStateFeatureMap>},
		{"RBF-State-Grid", CHOICE_ELEMENT_NEW<CGaussianRBFStateGridFeatureMap>},
		{ "Tile-Coding-State-Grid", CHOICE_ELEMENT_NEW<CTileCodingStateFeatureMap>},
		{"State-Bag", CHOICE_ELEMENT_NEW<CBagStateFeatureMap>}
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
		{"Discrete-Action-Grid", CHOICE_ELEMENT_NEW<CDiscreteActionFeatureMap>},
		{"RBF-Action-Grid", CHOICE_ELEMENT_NEW<CGaussianRBFActionGridFeatureMap>},
		{"Tile-Coding-Action-Grid", CHOICE_ELEMENT_NEW<CTileCodingActionFeatureMap>},
		{"Action-Bag", CHOICE_ELEMENT_NEW<CBagActionFeatureMap>}
	});
}

