#pragma once
class CConfigNode;
#include <memory>

class CNumericValue
{
public:
	virtual double get() = 0;

	static std::shared_ptr<CNumericValue> getInstance(CConfigNode* pConfigNode);
};
