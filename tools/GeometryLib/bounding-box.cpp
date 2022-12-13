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

#include "bounding-box.h"
#include <algorithm>
#include <limits>

BoundingBox3D::BoundingBox3D()
{
	reset();
}

BoundingBox3D::BoundingBox3D(Point3D min, Point3D max)
{
	m_min = min;
	m_max = max;
}

void BoundingBox3D::reset()
{
	m_min.setX(std::numeric_limits<double>::max());
	m_min.setY(std::numeric_limits<double>::max());
	m_min.setZ(std::numeric_limits<double>::max());
	m_max.setX(std::numeric_limits<double>::min());
	m_max.setY(std::numeric_limits<double>::min());
	m_max.setZ(std::numeric_limits<double>::min());
}

BoundingBox3D::~BoundingBox3D()
{
}

void BoundingBox3D::addPoint(Point3D p)
{
	m_bSet = true;

	if (p.x() > m_max.x()) m_max.setX(p.x());
	if (p.y() > m_max.y()) m_max.setY(p.y());
	if (p.z() > m_max.z()) m_max.setZ(p.z());

	if (p.x() < m_min.x()) m_min.setX(p.x());
	if (p.y() < m_min.y()) m_min.setY(p.y());
	if (p.z() < m_min.z()) m_min.setZ(p.z());
}


const Point3D& BoundingBox3D::min() const { return m_min; }
const Point3D& BoundingBox3D::max() const { return m_max; }

Point3D& BoundingBox3D::minAsRef()
{
	return m_min;
}

Point3D& BoundingBox3D::maxAsRef()
{
	return m_max;
}

Point3D BoundingBox3D::size() const 
{
	return Point3D(m_max.x() - m_min.x(), m_max.y() - m_min.y()
		, m_max.z() - m_min.z());
}
Point3D BoundingBox3D::center() const
{
	return Point3D(m_min.x() + (m_max.x() - m_min.x())*0.5
		, m_min.y() + (m_max.y() - m_min.y())*0.5, m_min.z() + (m_max.z() - m_min.z())*0.5);
}
Point3D BoundingBox3D::getMinMax(unsigned int index) const
{
	if (index == 0) return m_min;
	return m_max;
}

BoundingBox2D::BoundingBox2D()
{
	reset();
}
void BoundingBox2D::reset()
{
	m_min.setX(std::numeric_limits<double>::max());
	m_min.setY(std::numeric_limits<double>::max());
	m_max.setX(std::numeric_limits<double>::min());
	m_max.setY(std::numeric_limits<double>::min());
}
BoundingBox2D::~BoundingBox2D(){}

void BoundingBox2D::addPoint(Point2D p)
{
	m_bSet = true;

	if (p.x() > m_max.x()) m_max.setX(p.x());
	if (p.y() > m_max.y()) m_max.setY(p.y());

	if (p.x() < m_min.x()) m_min.setX(p.x());
	if (p.y() < m_min.y()) m_min.setY(p.y());
}

const Point2D& BoundingBox2D::min() const
{
	return m_min;
}

const Point2D& BoundingBox2D::max() const
{
	return m_max;
}

Point2D& BoundingBox2D::minAsRef()
{
	return m_min;
}

Point2D& BoundingBox2D::maxAsRef()
{
	return m_max;
}

Point2D BoundingBox2D::size() const
{
	return Point2D(m_max.x() - m_min.x(), m_max.y() - m_min.y());
}

Point2D BoundingBox2D::center() const 
{
	return Point2D(m_min.x() + (m_max.x() - m_min.x())*0.5
	, m_min.y() + (m_max.y() - m_min.y())*0.5);
}