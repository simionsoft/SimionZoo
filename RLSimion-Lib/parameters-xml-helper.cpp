#include "stdafx.h"
#include "parameters-xml-helper.h"



int ParametersXMLHelper::countChildren(tinyxml2::XMLElement* pElement, const char* name)
{
	int count = 0;
	tinyxml2::XMLElement* p = pElement->FirstChildElement(name);

	while (p != 0)
	{
		count++;
		p = p->NextSiblingElement(name);
	}
	return count;
}