#include "featuremap.h"
#include "config.h"
#include "app.h"


StateFeatureMap::StateFeatureMap(ConfigNode* pConfigNode)
{
	m_pParameters = pConfigNode;
}

std::shared_ptr<StateFeatureMap> StateFeatureMap::getInstance(ConfigNode* pConfigNode)
{
	return CHOICE<StateFeatureMap>(pConfigNode, "Type", "Feature map type",
	{
		{"Discrete-State-Grid", CHOICE_ELEMENT_NEW<DiscreteStateFeatureMap>},
		{"RBF-State-Grid", CHOICE_ELEMENT_NEW<GaussianRBFStateGridFeatureMap>},
		{ "Tile-Coding-State-Grid", CHOICE_ELEMENT_NEW<TileCodingStateFeatureMap>},
		{"State-Bag", CHOICE_ELEMENT_NEW<BagStateFeatureMap>}
	});
}


ActionFeatureMap::ActionFeatureMap(ConfigNode* pConfigNode)
{
	m_pParameters = pConfigNode;
}

std::shared_ptr<ActionFeatureMap> ActionFeatureMap::getInstance(ConfigNode* pConfigNode)
{
	return CHOICE<ActionFeatureMap>(pConfigNode, "Type", "Feature map type",
	{
		{"Discrete-Action-Grid", CHOICE_ELEMENT_NEW<DiscreteActionFeatureMap>},
		{"RBF-Action-Grid", CHOICE_ELEMENT_NEW<GaussianRBFActionGridFeatureMap>},
		{"Tile-Coding-Action-Grid", CHOICE_ELEMENT_NEW<TileCodingActionFeatureMap>},
		{"Action-Bag", CHOICE_ELEMENT_NEW<BagActionFeatureMap>}
	});
}

