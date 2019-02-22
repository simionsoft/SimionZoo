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

#include "bounding-cylinder.h"
#include "vector3d.h"
#include <algorithm>

BoundingCylinder::BoundingCylinder()
{
	reset();
}


BoundingCylinder::~BoundingCylinder()
{
}

void BoundingCylinder::addPoint(Vector3D& point)
{
	double radius = point.length();
	if (radius > m_radius)
		m_radius = radius;
	m_bSet = true;
}
#undef min
void BoundingCylinder::reset()
{
	m_radius = std::numeric_limits<double>::min();
	m_bSet = false;
}



double BoundingCylinder::radius() const
{
	return m_radius;
}

void BoundingCylinder::setRadius(double r)
{
	m_bSet = true;
	m_radius = r;
}