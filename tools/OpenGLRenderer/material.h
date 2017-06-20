#pragma once
#include "color.h"
namespace tinyxml2 { class XMLElement; }
#include <string>
using namespace std;

class CMaterial
{
public:
	CMaterial();
	virtual ~CMaterial();

	static CMaterial* getInstance(tinyxml2::XMLElement* pNode);

	virtual void set()= 0;
};

class CSimpleTLMaterial : public CMaterial
{
	int m_textureId= -1;
	Color m_ambient, m_diffuse, m_specular, m_emission;
	double m_shininess;
public:
	CSimpleTLMaterial() {}
	CSimpleTLMaterial(tinyxml2::XMLElement* pNode);
	CSimpleTLMaterial(string texturePath);
	CSimpleTLMaterial(Color ambient, Color diffuse, Color specular, Color emission, double shininess
		,string texturePath);
	void setTexture(int id) { m_textureId = id; }
	void setAmbient(Color color) { m_ambient = color; }
	void setDiffuse(Color color) { m_diffuse = color; }
	void setSpecular(Color color) { m_specular = color; }
	void setEmission(Color color) { m_emission = color; }
	void setShininess(double value) { m_shininess = value; }
	virtual void set();
};

class CColorMaterial : public CMaterial
{
	Color m_color = Color(1.0, 0.0, 0.0, 1.0);
public:
	CColorMaterial() {}
	void set();
	void setColor(Color color) { m_color = color; }
};

class CLineMaterial : public CMaterial
{
	Color m_color = Color(0.0,0.0,0.0,1.0);
	int m_width= 1;
public:
	CLineMaterial() {}
	void set();
	void setColor(Color color) { m_color = color; }
	void setWidth(int width) { m_width= width; }
};