#pragma once
class CConfigNode;

class CNumericValue
{
public:
	virtual double getValue() = 0;

	static CNumericValue* getInstance(CConfigNode* pParameters);
};
