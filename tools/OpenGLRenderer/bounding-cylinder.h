#pragma once

class Vector3D;

class BoundingCylinder
{
	bool m_bSet = false;
	double m_radius;
public:
	BoundingCylinder();
	virtual ~BoundingCylinder();

	void addPoint(Vector3D& point);
	void reset();

	void load(tinyxml2::XMLElement* pNode);

	double radius() const;
};

