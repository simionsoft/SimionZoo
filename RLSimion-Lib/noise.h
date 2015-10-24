#pragma once

class CParameters;


class CGaussianNoise
{
	double m_lastValue;
	//double *m_pWidth;  //2015/10/09
	double *m_pSigma; //2015/10/09
	//double getWidth(){if (m_pWidth) return *m_pWidth; else return 0.0;} //2015/10/09
public:
	CGaussianNoise(int actionDim, CParameters* pParameters);
	~CGaussianNoise();

	double getNewValue();
	double getLastValue();
	//double getLastValuesProbability();
	double getSigma(){return *m_pSigma;}
};

