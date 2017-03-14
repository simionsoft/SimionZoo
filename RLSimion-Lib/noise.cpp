#include "stdafx.h"
#include "noise.h"
#include "config.h"
#include "parameters-numeric.h"
#include "app-rlsimion.h"

double getRandomValue()
{
	return (double)(rand() + 1) / ((double)RAND_MAX + 1);
}

double getNormalDistributionSample(double mu, double sigma)
{
	if (sigma == 0.0) return 0.0;
	double x1 = (double) (rand() + 1) / ((double) RAND_MAX + 1);
	double x2 = (double) rand() / (double) RAND_MAX;
	assert(x1 != 0.0);
	assert(-2 * log(x1) >= 0.0);
	double z = sqrt(- 2 * log(x1)) * cos(2 * M_PI * x2);
	assert(sigma != 0.0);
	return z * sqrt(sigma) + mu;
}
////2015/10/09
//double CGaussianNoiseVariableSigma::getLastValuesProbability()
//{
//	//https://en.wikipedia.org/wiki/Normal_distribution
//	double intwidth= 0.001;
//	double sample= m_lastValue*3.0;
//
//	double sample1= sample+intwidth*0.5;
//	double sample2= sample-intwidth*0.5;
//	double val1,val2;
//
//	val1= (1/sqrt(2*M_PI))*exp(-(sample1*sample1/2));
//	val2= (1/sqrt(2*M_PI))*exp(-(sample2*sample2/2));
//
//	double prob= fabs(val2-val1)*0.5*intwidth;
//
//	return prob;
//}

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
	}
	);

	return 0;
}

CGaussianNoise::CGaussianNoise(CConfigNode* pConfigNode)
{
	m_pSigma = DOUBLE_PARAM(pConfigNode, "Sigma", "Width of the gaussian bell",1.0);
	m_pAlpha = DOUBLE_PARAM(pConfigNode, "Alpha", "Low-pass first-order filter's gain [0...1]. 1=no filter",1.0);
	m_scale = CHILD_OBJECT_FACTORY<CNumericValue>(pConfigNode, "Scale", "Scale factor applied to the noise signal before adding it to the policy's output");
}


double CGaussianNoise::get()
{
	double randValue = 0.0;
	double sigma = m_pSigma.get();
	double alpha = m_pAlpha.get();

	if (sigma > 0.00000000001)
		randValue = getNormalDistributionSample(0.0,sigma);

	randValue*= m_scale->get();

	randValue = alpha*randValue + (1.0 - alpha)*m_lastValue;

	m_lastValue= randValue;

	return randValue;
}


double CGaussianNoise::getSigma()
{
	return m_pSigma.get(); 
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

double CSinusoidalNoise::get()
{
	if (m_scale->get() == 0.0) return 0.0;

	double simTime= CSimionApp::get()->pWorld->getTotalSimTime();
	double timeWidth = 1.0 / std::max(0.00001, m_timeFreq.get());

	double slowerTimeWidth = 30;
	double wave = sin(simTime / timeWidth)*sin(simTime / slowerTimeWidth);
	double noise = m_scale->get()* wave;

	return noise;
}