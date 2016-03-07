#pragma once

#include "parameterized-object.h"

class CParameters;
class INumericValue;


double getNormalDistributionSample(double mu, double sigma);


class CNoise
{
protected:
	CNoise();
	double m_lastValue;
public:
	virtual double getValue()= 0;

	static CNoise* getInstance(CParameters* pParameters);
};



class CGaussianNoise: public CNoise, public CParamObject
{
	INumericValue* m_pSigma;
	INumericValue* m_pAlpha;
	INumericValue* m_pScale;
public:
	CGaussianNoise(CParameters* pParameters);
	~CGaussianNoise();
	double getSigma();
	double unscale(double noise);

	double getValue();
};
