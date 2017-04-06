// NoiseSanityCheck.cpp : Defines the entry point for the console application.
//

#include "../../../RLSimion-Lib/noise.h"
#include "../../../RLSimion-Lib/parameters-numeric.h"
#include <iostream>
#include <math.h>
int main()
{
#define NUM_SAMPLES 10000

#define THETA 10.2 //return rate to the mean value
#define SIGMA 0.2 //volatility
#define MU 0.0 //mean value

	
	COrnsteinUhlenbeckNoise brownianNoise(THETA,SIGMA, MU,0.01);
	double sum = 0.0, avg, max= numeric_limits<double>::min(),min= numeric_limits<double>::max();
	double *pNoiseSamples = new double[NUM_SAMPLES];
	for (int i = 0; i < NUM_SAMPLES; i++)
	{
		pNoiseSamples[i] = brownianNoise.getSample();
		sum += pNoiseSamples[i];
		if (pNoiseSamples[i] > max)max = pNoiseSamples[i];
		if (pNoiseSamples[i] < min) min = pNoiseSamples[i];
		std::cout << pNoiseSamples[i] << "\n";
	}
	avg = sum / NUM_SAMPLES;
	double avgDiff;
	double cuadDiffSum = 0.0;
	for (int i = 0; i < NUM_SAMPLES; i++)
	{
		avgDiff = pNoiseSamples[i] - avg;
		cuadDiffSum += avgDiff*avgDiff;
	}
	double stdDev = sqrt((1.0 / (double)NUM_SAMPLES)*cuadDiffSum);
	std::cout << "AVG: " << avg << "\n";
	std::cout << "STD.DEV: " << stdDev << "\n";
	std::cout << "Expected STD.DEV: " << sqrt(SIGMA*SIGMA/ (2.0 * THETA)) << "\n";
	std::cout << "MAX :" << max << "\n";
	std::cout << "MIN :" << min << "\n";
	delete[] pNoiseSamples;
	getchar();
    return 0;
}

