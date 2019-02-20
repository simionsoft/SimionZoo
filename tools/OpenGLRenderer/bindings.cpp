#include "stdafx.h"
#include "bindings.h"
#include "../GeometryLib/vector2d.h"
#include "../GeometryLib/vector3d.h"
#include "../GeometryLib/quaternion.h"

Binding::~Binding()
{
	for (Bindable* object : boundObjects)
		delete object;
}

void Binding::update(double value)
{
	for (Bindable* object : boundObjects)
		object->update( value*m_multiplier + m_offset );
}

void Binding::addBoundObject(Bindable* pObj)
{
	boundObjects.push_back(pObj);
}

template <typename T>
void BoundObject<T>::update(Vector3D& v, double value)
{
	if (internalName == XML_TAG_X) v.setX(value);
	else if (internalName == XML_TAG_Y) v.setY(value);
	else if (internalName == XML_TAG_Z) v.setZ(value);
}

template <typename T>
void BoundObject<T>::update(Point3D& p, double value)
{
	if (internalName == XML_TAG_X) p.setX(value);
	else if (internalName == XML_TAG_Y) p.setY(value);
	else if (internalName == XML_TAG_Z) p.setZ(value);
}

template <typename T>
void BoundObject<T>::update(Vector2D& v, double value)
{
	if (internalName == XML_TAG_X) v.setX(value);
	else if (internalName == XML_TAG_Y) v.setY(value);
}

template <typename T>
void BoundObject<T>::update(Point2D& p, double value)
{
	if (internalName == XML_TAG_X) p.setX(value);
	else if (internalName == XML_TAG_Y) p.setY(value);
}

template <typename T>
void BoundObject<T>::update(Quaternion& quat, double value)
{
	if (internalName == XML_TAG_X) quat.setX(value);
	else if (internalName == XML_TAG_Y) quat.setY(value);
	else if (internalName == XML_TAG_Z) quat.setZ(value);
	else if (internalName == XML_TAG_W) quat.setW(value);
	else if (internalName == XML_TAG_YAW) quat.setYaw(value);
	else if (internalName == XML_TAG_PITCH) quat.setPitch(value);
	else if (internalName == XML_TAG_ROLL) quat.setRoll(value);
}

template <typename T>
void BoundObject<T>::update(double& scalarParameter, double value)
{
	scalarParameter = value;
}

template <typename T>
void BoundObject<T>::update(double value)
{
	update(m_obj, value);
}

template void BoundObject<Vector3D>::update(double);
template void BoundObject<Point3D>::update(double);
template void BoundObject<Vector2D>::update(double);
template void BoundObject<Point2D>::update(double);
template void BoundObject<Quaternion>::update(double);
template void BoundObject<double>::update(double);