#pragma once
class CParameters;

class CNumericValue
{
	//static std::list<INumericValue*> m_handlers;
public:
	virtual double getValue() = 0;

	static CNumericValue* getInstance(CParameters* pParameters, const char* parameterName);
};
