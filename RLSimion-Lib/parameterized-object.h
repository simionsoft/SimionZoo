#pragma once


class tinyxml2::XMLElement;

class CParamObject
{
protected:
	tinyxml2::XMLElement* m_pParameters;
public:
	CParamObject(tinyxml2::XMLElement* pParameters){ m_pParameters = pParameters; }
	~CParamObject(){}

	tinyxml2::XMLElement* getParameters(){ return m_pParameters; }
};