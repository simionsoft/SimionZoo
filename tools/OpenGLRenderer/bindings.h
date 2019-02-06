#pragma once

#include <string>
#include <vector>
using namespace std;

#include "xml-tags.h"
class Vector3D;
class Point3D;
class Vector2D;
class Point2D;
class Quaternion;


class Bindable
{
public:
	virtual void update(double value)= 0;
	virtual ~Bindable() {};
};

template <typename T>
class BoundObject: public Bindable
{
	T& m_obj;

	void update(Vector3D& v, double value);
	void update(Point3D& p, double value);
	void update(Vector2D& v, double value);
	void update(Point2D& p, double value);
	void update(Quaternion& quat, double value);
	void update(double& scalarParameter, double value);
public:

	string internalName;

	BoundObject(T& object, string& name)
		: m_obj(object), internalName(name) {}
	
	void update(double value);
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
		: m_offset(offset), m_multiplier(multiplier), externalName(_externalName)
	{
		boundObjects.push_back(new BoundObject<T>(obj, _internalName));
	}
	~Binding();
	void update(double value);
	void addBoundObject(Bindable* pObj);
};
