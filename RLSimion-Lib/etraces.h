#pragma once
#include "features.h"

class CConfigNode;

class CETraces : public CFeatureList
{
	bool m_bUse;
	double m_threshold;
	double m_lambda;

public:
	CETraces(CConfigNode* pParameters, const char* name);
	virtual ~CETraces();

	//traces will be multiplied by factor*lambda
	//traces are automatically cleared if it's the first step of an episode
	void update(double factor = 1.0);

	void addFeatureList(CFeatureList *inList, double factor = 1.0);

	double getLambda(){ return m_lambda; };
};