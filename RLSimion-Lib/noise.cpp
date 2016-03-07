#include "stdafx.h"
#include "noise.h"
#include "globals.h"
#include "parameters.h"

double getNormalDistributionSample(double mu, double sigma)
{
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

CNoise* CNoise::getInstance(CParameters* pParameters)
{
	const char* pName = pParameters->getChild()->getName();

	if (!strcmp(pName, "GaussianNoise"))
		return new CGaussianNoise(pParameters->getChild());

	assert(0);
	return 0;
}

CGaussianNoise::CGaussianNoise(CParameters* pParameters) : CParamObject(pParameters)
{
	m_pSigma = pParameters->getNumericHandler("Sigma");
	m_pAlpha = pParameters->getNumericHandler("Alpha");
	m_pScale = pParameters->getNumericHandler("Scale");
}

CGaussianNoise::~CGaussianNoise()
{
}

double CGaussianNoise::getValue()
{
	double randValue = 0.0;
	double sigma = m_pSigma->getValue();
	double alpha = m_pAlpha->getValue();

	if (sigma > 0.00000000001)
		randValue = getNormalDistributionSample(0.0,sigma);

	randValue*= m_pScale->getValue();

	randValue = alpha*randValue + (1.0 - alpha)*m_lastValue;

	m_lastValue= randValue;

	return randValue;
}


double CGaussianNoise::getSigma()
{
	return m_pSigma->getValue(); 
}

double CGaussianNoise::unscale(double noise)
{
	return noise / m_pScale->getValue();
}