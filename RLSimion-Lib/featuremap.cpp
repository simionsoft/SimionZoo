#include "stdafx.h"
#include "featuremap.h"
#include "config.h"
#include "globals.h"
#include "app.h"


CStateFeatureMap::CStateFeatureMap(CConfigNode* pConfigNode)
{
	m_pParameters = pConfigNode;
}

std::shared_ptr<CStateFeatureMap> CStateFeatureMap::getInstance(CConfigNode* pConfigNode)
{
	return CHOICE<CStateFeatureMap>(pConfigNode,"Type", "Feature map type",
	{
		CHOICE_ELEMENT_NEW(pConfigNode,"RBF-State-Grid",CGaussianRBFStateGridFeatureMap);
		}
	});
}
//CLASS_FACTORY(CStateFeatureMap)
//{
//	CHOICE("Type","Feature map type");
//	CHOICE_ELEMENT("RBF-State-Grid", CGaussianRBFStateGridFeatureMap,"Grid on the state-space");
//	END_CHOICE();
//
//	END_CLASS();
//	return 0;
//}

CActionFeatureMap::CActionFeatureMap(CConfigNode* pConfigNode)
{
	m_pParameters = pConfigNode;
}

std::shared_ptr<CActionFeatureMap> CActionFeatureMap::getInstance(CConfigNode* pConfigNode)
{
	return CHOICE<CActionFeatureMap>(pConfigNode, "Type", "Feature map type",
	{
		CHOICE_ELEMENT_NEW(pConfigNode,"RBF-Action-Grid",CGaussianRBFActionGridFeatureMap);
	});
}
//CLASS_FACTORY(CActionFeatureMap)
//{
//	CHOICE("Type", "Feature map type");
//	CHOICE_ELEMENT("RBF-Action-Grid", CGaussianRBFActionGridFeatureMap, "Grid on the action-space");
//	END_CHOICE();
//
//	END_CLASS();
//	return 0;
//}
