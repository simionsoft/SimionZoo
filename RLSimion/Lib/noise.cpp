/*
	SimionZoo: A framework for online model-free Reinforcement Learning on continuous
	control problems

	Copyright (c) 2016 SimionSoft. https://github.com/simionsoft

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

#include "noise.h"
#include "config.h"
#include "parameters-numeric.h"
#include "app.h"
#include "worlds/world.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <algorithm>
#include <stdexcept>

#define MARGINAL_SIGMA 0.1
#define MINIMAL_PROBABILITY 0.000001
#define PROBABILITY_INTEGRATION_WIDTH 0.05

double getRandomValue()
{
	return (double)(rand() + 1) / ((double)RAND_MAX + 1);
}

int chooseRandomInteger(vector<double>& probability)
{
	int index = 0;
	double cumProb = 0;
	double randomValue = getRandomValue();

	while (cumProb < randomValue)
		cumProb += probability[index++];

	return index - 1;
}

/// <summary>
/// Returns a sample from a Gaussian distribution function. Used to generate noise
/// </summary>
/// <param name="mean">Mean value of the distribution</param>
/// <param name="sigma">Sigma of the distribution</param>
/// <returns>The sample</returns>
double GaussianNoise::getNormalDistributionSample(double mean, double sigma)
{
	if (sigma == 0.0) return mean;
	double x1 = (double) (rand() + 1) / ((double) RAND_MAX + 1);
	double x2 = (double) rand() / (double) RAND_MAX;
	double z = sqrt(- 2 * log(x1)) * cos(2 * M_PI * x2);
	return z * sigma + mean;
}

double GaussianNoise::getPDF(double mean, double sigma, double value,double scaleFactor)
{
	double diff = (value - mean)/scaleFactor;
	return (1. / sqrt(2 * M_PI*sigma*sigma))*exp(-(diff*diff / (2*sigma*sigma)));
}

double GaussianNoise::getSampleProbability(double mean, double sigma, double sample, double scale)
{
	//https://en.wikipedia.org/wiki/Normal_distribution
	double intwidth = PROBABILITY_INTEGRATION_WIDTH;

	double sample1 = getPDF(mean, sigma, sample + intwidth*0.5, scale);
	double sample2 = getPDF(mean, sigma, sample - intwidth*0.5, scale);

	double prob = fabs(sample1 + sample2)*0.5*intwidth;

	return std::max(MINIMAL_PROBABILITY,prob);
}

double GaussianNoise::getSampleProbability(double sample, bool bUseMarginalNoise)
{
	double sigma;
	if (!bUseMarginalNoise) sigma = m_sigma.get();
	else sigma = MARGINAL_SIGMA;

	return getSampleProbability(0.0, sigma, sample, m_scale->get());
}

Noise::Noise()
{
	m_lastValue = 0.0;
}

std::shared_ptr<Noise> Noise::getInstance(ConfigNode* pConfigNode)
{
	return CHOICE<Noise>(pConfigNode, "Noise", "Noise type",
	{
		{"GaussianNoise",CHOICE_ELEMENT_NEW<GaussianNoise>}
		,{"SinusoidalNoise",CHOICE_ELEMENT_NEW<SinusoidalNoise>}
		,{"Ornstein-Uhlenbeck",CHOICE_ELEMENT_NEW<OrnsteinUhlenbeckNoise>}
	}
	);

	return 0;
}

GaussianNoise::GaussianNoise(ConfigNode* pConfigNode)
{
	m_sigma = DOUBLE_PARAM(pConfigNode, "Sigma", "Width of the gaussian bell",1.0);
	m_alpha = DOUBLE_PARAM(pConfigNode, "Alpha", "Low-pass first-order filter's gain [0...1]. 1=no filter",1.0);
	m_scale = CHILD_OBJECT_FACTORY<NumericValue>(pConfigNode, "Scale", "Scale factor applied to the noise signal before adding it to the policy's output");
}

GaussianNoise::GaussianNoise(double sigma, double alpha, NumericValue* scale)
{
	m_sigma.set(sigma);
	m_alpha.set(alpha);
	m_scale= CHILD_OBJECT_FACTORY<NumericValue>(scale);
}

/// <summary>
/// Returns a sample from a Gaussian distribution function. Used to generate noise
/// </summary>
/// <returns>The sample</returns>
double GaussianNoise::getSample()
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


double GaussianNoise::getVariance()
{
	return pow(m_sigma.get(), 2); 
}

double GaussianNoise::unscale(double noise)
{
	return noise / m_scale->get();
}

SinusoidalNoise::SinusoidalNoise(ConfigNode* pConfigNode)
{
	m_scale = CHILD_OBJECT_FACTORY<NumericValue>(pConfigNode, "Amplitude-Scale", "Scaling factor applied to the sinusoidal");
	m_timeFreq = DOUBLE_PARAM(pConfigNode, "Time-Frequency", "Frequency of the signal in 1/simulation seconds", 0.5);
}

SinusoidalNoise::SinusoidalNoise(NumericValue* scale, double timeFreq)
{
	m_scale = CHILD_OBJECT_FACTORY<NumericValue>(scale);
	m_timeFreq.set(timeFreq);
}

double SinusoidalNoise::getVariance()
{
	return 1.0;
}

double SinusoidalNoise::unscale(double noise)
{
	double width = m_scale->get();
	if (width <= 0) return 0.0;
	return noise / width;
}

/// <summary>
/// Returns a sample from a sinusoidal signal. Used to generate noise
/// </summary>
/// <returns>The sample</returns>
double SinusoidalNoise::getSample()
{
	if (m_scale->get() == 0.0) return 0.0;

	double simTime= SimionApp::get()->pWorld->getTotalSimTime();
	double timeWidth = 1.0 / std::max(0.00001, m_timeFreq.get());

	double slowerTimeWidth = 30;
	double wave = sin(simTime / timeWidth)*sin(simTime / slowerTimeWidth);
	double noise = m_scale->get()* wave;

	return noise;
}

double SinusoidalNoise::getSampleProbability(double sample,bool bUseMarginalNoise)
{
	return 1.0;
}

OrnsteinUhlenbeckNoise::OrnsteinUhlenbeckNoise(ConfigNode* pConfigNode)
{
	//https://en.wikipedia.org/wiki/Ornstein%E2%80%93Uhlenbeck_process
	m_lastValue = 0.0;
	m_mu = DOUBLE_PARAM(pConfigNode, "Mu", "Mean value of the generated noise", 0.0);
	m_sigma = DOUBLE_PARAM(pConfigNode, "Sigma", "Degree of volatility around it caused by shocks", 0.0);
	m_theta = DOUBLE_PARAM(pConfigNode, "Theta", "Rate by which noise shocks dissipate and the variable reverts towards the mean", 0.0);
	m_scale = CHILD_OBJECT_FACTORY<NumericValue>(pConfigNode, "Scale", "Scale factor applied to the noise signal before adding it to the policy's output");

	if (SimionApp::get() != nullptr && SimionApp::get()->pWorld.ptr() != nullptr)
		m_dt = SimionApp::get()->pWorld->getDT();
	else throw std::runtime_error("OrnsteinUhlenbeckNoise initialization problem");
}

OrnsteinUhlenbeckNoise::OrnsteinUhlenbeckNoise(double theta, double sigma, double mu, double dt)
{
	m_lastValue = 0.0;
	m_theta.set(theta);
	m_sigma.set(sigma);
	m_mu.set(mu);
	m_dt = dt;
	m_scale.set(new ConstantValue(1.0));
}

OrnsteinUhlenbeckNoise::~OrnsteinUhlenbeckNoise()
{}

double OrnsteinUhlenbeckNoise::getVariance()
{
	return sqrt(m_sigma.get()*m_sigma.get() /( 2 * m_theta.get()));
}
double OrnsteinUhlenbeckNoise::unscale(double noise)
{
	//does this method really make sense????
	return 1.0;
}

/// <summary>
/// Returns a sample from an Ornstein Uhlenbeck process: https://en.wikipedia.org/wiki/Ornstein%E2%80%93Uhlenbeck_process
/// Used to generate temporally-correlated noise
/// </summary>
/// <returns>The sample</returns>
double OrnsteinUhlenbeckNoise::getSample()
{
	//http://math.stackexchange.com/questions/1287634/implementing-ornstein-uhlenbeck-in-matlab
	//x(i + 1) = x(i) + th*(mean - x(i))*dt + sig*sqrt(dt)*randn;

	double normalDistSample = GaussianNoise::getNormalDistributionSample(0.0, 1);

	double newNoise = m_lastValue + m_theta.get()*(m_mu.get() - m_lastValue)*m_dt
		+ m_sigma.get()*sqrt(m_dt) * normalDistSample;
	m_lastValue = newNoise;
	newNoise *= m_scale->get();

	return newNoise;
}

double OrnsteinUhlenbeckNoise::getSampleProbability(double sample, bool bUseMarginalNoise)
{
	double sigma;
	if (!bUseMarginalNoise) sigma = getVariance();
	else sigma = MARGINAL_SIGMA;

	return GaussianNoise::getSampleProbability(0.0, sigma, sample, m_scale->get());
}