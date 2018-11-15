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
	void setRotation(const Quaternion& quat);
	void setTranslation(const Vector3D& translation);
	void setScale(const Vector3D& scale);
	void setPerspective(double halfWidth, double halfHeight, double nearPlaneDist, double farPlaneDist);

	Matrix44 operator*(const Matrix44& mat) const;
	BoundingBox3D operator*(const BoundingBox3D& box) const;

	Point3D operator*(const Point3D& p) const;
	Vector3D operator*(const Vector3D& v) const;

	Point2D operator* (const Point2D& p) const;
	Vector2D operator* (const Vector2D& v) const;

	double* asArray();

	double get(int col, int row) const;
	void set(int col, int row, double value);

	void flipYZAxis();
};

