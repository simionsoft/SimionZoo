/*
	SimionZoo: A framework for online model-free Reinforcement Learning on continuous
	control problems

	Copyright (c) 2016 SimionSoft. https://github.com/simionsoft

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

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
	const char* name = pNode->Name();
	if (!strcmp(name, XML_TAG_SIMPLE_TL_MATERIAL))
		return new SimpleTLMaterial(pNode);
	if (!strcmp(name, XML_TAG_TRANSLUCENT_MATERIAL))
		return new TranslucentMaterial(pNode);
	if (!strcmp(name, XML_TAG_COLOR))
		return new ColorMaterial(pNode);
	if (!strcmp(name, XML_TAG_UNLIT_TEXTURE_MATERIAL))
		return new UnlitTextureMaterial(pNode);
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

SimpleTLMaterial::SimpleTLMaterial(Color ambient, Color diffuse, Color specular, Color emission
	, double shininess, std::string texture)
	:m_ambient(ambient), m_diffuse(diffuse), m_specular(specular), m_emission(emission)
	, m_shininess(shininess)
{
	m_textureId= (int) Renderer::get()->getTextureManager()->loadTexture(texture);
}

SimpleTLMaterial::SimpleTLMaterial(std::string texturePath)
{
	m_textureId= (int)Renderer::get()->getTextureManager()->loadTexture(texturePath);
}

UnlitTextureMaterial::UnlitTextureMaterial(tinyxml2::XMLElement* pNode)
{
	tinyxml2::XMLElement* pChild, *pChild2;
	pChild = pNode->FirstChildElement(XML_TAG_TEXTURE);
	if (pChild != nullptr)
	{
		pChild2 = pChild->FirstChildElement(XML_TAG_PATH);
		if (pChild2 != nullptr)
			m_textureId = (int)Renderer::get()->getTextureManager()->loadTexture(pChild2->GetText());
	}
}

UnlitTextureMaterial::UnlitTextureMaterial(string texturePath)
{
	m_textureId = (int)Renderer::get()->getTextureManager()->loadTexture(texturePath);
}

void UnlitTextureMaterial::setTextureWrapMode(int mode)
{
	m_textureWrapModeS = mode;
	m_textureWrapModeT = mode;
}

void UnlitTextureMaterial::set()
{
	//no alpha blending
	glDisable(GL_LIGHTING);
	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE);

	Renderer::get()->getTextureManager()->set(m_textureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_textureWrapModeS);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_textureWrapModeT);

	glShadeModel(GL_SMOOTH);

	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
}

ColorMaterial::ColorMaterial(tinyxml2::XMLElement* pNode)
{
	tinyxml2::XMLElement* pChild;
	pChild = pNode->FirstChildElement(XML_TAG_COLOR);
	if (pChild)
		XML::load(pChild, m_color);
}

void ColorMaterial::set()
{
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
	glDisable(GL_BLEND);
	glDepthMask(GL_TRUE);

	glColor4fv(m_color.rgba());
}

void TranslucentMaterial::set()
{
	glEnable(GL_LIGHTING);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.1f);

	Renderer::get()->getTextureManager()->set(m_textureId);

	glShadeModel(GL_SMOOTH);

	glMaterialfv(GL_FRONT, GL_AMBIENT, m_ambient.rgba());
	glMaterialfv(GL_FRONT, GL_DIFFUSE, m_diffuse.rgba());
	glMaterialfv(GL_FRONT, GL_SPECULAR, m_specular.rgba());
	glMaterialf(GL_FRONT, GL_SHININESS, (float)m_shininess);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
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

LineMaterial::LineMaterial(tinyxml2::XMLElement* pNode)
{
	tinyxml2::XMLElement* pChild;
	pChild = pNode->FirstChildElement(XML_TAG_COLOR);
	if (pChild)
		XML::load(pChild, m_color);
	pChild = pNode->FirstChildElement(XML_TAG_WIDTH);
	if (pChild)
		m_width = atoi(pChild->GetText());
}