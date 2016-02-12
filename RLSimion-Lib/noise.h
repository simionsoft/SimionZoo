#pragma once

#include "parameterized-object.h"

class tinyxml2::XMLElement;
class INumericValue;

class CGaussianNoise: public CParamObject
{
	double m_lastValue;
	//double *m_pWidth;  //2015/10/09
	INumericValue* m_pSigma; //2015/10/09
	//double getWidth(){if (m_pWidth) return *m_pWidth; else return 0.0;} //2015/10/09
public:
	CGaussianNoise(tinyxml2::XMLElement* pParameters);
	~CGaussianNoise();

	double getNewValue();
	double getLastValue();
	//double getLastValuesProbability();
	double getSigma();
};

