#pragma once
#include "parameters.h"

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
	static std::shared_ptr<CNoise> getInstance(CConfigNode* pParameters);
	virtual ~CNoise() {}

	virtual double getSigma() { return 0.0; }
	virtual double unscale(double noise) { return noise; }
	virtual double getValue()= 0;
};



class CGaussianNoise: public CNoise
{
	CHILD_OBJECT_FACTORY<CNumericValue> m_pSigma;
	CHILD_OBJECT_FACTORY<CNumericValue> m_pAlpha;
	CHILD_OBJECT_FACTORY<CNumericValue> m_pScale;
public:
	CGaussianNoise(CConfigNode* pParameters);
	~CGaussianNoise()= default;
	double getSigma();
	double unscale(double noise);

	double getValue();
};
