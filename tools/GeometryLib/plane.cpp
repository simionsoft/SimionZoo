#include "plane.h"


Plane::Plane()
{
}


Plane::~Plane()
{
}

void Plane::setNormal(double x, double y, double z)
{
	m_normal = Vector3D(x, y, z);
}

void Plane::setNormal(Vector3D normal)
{
	m_normal = normal;
}

void Plane::setDistance(double dist)
{
	m_distance = dist;
}

Vector3D Plane::getNormal() const
{
	return m_normal;
}

double Plane::getDistance() const
{
	return m_distance;
}