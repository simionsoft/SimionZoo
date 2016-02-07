#pragma once
#include "features.h"
#include "parameterized-object.h"

class CETraces : public CFeatureList, public CParamObject
{
public:
	CETraces(CParameters* pParameters);
	~CETraces();

	//traces will be multiplied by factor*lambda
	//traces are automatically cleared if it's the first step of an episode
	void update(double factor = 1.0);

	void addFeatureList(CFeatureList *inList, double factor = 1.0);

};