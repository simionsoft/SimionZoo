#pragma once
#include "color.h"
namespace tinyxml2 { class XMLElement; }
#include <string>
using namespace std;

class Material
{
public:
	Material();
	virtual ~Material();

	static Material* getInstance(tinyxml2::XMLElement* pNode);

	virtual void set()= 0;
};

class SimpleTLMaterial : public Material
{
protected:
	int m_textureId= -1;
	int m_textureWrapModeS = GL_REPEAT;
	int m_textureWrapModeT = GL_REPEAT;
	Color m_ambient, m_diffuse, m_specular, m_emission;

	double m_shininess;
public:
	SimpleTLMaterial() {}
	SimpleTLMaterial(tinyxml2::XMLElement* pNode);
	SimpleTLMaterial(string texturePath);
	SimpleTLMaterial(Color ambient, Color diffuse, Color specular, Color emission, double shininess
		,string texturePath);
	virtual ~SimpleTLMaterial() {}
	void setTexture(int id) { m_textureId = id; }
	void setAmbient(Color color) { m_ambient = color; }
	void setDiffuse(Color color) { m_diffuse = color; }
	void setSpecular(Color color) { m_specular = color; }
	void setEmission(Color color) { m_emission = color; }
	void setShininess(double value) { m_shininess = value; }
	void setTextureWrapMode(int mode) { m_textureWrapModeS = mode; m_textureWrapModeT = mode; }
	virtual void set();
};

class TranslucentMaterial : public SimpleTLMaterial
{
public:
	TranslucentMaterial(tinyxml2::XMLElement* pNode) : SimpleTLMaterial(pNode) {}
	TranslucentMaterial(string texture) :SimpleTLMaterial(texture) {}
	TranslucentMaterial(Color ambient, Color diffuse, Color specular, Color emission, double shininess, string texture)
		:SimpleTLMaterial(ambient, diffuse, specular, emission, shininess, texture) {}
	virtual ~TranslucentMaterial() {}

	virtual void set();
};

class ColorMaterial : public Material
{
	Color m_color = Color(1.0, 0.0, 0.0, 1.0);
public:
	ColorMaterial() {}
	void set();
	void setColor(Color color) { m_color = color; }
};

class LineMaterial : public Material
{
	Color m_color = Color(0.0,0.0,0.0,1.0);
	int m_width= 1;
public:
	LineMaterial() {}
	void set();
	void setColor(Color color) { m_color = color; }
	void setWidth(int width) { m_width= width; }
};