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

#include "frustum.h"
#include "matrix44.h"
#include "bounding-box.h"
#include "vector3d.h"

Frustum::Frustum()
{
}


Frustum::~Frustum()
{
}

void Frustum::fromCameraMatrix(Matrix44& cameraMatrix)
{
	//FROM: http://gamedevs.org/uploads/fast-extraction-viewing-frustum-planes-from-world-view-projection-matrix.pdf

	// left
	planes[LEFT_PLANE].setNormal(cameraMatrix.get(0, 3) + cameraMatrix.get(0, 0)
		, cameraMatrix.get(1, 3) + cameraMatrix.get(1, 0)
		, cameraMatrix.get(2, 3) + cameraMatrix.get(2, 0));
	planes[LEFT_PLANE].setDistance(cameraMatrix.get(3, 3) + cameraMatrix.get(3, 0));

	// right
	planes[RIGHT_PLANE].setNormal(cameraMatrix.get(0, 3) - cameraMatrix.get(0, 0)
		, cameraMatrix.get(1, 3) - cameraMatrix.get(1, 0)
		, cameraMatrix.get(2, 3) - cameraMatrix.get(2, 0));
	planes[RIGHT_PLANE].setDistance(cameraMatrix.get(3, 3) - cameraMatrix.get(3, 0));

	// bottom
	planes[BOTTOM_PLANE].setNormal(cameraMatrix.get(0, 3) + cameraMatrix.get(0, 1)
		, cameraMatrix.get(1, 3) + cameraMatrix.get(1, 1)
		, cameraMatrix.get(2, 3) + cameraMatrix.get(2, 1));
	planes[BOTTOM_PLANE].setDistance( cameraMatrix.get(3, 3) + cameraMatrix.get(3, 1));

	// top
	planes[TOP_PLANE].setNormal(cameraMatrix.get(0, 3) - cameraMatrix.get(0, 1)
		, cameraMatrix.get(1, 3) - cameraMatrix.get(1, 1)
		, cameraMatrix.get(2, 3) - cameraMatrix.get(2, 1));
	planes[TOP_PLANE].setDistance(cameraMatrix.get(3, 3) - cameraMatrix.get(3, 1));

	// near
	planes[NEAR_PLANE].setNormal(cameraMatrix.get(0, 3) + cameraMatrix.get(0, 2)
		, cameraMatrix.get(1, 3) + cameraMatrix.get(1, 2)
		, cameraMatrix.get(2, 3) + cameraMatrix.get(2, 2));
	planes[NEAR_PLANE].setDistance(cameraMatrix.get(3, 3) + cameraMatrix.get(3, 2));

	// far
	planes[FAR_PLANE].setNormal(cameraMatrix.get(0, 3) - cameraMatrix.get(0, 2)
		, cameraMatrix.get(1, 3) - cameraMatrix.get(1, 2)
		, cameraMatrix.get(2, 3) - cameraMatrix.get(2, 2));
	planes[FAR_PLANE].setDistance(cameraMatrix.get(3, 3) - cameraMatrix.get(3, 2));

	// normalize
	for (unsigned int i = 0; i < NUM_PLANES; i++)
	{
		double length = planes[i].getNormal().length();
		if (length != 0.0)
		{
			planes[i].setNormal(planes[i].getNormal() / length);
			planes[i].setDistance(planes[i].getDistance() / length); // d also has to be divided by the length of the normal
		}
	}
}

bool Frustum::isVisible(const BoundingBox3D& box) const
{
	for (unsigned int i = 0; i < NUM_PLANES; i++)
	{
		// planes have unit-length normal, offset = -dot(normal, point on plane)
		const Plane& plane = planes[i];
		unsigned int nx = plane.getNormal().x() > 0.0;
		unsigned int ny = plane.getNormal().y() > 0.0;
		unsigned int nz = plane.getNormal().z() > 0.0;

		// getMinMax(): 0 = return min coordinate. 1 = return max.
		double dot = (plane.getNormal().x()*box.getMinMax(nx).x()) 
			+ (plane.getNormal().y()*box.getMinMax(ny).y()) 
			+ (plane.getNormal().z()*box.getMinMax(nz).z());

		if (dot < -plane.getDistance())
			return false;

		double dot2 = (plane.getNormal().x()*box.getMinMax(1 - nx).x()) 
			+ (plane.getNormal().y()*box.getMinMax(1 - ny).y()) 
			+ (plane.getNormal().z()*box.getMinMax(1 - nz).z());

		if (dot2 <= -plane.getDistance())
			return true;
	}

	return true;
}

//http://www.iquilezles.org/www/articles/frustumcorrect/frustumcorrect.htm
//bool Frustum::isVisible(BoundingBox3D& box) const
//{
//	// check box outside/inside of frustum
//	
//	for (int i = 0; i<6; i++)
//	{
//		Vector3D &normal = planes[i].getNormal();
//		double dist = planes[i].getDistance();
//		int out = 0;
//		out += (normal.dot(Vector3D(box.min().x(), box.min().y(), box.min().z())) + dist < 0.0) ? 1 : 0;
//		out += (normal.dot(Vector3D(box.max().x(), box.min().y(), box.min().z())) + dist < 0.0) ? 1 : 0;
//		out += (normal.dot(Vector3D(box.min().x(), box.max().y(), box.min().z())) + dist < 0.0) ? 1 : 0;
//		out += (normal.dot(Vector3D(box.max().x(), box.max().y(), box.min().z())) + dist < 0.0) ? 1 : 0;
//		out += (normal.dot(Vector3D(box.min().x(), box.min().y(), box.max().z())) + dist < 0.0) ? 1 : 0;
//		out += (normal.dot(Vector3D(box.max().x(), box.min().y(), box.max().z())) + dist < 0.0) ? 1 : 0;
//		out += (normal.dot(Vector3D(box.min().x(), box.max().y(), box.max().z())) + dist < 0.0) ? 1 : 0;
//		out += (normal.dot(Vector3D(box.max().x(), box.max().y(), box.max().z())) + dist < 0.0) ? 1 : 0;
//		if (out == 8)
//			return false;
//	}
//
//	//// check frustum outside/inside box
//	//int out;
//	//out = 0; for (int i = 0; i<8; i++) out += ((fru.mPoints[i].x > box.mMaxX) ? 1 : 0); if (out == 8) return false;
//	//out = 0; for (int i = 0; i<8; i++) out += ((fru.mPoints[i].x < box.mMinX) ? 1 : 0); if (out == 8) return false;
//	//out = 0; for (int i = 0; i<8; i++) out += ((fru.mPoints[i].y > box.mMaxY) ? 1 : 0); if (out == 8) return false;
//	//out = 0; for (int i = 0; i<8; i++) out += ((fru.mPoints[i].y < box.mMinY) ? 1 : 0); if (out == 8) return false;
//	//out = 0; for (int i = 0; i<8; i++) out += ((fru.mPoints[i].z > box.mMaxZ) ? 1 : 0); if (out == 8) return false;
//	//out = 0; for (int i = 0; i<8; i++) out += ((fru.mPoints[i].z < box.mMinZ) ? 1 : 0); if (out == 8) return false;
//
//	//return true;
//}

bool Frustum::isVisible(const Point3D& point) const
{
	double dist;
	for (unsigned int i = 0; i < NUM_PLANES; i++)
	{
		dist = planes[i].getNormal().dot(point) + planes[i].getDistance();
		if (dist < 0)
		{
			return false;
		}
	}
	return true;
}