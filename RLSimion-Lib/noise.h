#pragma once

#include "parameterized-object.h"

class tinyxml2::XMLElement;
class INumericValue;

class CNoise
{
protected:
	CNoise();
	double m_lastValue;
public:
	virtual double getValue()= 0;

	static CNoise* getInstance(tinyxml2::XMLElement* pParameters);
};



class CGaussianNoise: public CNoise, public CParamObject
{
	INumericValue* m_pSigma;
	INumericValue* m_pAlpha;
	INumericValue* m_pScale;
public:
	CGaussianNoise(tinyxml2::XMLElement* pParameters);
	~CGaussianNoise();
	double getSigma(){ return m_pSigma->getValue(); }
	double unscale(double noise){ return noise / m_pScale->getValue(); }

	double getValue();
};
