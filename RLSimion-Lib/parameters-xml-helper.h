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

	static bool getBoolean(tinyxml2::XMLElement* pParameters);
	static int getConstInteger(tinyxml2::XMLElement* pParameters);
	static double getConstDouble(tinyxml2::XMLElement* pParameters);
};