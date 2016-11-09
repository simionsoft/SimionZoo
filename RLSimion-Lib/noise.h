#pragma once

#include "parameterized-object.h"

class CConfigNode;
class CNumericValue;

double getRandomValue();// returns a random value in range [0,1]

double getNormalDistributionSample(double mu, double sigma);


class CNoise
{
protected:
	CNoise();
	double m_lastValue;
public:
	static CNoise* getInstance(CConfigNode* pParameters);
	virtual ~CNoise() {}

	virtual double getSigma() { return 0.0; }
	virtual double unscale(double noise) { return noise; }
	virtual double getValue()= 0;
};



class CGaussianNoise: public CNoise, public CParamObject
{
	CNumericValue* m_pSigma;
	CNumericValue* m_pAlpha;
	CNumericValue* m_pScale;
public:
	CGaussianNoise(CConfigNode* pParameters);
	virtual ~CGaussianNoise();
	double getSigma();
	double unscale(double noise);

	double getValue();
};
