#pragma once
#include "parameters.h"

class CConfigNode;
class CNumericValue;

double getRandomValue();// returns a random value in range [0,1]


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
	virtual double getSample()= 0;
	//bUseMarginal= true means that the internal parameters should be neglected and, instead
	//, a very thin noise source should be used. This is used to simulate the calculation of
	//the probability of a sample belonging to a deterministic policy
	virtual double getSampleProbability(double sample, bool bUseMarginalNoise= false) = 0;
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
	double getSample();
	double getSampleProbability(double sample, bool bUseMarginalNoise = false);

	static double getSampleProbability(double mean, double sigma, double value, double scale = 1.0);
	static double getNormalDistributionSample(double mean, double sigma);
	static double getPDF(double mean, double sigma, double value,double scaleFactor=1.0);
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
	double getSample();
	double getSampleProbability(double sample, bool bUseMarginalNoise = false) { throw std::exception("not implemented"); }
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
	double getSample();
	double getSampleProbability(double sample, bool bUseMarginalNoise = false) { throw std::exception("not implemented"); }
};