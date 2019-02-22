/*
	SimionZoo: A framework for online model-free Reinforcement Learning on continuous
	control problems

	Copyright (c) 2016 SimionSoft. https://github.com/simionsoft

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

#include "vector2d.h"
#include <math.h>
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

double Vector2D::length() const
{
	return sqrt(_values[0]*_values[0] + _values[1]*_values[1]);
}

double Vector2D::squareLength() const
{
	return _values[0] *_values[0]+ _values[1]* _values[1];
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

