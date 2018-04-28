#pragma once
#include "color.h"
namespace tinyxml2 { class XMLElement; }
#include <string>
#include <vector>
using namespace std;

class Material
{
public:
	Material();
	virtual ~Material();

	static Material* getInstance(tinyxml2::XMLElement* pNode);

	virtual void set()= 0;
};

class UnlitTextureMaterial : public Material
{
protected:
	int m_textureId = -1;
	int m_textureWrapModeS = 0x2901;// GL_REPEAT;
	int m_textureWrapModeT = 0x2901;// GL_REPEAT;
public:
	UnlitTextureMaterial() {}
	UnlitTextureMaterial(tinyxml2::XMLElement* pNode);
	UnlitTextureMaterial(string texturePath);
	void setTextureWrapMode(int mode);
	virtual void set();
};

class UnlitLiveTextureMaterial : public Material
{
protected:
	unsigned int m_textureId = -1;
	float* m_pTexelBuffer = nullptr;
	unsigned int m_sizeX= 0, m_sizeY= 0;
	unsigned int m_numTexels = 0;
	unsigned int m_numChannels = 4;
public:
	UnlitLiveTextureMaterial(unsigned int sizeX, unsigned int sizeY);
	virtual ~UnlitLiveTextureMaterial();
	void updateTexture(const vector<double>& pBuffer);
	virtual void set();
	
};

class SimpleTLMaterial : public Material
{
protected:
	int m_textureId= -1;
	int m_textureWrapModeS = 0x2901;// GL_REPEAT;
	int m_textureWrapModeT = 0x2901;// GL_REPEAT;
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
	ColorMaterial(Color color) : m_color(color) {}
	ColorMaterial() {}
	ColorMaterial(tinyxml2::XMLElement* pNode);
	void set();
	void setColor(Color color) { m_color = color; }
};

class LineMaterial : public Material
{
	Color m_color = Color(0.0,0.0,0.0,1.0);
	int m_width= 1;
public:
	LineMaterial() {}
	LineMaterial(tinyxml2::XMLElement* pNode);
	void set();
	void setColor(Color color) { m_color = color; }
	void setWidth(int width) { m_width= width; }
};