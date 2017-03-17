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
	DOUBLE_PARAM m_sigma;
	DOUBLE_PARAM m_alpha;
	CHILD_OBJECT_FACTORY<CNumericValue> m_scale;
public:
	CGaussianNoise(CConfigNode* pParameters);
	CGaussianNoise(double sigma, double alpha, CNumericValue* scale);
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
	CSinusoidalNoise(CNumericValue* scale, double timeFreq);
	~CSinusoidalNoise() = default;
	double getSigma();
	double unscale(double noise);
	double get();
};

class COrnsteinUhlenbeckNoise : public CNoise
{
	DOUBLE_PARAM m_theta, m_sigma,m_mu;
	double m_dt;
public:
	COrnsteinUhlenbeckNoise(CConfigNode* pParameters);
	COrnsteinUhlenbeckNoise(double theta, double sigma, double mu, double dt);
	~COrnsteinUhlenbeckNoise();
	double getSigma();
	double unscale(double noise);
	double get();
};