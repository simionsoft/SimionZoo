#include "vector2d.h"
#include <algorithm>

Vector2D Vector2D::operator+(const Vector2D& v) const
{
	return Vector2D(x() + v.x(), y() + v.y());
}

Vector2D& Vector2D::operator+=(const Vector2D& v)
{
	_values[0] = _values[0] + v._values[0];
	_values[1] = _values[1] + v._values[1];
	return (*this);
}

Vector2D Vector2D::operator-(const Vector2D& v) const
{
	return Vector2D(x() - v.x(), y() - v.y());
}

Vector2D& Vector2D::operator-=(const Vector2D& v)
{
	_values[0] = _values[0] - v._values[0];
	_values[1] = _values[1] - v._values[1];
	return (*this);
}

Vector2D Vector2D::operator*(double scalar) const
{
	return Vector2D(x()*scalar, y()*scalar);
}

Vector2D& Vector2D::operator*=(double scalar)
{
	_values[0] *= scalar;
	_values[1] *= scalar;
	return (*this);
}

Vector2D Vector2D::operator/(double scalar) const
{
	if (scalar!=0)
		return Vector2D(x()/scalar, y()/scalar);
	return Vector2D(x(), y());
}

Vector2D& Vector2D::operator/=(double scalar)
{
	_values[0] /= scalar;
	_values[1] /= scalar;
	return (*this);
}

double Range::clamp(double value)
{
	return std::min(std::max(value, Range::min()), Range::max());
}

double Range::normPosInRange(double value)
{
	if (Range::max() <= Range::min())
		return 0.0;

	value = clamp(value);

	return (value - Range::min()) / (Range::max() - Range::min());
}