#pragma once


class Vector3D
{
protected:
	double m_x, m_y, m_z;
public:

	Vector3D(double x, double y, double z) :m_x(x), m_y(y), m_z(z) {}
	Vector3D() :m_x(0.0), m_y(0.0), m_z(0.0) {}

	double x() const { return m_x; }
	double y() const { return m_y; }
	double z() const { return m_z; }

	void setX(const double x) { m_x = x; }
	void setY(const double y) { m_y = y; }
	void setZ(const double z) { m_z = z; }

	Vector3D operator+(const Vector3D& vec) const;
	Vector3D operator-(const Vector3D& vec) const;
	Vector3D operator*(const double scalar) const;
	Vector3D operator*(const Vector3D& vec) const;
	Vector3D operator/(const double scalar) const;
	Vector3D& operator+=(const Vector3D& vec);
	Vector3D& operator-=(const Vector3D& vec);
	Vector3D& operator*=(const double scalar);
	Vector3D& operator*=(const Vector3D& vec);
	Vector3D& operator/=(const double scalar);
	Vector3D cross(const Vector3D& vec) const;
	double dot(const Vector3D& vec) const;
	void normalize();
	double length() const;
	Vector3D inverse() const;

	static Vector3D lerp(const Vector3D& vec1, const Vector3D& vec2, double u);
};

class Point3D : public Vector3D
{
public:
	Point3D() :Vector3D() {}
	Point3D(double x,double y, double z): Vector3D(x,y,z){}
};