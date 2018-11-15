#pragma once

#include <string>
#include <vector>
using namespace std;
#include "../GeometryLib/vector2d.h"
#include "../GeometryLib/vector3d.h"
#include "../GeometryLib/quaternion.h"
#include "xml-load.h"

class Binding;

class Bindable
{
public:
	virtual void update(double value)= 0;
};

template <typename T>
class BoundObject: public Bindable
{
	T& m_obj;

	void update(Vector3D& v, double value)
	{
		if (internalName == XML_TAG_X) v.setX(value);
		else if (internalName == XML_TAG_Y) v.setY(value);
		else if (internalName == XML_TAG_Z) v.setZ(value);
	}

	void update(Point3D& p, double value)
	{
		if (internalName == XML_TAG_X) p.setX(value);
		else if (internalName == XML_TAG_Y) p.setY(value);
		else if (internalName == XML_TAG_Z) p.setZ(value);
	}
	
	void update(Vector2D& v, double value)
	{
		if (internalName == XML_TAG_X) v.setX(value);
		else if (internalName == XML_TAG_Y) v.setY(value);
	}
	
	void update(Point2D& p, double value)
	{
		if (internalName == XML_TAG_X) p.setX(value);
		else if (internalName == XML_TAG_Y) p.setY(value);
	}
	
	void update(Quaternion& quat, double value)
	{
		if (internalName == XML_TAG_X) quat.setX(value);
		else if (internalName == XML_TAG_Y) quat.setY(value);
		else if (internalName == XML_TAG_Z) quat.setZ(value);
		else if (internalName == XML_TAG_W) quat.setW(value);
		else if (internalName == XML_TAG_YAW) quat.setYaw(value);
		else if (internalName == XML_TAG_PITCH) quat.setPitch(value);
		else if (internalName == XML_TAG_ROLL) quat.setRoll(value);
	}

	void update(double& scalarParameter, double value)
	{
		scalarParameter = value;
	}
public:

	string internalName;

	BoundObject(T& object, string& name)
		: m_obj(object), internalName(name) {}
	
	void update(double value)
	{
		update(m_obj, value);
	}
};

class Binding
{
	double m_offset= 0.0;
	double m_multiplier = 1.0;
	vector <Bindable*> boundObjects;
public:
	string externalName;

	template <typename T>
	Binding(std::string& _externalName, T& obj, std::string& _internalName, double offset= 0.0, double multiplier= 0.0)
		:externalName(_externalName), m_offset(offset), m_multiplier(multiplier)
	{
		boundObjects.push_back(new BoundObject<T>(obj, _internalName));
	}
	~Binding();
	void update(double value);
	void addBoundObject(Bindable* pObj);
};
