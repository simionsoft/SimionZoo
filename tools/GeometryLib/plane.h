#pragma once

#include "vector3d.h"

class Plane
{
protected:
	Vector3D m_normal; //normal to the plane
	double m_distance; //distance to the origin of coordinates
public:
	Plane();
	virtual ~Plane();

	void setNormal(double x, double y, double z);
	void setNormal(Vector3D normal);
	void setDistance(double dist);
	Vector3D getNormal() const;
	double getDistance() const;
};

