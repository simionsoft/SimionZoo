#pragma once
#include "quaternion.h"
#include "vector3d.h"
#include "bounding-box.h"

class Matrix44
{
protected:
	double _values[16];
public:
	Matrix44();
	virtual ~Matrix44();

	void setIdentity();
	void setRotation(Quaternion& quat);
	void setTranslation(Vector3D& translation);
	void setScale(Vector3D& scale);
	void setPerspective(double halfWidth, double halfHeight, double nearPlaneDist, double farPlaneDist);

	Matrix44 operator*(Matrix44& mat) const;
	BoundingBox3D operator*( const BoundingBox3D& box) const;
	Point3D operator*(Point3D& p) const;
	Vector3D operator*(Vector3D& v) const;

	double* asArray();

	double get(int col, int row) const;
	void set(int col, int row, double value);

	void flipYZAxis();
};

