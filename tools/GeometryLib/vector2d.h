#pragma once


#undef min
#undef max

class Vector2D
{
protected:
	double _values[2];
public:
	Vector2D():Vector2D(0.0,0.0){}
	Vector2D(double x, double y)
	{
		_values[0] = x;
		_values[1] = y;
	}
	double s() const { return _values[0]; }
	double t() const { return _values[1]; }
	double x() const { return _values[0]; }
	double y() const { return _values[1]; }

	Vector2D operator+(const Vector2D& v) const;
	Vector2D& operator+=(const Vector2D& v);
	Vector2D operator-(const Vector2D& v) const;
	Vector2D& operator-=(const Vector2D& v);
	Vector2D operator*(double scalar) const;
	Vector2D& operator*=(double scalar);
	Vector2D operator/(double scalar) const;
	Vector2D& operator/=(double scalar);

	void setX(double x) { _values[0] = x; }
	void setY(double y) { _values[1] = y; }
};

class Point2D : public Vector2D
{
public:
	Point2D() :Vector2D() {}
	Point2D(double x, double y) : Vector2D(x, y) {}
};

class Range : public Vector2D
{
public:
	Range() : Vector2D(0.0, 1.0) {}
	Range(double min, double max) : Vector2D(min, max) {}
	double min() const { return _values[0]; }
	double max() const { return _values[1]; }
	
	double clamp(double value);
	double normPosInRange(double value);
};

