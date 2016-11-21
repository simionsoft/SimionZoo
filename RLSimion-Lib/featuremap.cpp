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
		CHOICE_ELEMENT_NEW(pConfigNode,CGaussianRBFStateGridFeatureMap,"RBF-State-Grid","An RBF state-grid","")
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
		CHOICE_ELEMENT_NEW(pConfigNode,CGaussianRBFActionGridFeatureMap,"RBF-Action-Grid","An RBF action-grid","")
	});
}

