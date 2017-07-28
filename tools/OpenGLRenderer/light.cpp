#include "stdafx.h"
#include "light.h"
#include "xml-load.h"



CLight::CLight(tinyxml2::XMLElement* pNode)
{
	tinyxml2::XMLElement* pChild= pNode->FirstChildElement(XML_TAG_COLOR);
	if (pChild)
	{
		XML::load(pChild, m_diffuseColor);
		m_ambientColor = m_diffuseColor;
		m_specularColor = m_diffuseColor;
	}
	pChild = pNode->FirstChildElement(XML_TAG_DIFFUSE);
	if (pChild) XML::load(pChild, m_diffuseColor);
	pChild = pNode->FirstChildElement(XML_TAG_AMBIENT);
	if (pChild) XML::load(pChild, m_ambientColor);
	pChild = pNode->FirstChildElement(XML_TAG_SPECULAR);
	if (pChild) XML::load(pChild, m_specularColor);
}

CLight::~CLight()
{
}

void CLight::enable(bool enable)
{
	if (enable) glEnable(GL_LIGHTING);
	else glDisable(GL_LIGHTING);
}

CLight* CLight::getInstance(tinyxml2::XMLElement* pNode)
{
	tinyxml2::XMLElement* pChild = pNode->FirstChildElement();
	if (!strcmp(pChild->Name(), XML_TAG_DIR_LIGHT))
		return new CDirectionalLight(pChild);
	return nullptr;
}

CDirectionalLight::CDirectionalLight(tinyxml2::XMLElement* pNode):CLight(pNode)
{
	tinyxml2::XMLElement* pChild = pNode->FirstChildElement(XML_TAG_DIRECTION);
	if (pChild)
	{
		XML::load(pChild,m_direction);
	}
}

void CDirectionalLight::set()
{
	int lightId = GL_LIGHT0 + m_id;
	glEnable(lightId);

	float lightDir[4];
	//w=0 because it's a directional light
	lightDir[0] = (float)m_direction.x(); lightDir[1] = (float)m_direction.y();
	lightDir[2] = (float) m_direction.z(); lightDir[3] = 0.0f;
	glLightfv(lightId, GL_POSITION, lightDir);
	glLightfv(lightId, GL_DIFFUSE, m_diffuseColor.rgba());
	glLightfv(lightId, GL_AMBIENT, m_ambientColor.rgba());
	glLightfv(lightId, GL_SPECULAR, m_specularColor.rgba());
}