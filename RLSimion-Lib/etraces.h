#pragma once
#include "parameters.h"
#include "features.h"

class CConfigNode;

class CETraces : public CFeatureList
{
	bool m_bUse;
	DOUBLE_PARAM m_threshold;
	DOUBLE_PARAM m_lambda;
	BOOL_PARAM m_bReplace;
public:
	CETraces(CConfigNode* pConfigNode);
	virtual ~CETraces();

	//traces will be multiplied by factor*lambda
	//traces are automatically cleared if it's the first step of an episode
	void update(double factor = 1.0);

	void addFeatureList(CFeatureList *inList, double factor = 1.0);

	double getLambda(){ return m_lambda.get(); };
};