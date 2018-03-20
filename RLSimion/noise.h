#pragma once
#include "parameters.h"

class ConfigNode;
class NumericValue;

double getRandomValue();// returns a random value in range [0,1]
int chooseRandomInteger(vector<double>& probability); //returns an integer in range [0, probability.size] according to the given probability

class Noise
{
protected:
	Noise();
	double m_lastValue;
public:
	static std::shared_ptr<Noise> getInstance(ConfigNode* pParameters);
	virtual ~Noise() {}

	virtual double getVariance() = 0;
	virtual double unscale(double noise) { return noise; }
	virtual double getSample()= 0;
	//bUseMarginal= true means that the internal parameters should be neglected and, instead
	//, a very thin noise source should be used. This is used to simulate the calculation of
	//the probability of a sample belonging to a deterministic policy
	virtual double getSampleProbability(double sample, bool bUseMarginalNoise= false) = 0;
};



class GaussianNoise: public Noise
{
	DOUBLE_PARAM m_sigma;
	DOUBLE_PARAM m_alpha;
	CHILD_OBJECT_FACTORY<NumericValue> m_scale;
public:
	GaussianNoise(ConfigNode* pParameters);
	GaussianNoise(double sigma, double alpha, NumericValue* scale);
	~GaussianNoise()= default;
	double getVariance();
	double unscale(double noise);
	double getSample();
	double getSampleProbability(double sample, bool bUseMarginalNoise = false);

	static double getSampleProbability(double mean, double sigma, double value, double scale = 1.0);
	static double getNormalDistributionSample(double mean, double sigma);
	static double getPDF(double mean, double sigma, double value,double scaleFactor=1.0);
};

class SinusoidalNoise : public Noise
{
	CHILD_OBJECT_FACTORY<NumericValue> m_scale;
	DOUBLE_PARAM m_timeFreq;
public:
	SinusoidalNoise(ConfigNode* pParameters);
	SinusoidalNoise(NumericValue* scale, double timeFreq);
	~SinusoidalNoise() = default;
	double getVariance();
	double unscale(double noise);
	double getSample();
	double getSampleProbability(double sample, bool bUseMarginalNoise = false);
};

class OrnsteinUhlenbeckNoise : public Noise
{
	DOUBLE_PARAM m_theta, m_sigma,m_mu;
	CHILD_OBJECT_FACTORY<NumericValue> m_scale;
	double m_dt;
public:
	OrnsteinUhlenbeckNoise(ConfigNode* pParameters);
	OrnsteinUhlenbeckNoise(double theta, double sigma, double mu, double dt);
	~OrnsteinUhlenbeckNoise();
	double getVariance();
	double unscale(double noise);
	double getSample();
	double getSampleProbability(double sample, bool bUseMarginalNoise = false);
};