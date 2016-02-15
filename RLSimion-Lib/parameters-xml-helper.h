#pragma once


class INumericValue
{
public:
	virtual double getValue() = 0;
};



class XMLParameters
{
public:
	static int countChildren(tinyxml2::XMLElement* pElement, const char* name= 0);

	static INumericValue* getNumericHandler(tinyxml2::XMLElement* pParameters);
};