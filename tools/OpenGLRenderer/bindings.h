#pragma once
#include "xml-load.h"
#include <string>
#include <vector>
using namespace std;

class Bindable
{
public:
	virtual void update(std::string key, double value)= 0;
};

class BoundObject
{
public:
	Bindable* pObj;
	string internalName;

	BoundObject(Bindable* pObject, string& name)
		: pObj(pObject), internalName(name) {}
};

class Binding
{
public:
	string externalName;
	vector <BoundObject*> boundObjects;

	Binding(std::string& _externalName, Bindable* _pObj, std::string& _internalName)
		:externalName(_externalName)
	{
		boundObjects.push_back(new BoundObject(_pObj,_internalName));
	}
	~Binding()
	{
		for (auto it = boundObjects.begin(); it != boundObjects.end(); ++it)
			delete (*it);
	}
	size_t getNumBoundObjects() const { return boundObjects.size(); }
	BoundObject* getBoundObject(unsigned int i)
	{
		if (i<boundObjects.size())
			return boundObjects[i];
		return nullptr;
	}
	void addBoundObject(BoundObject* pObj)
	{
		boundObjects.push_back(pObj);
	}
};

namespace tinyxml2 { class XMLElement; }
#include "../GeometryLib/quaternion.h"
#include "../GeometryLib/bounding-box.h"
#include "../GeometryLib/bounding-cylinder.h"
#include "../GeometryLib/vector3d.h"
#include "../GeometryLib/vector2d.h"
#include "../GeometryLib/transform2d.h"
#include "../GeometryLib/transform3d.h"
#include "color.h"

class BindableQuaternion : public Bindable, XMLReader, Quaternion
{
public:
	void load(tinyxml2::XMLElement* pNode);
	void update(std::string key, double value);
};

class BindableVector3D : public Bindable, public XMLReader, public Vector3D
{
public:
	void load(tinyxml2::XMLElement* pNode);
	void update(std::string key, double value);
};

class BindablePoint3D : public Bindable, public XMLReader, public Point3D
{
public:
	void load(tinyxml2::XMLElement* pNode);
	void update(std::string key, double value);
};

class BindableVector2D : public Bindable, public XMLReader, public Vector2D
{
public:
	void load(tinyxml2::XMLElement* pNode);
	void update(std::string key, double value);
};

class LoadableTransform3D: public XMLReader, public Transform3D
{
public:
	void load(tinyxml2::XMLElement* pNode);
};

class LoadableColor : public XMLReader, public Color
{
public:
	void load(tinyxml2::XMLElement* pNode);
};

class LoadableBoundingBox3D : public XMLReader, public BoundingBox3D
{
public:
	void draw() const;
	void load(tinyxml2::XMLElement* pNode);
};

class LoadableBoundingBox2D : public XMLReader, public BoundingBox2D
{
public:
	void load(tinyxml2::XMLElement* pNode);
};

class LoadableBoundingCylinder : public XMLReader, public BoundingCylinder
{
public:
	void load(tinyxml2::XMLElement* pNode);
};
