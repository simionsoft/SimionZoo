#pragma once
#include "quaternion.h"
#include "vector3d.h"

class Matrix44
{
	double _values[16];
public:
	Matrix44();
	virtual ~Matrix44();

	static Matrix44 identity();
	static Matrix44 rotationMatrix(Quaternion& quat);
	static Matrix44 translationMatrix(Vector3D& translation);
	static Matrix44 scaleMatrix(Vector3D& scale);

	Matrix44 operator*(Matrix44& mat) const;

	Point3D operator*(Point3D& p) const;
	Vector3D operator*(Vector3D& v) const;

	double* asArray();

	double get(int col, int row) const;
	void set(int col, int row, double value);

	void flipYZAxis();
};

