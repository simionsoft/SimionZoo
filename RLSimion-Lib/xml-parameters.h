#pragma once


class INumericValue
{
public:
	virtual double getValue() = 0;
};

#include <list>

class XMLUtils
{
	static std::list<INumericValue*> m_handlers;
public:
	static int countChildren(tinyxml2::XMLElement* pElement, const char* name= 0);

	static INumericValue* getNumericHandler(tinyxml2::XMLElement* pParameters);

	static bool getConstBoolean(tinyxml2::XMLElement* pParameter);
	static int getConstInteger(tinyxml2::XMLElement* pParameter);
	static double getConstDouble(tinyxml2::XMLElement* pParameter);
	static const char* getConstString(tinyxml2::XMLElement* pParameter);

	static void freeHandlers();
};