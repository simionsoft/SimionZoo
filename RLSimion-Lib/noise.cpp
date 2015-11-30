#include "stdafx.h"
#include "noise.h"
#include "globals.h"
#include "parameters.h"
#include "parameter.h"

double getNormalDistributionSample(/*double mu, */double sigma)
{
	double x1 = (double) (rand() + 1) / ((double) RAND_MAX + 1);
	double x2 = (double) rand() / (double) RAND_MAX;

	double z = sqrt(- 2 * log(x1)) * cos(2 * M_PI * x2);

	return z * sqrt(sigma) /*+ mu*/;
}
////2015/10/09
//double CGaussianNoise::getLastValuesProbability()
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


CGaussianNoise::CGaussianNoise(int actionDim, CParameters* pParameters)
{
	char paramName[MAX_PARAMETER_NAME_SIZE];
	sprintf_s(paramName,MAX_PARAMETER_NAME_SIZE, "NOISE_WIDTH_%d",actionDim);

	m_pSigma= pParameters->addParameter(CParameter(paramName, 0.0)); //2015/10/09

	m_lastValue= 0.0;
}

CGaussianNoise::~CGaussianNoise()
{
}

double CGaussianNoise::getNewValue()
{
	double randValue = 0.0;
	//if (*m_pWidth > 0.0000000001) //2015/10/09
	if (m_pSigma->getDouble() > 0.00000000001) //2015/10/09
	{
		//standard normal distribution: 99.7% of the samples will be within [-3.0,3.0]
		randValue = getNormalDistributionSample(m_pSigma->getDouble()); //1.0 //2015/10/09

		//randValue= 0.7*randValue + 0.3* m_lastValue;

		//2015/10/09
		//if (randValue<-3.0) randValue+= 3.0;
		//else if (randValue>3.0) randValue-= 3.0; //100% of samples within [-3.0,3.0]
		//randValue*= 0.3333; //100% the samples samples within [-1.0,1.0]
	}
	m_lastValue= randValue;

	return randValue; //2015/10/09   *(*m_pWidth);
}

double CGaussianNoise::getLastValue()
{
	return m_lastValue;// * (*m_pWidth);
}

double CGaussianNoise::getSigma()
{
	return m_pSigma->getDouble();
}