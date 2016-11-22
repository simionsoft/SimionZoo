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
	return CHOICE<CStateFeatureMap>(pConfigNode,"Type", "Feature map type",
	{
		{"RBF-State-Grid",CHOICE_ELEMENT_NEW<CGaussianRBFStateGridFeatureMap>}
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
		{"RBF-Action-Grid",CHOICE_ELEMENT_NEW<CGaussianRBFActionGridFeatureMap>}
	});
}

