#pragma once

#include "vector3d.h"
#include "vector2d.h"
#undef min
#undef max
namespace tinyxml2 { class XMLElement; }

class BoundingBox3D
{
	bool m_bSet= false;
	Point3D m_min, m_max;
public:
	BoundingBox3D();
	BoundingBox3D(Point3D min, Point3D max);
	virtual ~BoundingBox3D();
	void addPoint(Point3D p);
	void reset();

	Point3D min() const;
	Point3D max() const;
	Point3D size() const;
	Point3D center() const;
	Point3D getMinMax(unsigned int index) const;

	void draw() const;
	void load(tinyxml2::XMLElement* pNode);
	bool bSet() const { return m_bSet; }
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

	Point2D min() const;
	Point2D max() const;
	Point2D size() const;
	Point2D center() const;

	void draw() const;
	void load(tinyxml2::XMLElement* pNode);
	bool bSet() const { return m_bSet; }
};

