#include "stdafx.h"
#include "noise.h"
#include "config.h"
#include "parameters-numeric.h"
#include "app-rlsimion.h"
#include <math.h>

#define MARGINAL_SIGMA 0.1
#define MINIMAL_PROBABILITY 0.000001
#define PROBABILITY_INTEGRATION_WIDTH 0.001

double getRandomValue()
{
	return (double)(rand() + 1) / ((double)RAND_MAX + 1);
}

double CGaussianNoise::getNormalDistributionSample(double mean, double sigma)
{
	if (sigma == 0.0) return 0.0;
	double x1 = (double) (rand() + 1) / ((double) RAND_MAX + 1);
	double x2 = (double) rand() / (double) RAND_MAX;
	double z = sqrt(- 2 * log(x1)) * cos(2 * M_PI * x2);
	return z * sqrt(sigma) + mean;
}

double CGaussianNoise::getPDF(double mean, double sigma, double value,double scaleFactor)
{
	double diff = (value - mean)/scaleFactor;
	return (1. / sqrt(2 * M_PI*sigma*sigma))*exp(-(diff*diff / 2*sigma*sigma));
}

double CGaussianNoise::getSampleProbability(double mean, double sigma, double sample, double scale)
{
	//https://en.wikipedia.org/wiki/Normal_distribution
	double intwidth = PROBABILITY_INTEGRATION_WIDTH;

	double sample1 = getPDF(0.0, sigma, sample + intwidth*0.5, scale);
	double sample2 = getPDF(0.0, sigma, sample - intwidth*0.5, scale);

	double prob = fabs(sample1 - sample2)*0.5*intwidth;

	return std::max(MINIMAL_PROBABILITY,prob);
}

double CGaussianNoise::getSampleProbability(double sample, bool bUseMarginalNoise)
{
	double sigma;
	if (!bUseMarginalNoise) sigma = m_sigma.get();
	else sigma = MARGINAL_SIGMA;

	return getSampleProbability(0.0, sigma, sample, m_scale->get());
}

CNoise::CNoise()
{
	m_lastValue = 0.0;
}

std::shared_ptr<CNoise> CNoise::getInstance(CConfigNode* pConfigNode)
{
	return CHOICE<CNoise>(pConfigNode, "Noise", "Noise type",
	{
		{"GaussianNoise",CHOICE_ELEMENT_NEW<CGaussianNoise>}
		,{"SinusoidalNoise",CHOICE_ELEMENT_NEW<CSinusoidalNoise>}
		,{"Ornstein-Uhlenbeck",CHOICE_ELEMENT_NEW<COrnsteinUhlenbeckNoise>}
	}
	);

	return 0;
}

CGaussianNoise::CGaussianNoise(CConfigNode* pConfigNode)
{
	m_sigma = DOUBLE_PARAM(pConfigNode, "Sigma", "Width of the gaussian bell",1.0);
	m_alpha = DOUBLE_PARAM(pConfigNode, "Alpha", "Low-pass first-order filter's gain [0...1]. 1=no filter",1.0);
	m_scale = CHILD_OBJECT_FACTORY<CNumericValue>(pConfigNode, "Scale", "Scale factor applied to the noise signal before adding it to the policy's output");
}

CGaussianNoise::CGaussianNoise(double sigma, double alpha, CNumericValue* scale)
{
	m_sigma.set(sigma);
	m_alpha.set(alpha);
	m_scale= CHILD_OBJECT_FACTORY<CNumericValue>(scale);
}

double CGaussianNoise::getSample()
{
	double randValue = 0.0;
	double sigma = m_sigma.get();
	double alpha = m_alpha.get();

	if (sigma > 0.00000000001)
		randValue = getNormalDistributionSample(0.0,sigma);

	randValue*= m_scale->get();

	randValue = alpha*randValue + (1.0 - alpha)*m_lastValue;

	m_lastValue= randValue;

	return randValue;
}


double CGaussianNoise::getSigma()
{
	return m_sigma.get(); 
}

double CGaussianNoise::unscale(double noise)
{
	return noise / m_scale->get();
}

CSinusoidalNoise::CSinusoidalNoise(CConfigNode* pConfigNode)
{
	m_scale = CHILD_OBJECT_FACTORY<CNumericValue>(pConfigNode, "Amplitude-Scale", "Scaling factor applied to the sinusoidal");
	m_timeFreq = DOUBLE_PARAM(pConfigNode, "Time-Frequency", "Frequency of the signal in 1/simulation seconds", 0.5);
}

CSinusoidalNoise::CSinusoidalNoise(CNumericValue* scale, double timeFreq)
{
	m_scale = CHILD_OBJECT_FACTORY<CNumericValue>(scale);
	m_timeFreq.set(timeFreq);
}

double CSinusoidalNoise::getSigma()
{
	return 1.0;
}

double CSinusoidalNoise::unscale(double noise)
{
	double width = m_scale->get();
	if (width <= 0) return 0.0;
	return noise / width;
}

double CSinusoidalNoise::getSample()
{
	if (m_scale->get() == 0.0) return 0.0;

	double simTime= CSimionApp::get()->pWorld->getTotalSimTime();
	double timeWidth = 1.0 / std::max(0.00001, m_timeFreq.get());

	double slowerTimeWidth = 30;
	double wave = sin(simTime / timeWidth)*sin(simTime / slowerTimeWidth);
	double noise = m_scale->get()* wave;

	return noise;
}

COrnsteinUhlenbeckNoise::COrnsteinUhlenbeckNoise(CConfigNode* pParameters)
{
	//https://en.wikipedia.org/wiki/Ornstein%E2%80%93Uhlenbeck_process
	m_lastValue = 0.0;
	m_mu = DOUBLE_PARAM(pParameters, "Mu", "Mean value of the generated noise", 0.0);
	m_sigma = DOUBLE_PARAM(pParameters, "Sigma", "Degree of volatility around it caused by shocks", 0.0);
	m_theta = DOUBLE_PARAM(pParameters, "Theta", "Rate by which noise shocks dissipate and the variable reverts towards the mean", 0.0);

	if (CSimionApp::get() != nullptr && CSimionApp::get()->pWorld.ptr() != nullptr)
		m_dt = CSimionApp::get()->pWorld->getDT();
	else throw std::exception("COrnsteinUhlenbeckNoise initialization problem");
}

COrnsteinUhlenbeckNoise::COrnsteinUhlenbeckNoise(double theta, double sigma, double mu, double dt)
{
	m_theta.set(theta);
	m_sigma.set(sigma);
	m_mu.set(mu);
	m_dt = dt;
}

COrnsteinUhlenbeckNoise::~COrnsteinUhlenbeckNoise()
{}

double COrnsteinUhlenbeckNoise::getSigma()
{
	return sqrt(m_sigma.get()*m_sigma.get() /( 2 * m_theta.get()));
}
double COrnsteinUhlenbeckNoise::unscale(double noise)
{
	//does this method really make sense????
	return 1.0;
}
double COrnsteinUhlenbeckNoise::getSample()
{
	//http://math.stackexchange.com/questions/1287634/implementing-ornstein-uhlenbeck-in-matlab
	//x(i + 1) = x(i) + th*(mean - x(i))*dt + sig*sqrt(dt)*randn;

	double newNoise = m_lastValue + m_theta.get()*(m_mu.get() - m_lastValue)*m_dt
		+ m_sigma.get()*sqrt(m_dt)*CGaussianNoise::getNormalDistributionSample(0.0, 1);
	m_lastValue = newNoise;
	return newNoise;
}