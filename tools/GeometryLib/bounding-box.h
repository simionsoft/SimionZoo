#pragma once

#include "vector3d.h"
#include "vector2d.h"


class BoundingBox3D
{
protected:
	bool m_bSet= false;
	Point3D m_min, m_max;
public:
	BoundingBox3D();
	BoundingBox3D(Point3D min, Point3D max);
	virtual ~BoundingBox3D();
	void addPoint(Point3D p);
	void reset();

	Point3D& min();
	Point3D& max();
	Point3D size() const;
	Point3D center() const;
	Point3D getMinMax(unsigned int index) const;

	bool bSet() const { return m_bSet; }
};

class BoundingBox2D
{
	bool m_bSet = false;
	Vector2D m_min, m_max;
public:
	BoundingBox2D();
	virtual ~BoundingBox2D();

	void addPoint(Vector2D p);
	void reset();

	Vector2D& min();
	Vector2D& max();
	Vector2D size() const;
	Vector2D center() const;

	bool bSet() const { return m_bSet; }
};

