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

	const Point3D& min() const;
	const Point3D& max() const;
	Point3D& minAsRef();
	Point3D& maxAsRef();
	void setMin(const Point3D& min) { m_min = min; }
	void setMax(const Point3D& max) { m_max = max; }
	Point3D size() const;
	Point3D center() const;
	Point3D getMinMax(unsigned int index) const;

	bool bSet() const { return m_bSet; }
	void set(bool set) { m_bSet = set; }
};

class BoundingBox2D
{
	bool m_bSet = false;
	Point2D m_min, m_max;
public:
	BoundingBox2D();
	virtual ~BoundingBox2D();

	void addPoint(Point2D p);
	void reset();

	const Point2D& min() const;
	const Point2D& max() const;
	Point2D& minAsRef();
	Point2D& maxAsRef();
	void setMin(const Point2D& min) { m_min = min; }
	void setMax(const Point2D& max) { m_max = max; }
	Point2D size() const;
	Point2D center() const;

	bool bSet() const { return m_bSet; }
	void set(bool set) { m_bSet = set; }
};

