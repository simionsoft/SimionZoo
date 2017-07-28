#pragma once
#include "graphic-object.h"
#include "../GeometryLib/vector2d.h"
#include "../GeometryLib/vector3d.h"
namespace tinyxml2 { class XMLElement; }

class CBasicShape : public CGraphicObject
{
protected:
	CMaterial* m_pMaterialLoaded; //for later use when the mesh is created in the respective shape's constructor
public:
	CBasicShape(tinyxml2::XMLElement* pNode);
	CBasicShape(string name);
	~CBasicShape();
};

class CSphere : public CBasicShape
{
public:
	CSphere(string name) :CBasicShape(name){}
	CSphere(tinyxml2::XMLElement* pNode);
};


class CBox : public CBasicShape
{
	Vector3D m_min = Vector3D(-0.5,0.0,-0.5);
	Vector3D m_max = Vector3D(0.5,1.0, 0.5);
	Vector2D m_minTexCoord = Vector2D(0.0,0.0);
	Vector2D m_maxTexCoord = Vector2D(1.0,1.0);

public:
	CBox(string name) :CBasicShape(name) {}
	CBox(tinyxml2::XMLElement* pNode);
};

class CCilinder : public CBasicShape
{
public:
	CCilinder(string name): CBasicShape(name){}
	CCilinder(tinyxml2::XMLElement* pNode);
};

class CPolyline : public CBasicShape
{
public:
	CPolyline(string name) : CBasicShape(name) {}
	CPolyline(tinyxml2::XMLElement* pNode);
};