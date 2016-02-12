#pragma once


class INumericValue
{
public:
	virtual double get() = 0;
};



class XMLParameters
{
public:
	static int countChildren(tinyxml2::XMLElement* pElement, const char* name);

	static INumericValue* getNumericHandler(tinyxml2::XMLElement* pParameters);
};