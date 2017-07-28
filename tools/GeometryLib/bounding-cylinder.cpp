#include "bounding-cylinder.h"
#include "vector3d.h"
#include <algorithm>

BoundingCylinder::BoundingCylinder()
{
	reset();
}


BoundingCylinder::~BoundingCylinder()
{
}

void BoundingCylinder::addPoint(Vector3D& point)
{
	double radius = point.length();
	if (radius > m_radius)
		m_radius = radius;
	m_bSet = true;
}
#undef min
void BoundingCylinder::reset()
{
	m_radius = std::numeric_limits<double>::min();
	m_bSet = false;
}



double BoundingCylinder::radius() const
{
	return m_radius;
}

void BoundingCylinder::setRadius(double r)
{
	m_bSet = true;
	m_radius = r;
}