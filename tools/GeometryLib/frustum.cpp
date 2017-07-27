#include "Frustum.h"
#include "matrix44.h"
#include "bounding-box.h"

Frustum::Frustum()
{
}


Frustum::~Frustum()
{
}

void Frustum::fromCameraMatrix(Matrix44& cameraMatrix)
{
	//FROM: http://ruh.li/CameraViewFrustum.html

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

bool Frustum::isVisible(BoundingBox3D& box) const
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

bool Frustum::isVisible(Point3D& point) const
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