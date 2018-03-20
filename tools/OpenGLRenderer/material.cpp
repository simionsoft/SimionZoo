#include "stdafx.h"
#include "material.h"
#include "xml-load.h"
#include "renderer.h"
#include "texture-manager.h"

Material::Material()
{
}


Material::~Material()
{
}

Material* Material::getInstance(tinyxml2::XMLElement* pNode)
{
	if (!strcmp(pNode->Name(), XML_TAG_SIMPLE_TL_MATERIAL))
		return new SimpleTLMaterial(pNode);
	if (!strcmp(pNode->Name(), XML_TAG_TRANSLUCENT_MATERIAL))
		return new TranslucentMaterial(pNode);
	return nullptr;
}

SimpleTLMaterial::SimpleTLMaterial(tinyxml2::XMLElement* pNode)
{
	tinyxml2::XMLElement *pChild, *pChild2;
	pChild = pNode->FirstChildElement(XML_TAG_AMBIENT);
	if (pChild != nullptr) XML::load(pChild,m_ambient);
	pChild = pNode->FirstChildElement(XML_TAG_DIFFUSE);
	if (pChild != nullptr) XML::load(pChild, m_diffuse);
	pChild = pNode->FirstChildElement(XML_TAG_SPECULAR);
	if (pChild != nullptr) XML::load(pChild, m_specular);
	pChild = pNode->FirstChildElement(XML_TAG_EMISSION);
	if (pChild != nullptr) XML::load(pChild, m_emission);
	pChild = pNode->FirstChildElement(XML_TAG_SHININESS);
	if (pChild != nullptr) m_shininess = atof(pChild->GetText());
	pChild = pNode->FirstChildElement(XML_TAG_TEXTURE);
	if (pChild != nullptr)
	{
		pChild2 = pChild->FirstChildElement(XML_TAG_PATH);
		if (pChild2!=nullptr)
			m_textureId = (int) Renderer::get()->getTextureManager()->loadTexture(pChild2->GetText());
	}
}

void SimpleTLMaterial::set()
{
	//no alpha blending
	glEnable(GL_LIGHTING);
	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE);

	Renderer::get()->getTextureManager()->set(m_textureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_textureWrapModeS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_textureWrapModeT);

	glShadeModel(GL_SMOOTH);

	glMaterialfv(GL_FRONT, GL_AMBIENT, m_ambient.rgba());
	glMaterialfv(GL_FRONT, GL_DIFFUSE, m_diffuse.rgba());
	glMaterialfv(GL_FRONT, GL_SPECULAR, m_specular.rgba());
	glMaterialf(GL_FRONT, GL_SHININESS, (float)m_shininess);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}

void TranslucentMaterial::set()
{
	glEnable(GL_LIGHTING);
	//alpha blending: no depth writing, no face culling
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//	glDepthMask(GL_FALSE);
	glDisable(GL_CULL_FACE);

	Renderer::get()->getTextureManager()->set(m_textureId);

	glShadeModel(GL_SMOOTH);

	glMaterialfv(GL_FRONT, GL_AMBIENT, m_ambient.rgba());
	glMaterialfv(GL_FRONT, GL_DIFFUSE, m_diffuse.rgba());
	glMaterialfv(GL_FRONT, GL_SPECULAR, m_specular.rgba());
	glMaterialf(GL_FRONT, GL_SHININESS, (float)m_shininess);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}

SimpleTLMaterial::SimpleTLMaterial(Color ambient, Color diffuse, Color specular, Color emission
	, double shininess, string texture)
	:m_ambient(ambient), m_diffuse(diffuse), m_specular(specular), m_emission(emission)
	, m_shininess(shininess)
{
	m_textureId= (int) Renderer::get()->getTextureManager()->loadTexture(texture);
}

SimpleTLMaterial::SimpleTLMaterial(string texturePath)
{
	m_textureId= (int)Renderer::get()->getTextureManager()->loadTexture(texturePath);
}


void ColorMaterial::set()
{
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);

	glColor4fv(m_color.rgba());
}

void LineMaterial::set()
{
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);
	glLineWidth((float)m_width);
	glColor4fv(m_color.rgba());
}