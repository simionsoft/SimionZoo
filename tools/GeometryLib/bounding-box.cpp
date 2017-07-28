#include "bounding-box.h"
#include <algorithm>

BoundingBox3D::BoundingBox3D()
{
	reset();
}

BoundingBox3D::BoundingBox3D(Vector3D min, Vector3D max)
{
	m_min = min;
	m_max = max;
}

void BoundingBox3D::reset()
{
	m_min.setX(std::numeric_limits<double>::max());
	m_min.setY(std::numeric_limits<double>::max());
	m_min.setZ(std::numeric_limits<double>::max());
	m_max.setX(std::numeric_limits<double>::min());
	m_max.setY(std::numeric_limits<double>::min());
	m_max.setZ(std::numeric_limits<double>::min());
}

BoundingBox3D::~BoundingBox3D()
{
}

void BoundingBox3D::addPoint(Vector3D p)
{
	m_bSet = true;

	if (p.x() > m_max.x()) m_max.setX(p.x());
	if (p.y() > m_max.y()) m_max.setY(p.y());
	if (p.z() > m_max.z()) m_max.setZ(p.z());

	if (p.x() < m_min.x()) m_min.setX(p.x());
	if (p.y() < m_min.y()) m_min.setY(p.y());
	if (p.z() < m_min.z()) m_min.setZ(p.z());
}

Vector3D BoundingBox3D::min() const { return m_min; }
Vector3D BoundingBox3D::max() const { return m_max; }
Vector3D& BoundingBox3D::min() { return m_min; }
Vector3D& BoundingBox3D::max() { return m_max; }
Vector3D BoundingBox3D::size() const 
{
	return Vector3D(m_max.x() - m_min.x(), m_max.y() - m_min.y()
		, m_max.z() - m_min.z());
}
Vector3D BoundingBox3D::center() const
{
	return Vector3D(m_min.x() + (m_max.x() - m_min.x())*0.5
		, m_min.y() + (m_max.y() - m_min.y())*0.5, m_min.z() + (m_max.z() - m_min.z())*0.5);
}
Vector3D BoundingBox3D::getMinMax(unsigned int index) const
{
	if (index == 0) return m_min;
	return m_max;
}

BoundingBox2D::BoundingBox2D()
{
	reset();
}
void BoundingBox2D::reset()
{
	m_min.setX(std::numeric_limits<double>::max());
	m_min.setY(std::numeric_limits<double>::max());
	m_max.setX(std::numeric_limits<double>::min());
	m_max.setY(std::numeric_limits<double>::min());
}
BoundingBox2D::~BoundingBox2D(){}

void BoundingBox2D::addPoint(Vector2D p)
{
	m_bSet = true;

	if (p.x() > m_max.x()) m_max.setX(p.x());
	if (p.y() > m_max.y()) m_max.setY(p.y());

	if (p.x() < m_min.x()) m_min.setX(p.x());
	if (p.y() < m_min.y()) m_min.setY(p.y());
}

Vector2D& BoundingBox2D::min()
{
	return m_min;
}

Vector2D& BoundingBox2D::max()
{
	return m_max;
}

Vector2D BoundingBox2D::size() const
{
	return Vector2D(m_max.x() - m_min.x(), m_max.y() - m_min.y());
}

Vector2D BoundingBox2D::center() const 
{
	return Vector2D(m_min.x() + (m_max.x() - m_min.x())*0.5
	, m_min.y() + (m_max.y() - m_min.y())*0.5);
}