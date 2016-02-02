#pragma once

class CParameters;
class CParameter;

#include "parameterized-object.h"

class CGaussianNoise: public CParamObject
{
	double m_lastValue;
	//double *m_pWidth;  //2015/10/09
	CParameter* m_pSigma; //2015/10/09
	//double getWidth(){if (m_pWidth) return *m_pWidth; else return 0.0;} //2015/10/09
public:
	CGaussianNoise(CParameters* pParameters);
	~CGaussianNoise();

	double getNewValue();
	double getLastValue();
	//double getLastValuesProbability();
	double getSigma();
};

