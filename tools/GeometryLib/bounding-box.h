#pragma once

#include "vector3d.h"
#include "vector2d.h"


class BoundingBox3D
{
protected:
	bool m_bSet= false;
	Vector3D m_min, m_max;
public:
	BoundingBox3D();
	BoundingBox3D(Vector3D min, Vector3D max);
	virtual ~BoundingBox3D();
	void addPoint(Vector3D p);
	void reset();

	Vector3D min() const;
	Vector3D max() const;
	Vector3D& min();
	Vector3D& max();
	Vector3D size() const;
	Vector3D center() const;
	Vector3D getMinMax(unsigned int index) const;

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

