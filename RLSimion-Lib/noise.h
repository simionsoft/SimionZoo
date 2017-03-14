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
	virtual double get()= 0;
};



class CGaussianNoise: public CNoise
{
	DOUBLE_PARAM m_pSigma;
	DOUBLE_PARAM m_pAlpha;
	CHILD_OBJECT_FACTORY<CNumericValue> m_scale;
public:
	CGaussianNoise(CConfigNode* pParameters);
	~CGaussianNoise()= default;
	double getSigma();
	double unscale(double noise);

	double get();
};

class CSinusoidalNoise : public CNoise
{
	CHILD_OBJECT_FACTORY<CNumericValue> m_scale;
	DOUBLE_PARAM m_timeFreq;
public:
	CSinusoidalNoise(CConfigNode* pParameters);
	~CSinusoidalNoise() = default;
	double getSigma();
	double unscale(double noise);
	double get();
};