#include "stdafx.h"
#include "material.h"
#include "xml-load-utils.h"
#include "renderer.h"
#include "texture-manager.h"

CMaterial::CMaterial()
{
}


CMaterial::~CMaterial()
{
}

CMaterial* CMaterial::getInstance(tinyxml2::XMLElement* pNode)
{
	if (!strcmp(pNode->Name(), XML_TAG_SIMPLE_TL_MATERIAL))
		return new CSimpleTLMaterial(pNode);
	return nullptr;
}

CSimpleTLMaterial::CSimpleTLMaterial(tinyxml2::XMLElement* pNode)
{
	tinyxml2::XMLElement *pChild, *pChild2;
	pChild = pNode->FirstChildElement(XML_TAG_AMBIENT);
	if (pChild != nullptr) m_ambient.load(pChild);
	pChild = pNode->FirstChildElement(XML_TAG_DIFFUSE);
	if (pChild != nullptr) m_diffuse.load(pChild);
	pChild = pNode->FirstChildElement(XML_TAG_SPECULAR);
	if (pChild != nullptr) m_specular.load(pChild);
	pChild = pNode->FirstChildElement(XML_TAG_EMISSION);
	if (pChild != nullptr) m_emission.load(pChild);
	pChild = pNode->FirstChildElement(XML_TAG_SHININESS);
	if (pChild != nullptr) m_shininess = atof(pChild->GetText());
	pChild = pNode->FirstChildElement(XML_TAG_TEXTURE);
	if (pChild != nullptr)
	{
		pChild2 = pChild->FirstChildElement(XML_TAG_PATH);
		if (pChild2!=nullptr)
			m_textureId = CRenderer::get()->getTextureManager()->loadTexture(pChild2->GetText());
	}
}

void CSimpleTLMaterial::set()
{
	CRenderer::get()->getTextureManager()->set(m_textureId);

	glShadeModel(GL_SMOOTH);

	glMaterialfv(GL_FRONT, GL_AMBIENT, m_ambient.rgba());
	glMaterialfv(GL_FRONT, GL_DIFFUSE, m_diffuse.rgba());
	glMaterialfv(GL_FRONT, GL_SPECULAR, m_specular.rgba());
	glMaterialf(GL_FRONT, GL_SHININESS, (float)m_shininess);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}

CSimpleTLMaterial::CSimpleTLMaterial(Color ambient, Color diffuse, Color specular, Color emission
	, double shininess, string texture)
	:m_ambient(ambient), m_diffuse(diffuse), m_specular(specular), m_emission(emission)
	, m_shininess(shininess)
{
	m_textureId= CRenderer::get()->getTextureManager()->loadTexture(texture);
}

CSimpleTLMaterial::CSimpleTLMaterial(string texturePath)
{
	m_textureId= CRenderer::get()->getTextureManager()->loadTexture(texturePath);
}


void CColorMaterial::set()
{
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glColor4fv(m_color.rgba());
}

void CLineMaterial::set()
{
//	glEnable(GL_LINE_SMOOTH);
	glLineWidth(m_width);
	glColor4fv(m_color.rgba());
}