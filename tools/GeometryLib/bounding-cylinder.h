#pragma once

class Vector3D;

class BoundingCylinder
{
protected:
	bool m_bSet = false;
	double m_radius;
public:
	BoundingCylinder();
	virtual ~BoundingCylinder();

	void addPoint(Vector3D& point);
	void reset();

	double radius() const;
	void setRadius(double r);
};

