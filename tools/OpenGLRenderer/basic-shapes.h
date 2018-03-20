#pragma once
#include "graphic-object.h"
#include "../GeometryLib/vector2d.h"
#include "../GeometryLib/vector3d.h"
namespace tinyxml2 { class XMLElement; }

class BasicShape : public GraphicObject
{
protected:
	Material* m_pMaterialLoaded; //for later use when the mesh is created in the respective shape's constructor
public:
	BasicShape(tinyxml2::XMLElement* pNode);
	BasicShape(string name);
	~BasicShape();
};

class Sphere : public BasicShape
{
public:
	Sphere(string name) :BasicShape(name){}
	Sphere(tinyxml2::XMLElement* pNode);
};


class Box : public BasicShape
{
	Vector3D m_min = Vector3D(-0.5,0.0,-0.5);
	Vector3D m_max = Vector3D(0.5,1.0, 0.5);
	Vector2D m_minTexCoord = Vector2D(0.0,0.0);
	Vector2D m_maxTexCoord = Vector2D(1.0,1.0);

public:
	Box(string name) :BasicShape(name) {}
	Box(tinyxml2::XMLElement* pNode);
};

class Cilinder : public BasicShape
{
public:
	Cilinder(string name): BasicShape(name){}
	Cilinder(tinyxml2::XMLElement* pNode);
};

class PolyLine : public BasicShape
{
public:
	PolyLine(string name) : BasicShape(name) {}
	PolyLine(tinyxml2::XMLElement* pNode);
};