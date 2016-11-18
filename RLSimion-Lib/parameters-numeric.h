#pragma once
class CConfigNode;
#include <memory>

class CNumericValue
{
public:
	virtual double getValue() = 0;

	static std::shared_ptr<CNumericValue> getInstance(CConfigNode* pConfigNode);
};
