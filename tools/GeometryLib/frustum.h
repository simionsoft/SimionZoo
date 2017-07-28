#pragma once

class Matrix44;
class BoundingBox3D;
#include "plane.h"
#include "vector3d.h"
enum planeEnum
{
	NEAR_PLANE = 0,
	FAR_PLANE,
	LEFT_PLANE,
	RIGHT_PLANE,
	TOP_PLANE,
	BOTTOM_PLANE
};

#define NUM_PLANES 6

class Frustum
{
protected:
	Plane planes[NUM_PLANES];

public:
	Frustum();
	virtual ~Frustum();

	void fromCameraMatrix(Matrix44& cameraMatrix);

	bool isVisible(BoundingBox3D& boundingBox) const;
	bool isVisible(Point3D& point) const;
};

