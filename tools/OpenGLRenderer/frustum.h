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
#define NUM_POINTS 8
class Frustum
{
	Plane planes[NUM_PLANES];
	Point3D points[NUM_POINTS];

	void planesFromCameraMatrix(Matrix44& cameraMatrix);
public:
	Frustum();
	virtual ~Frustum();

	void fromCameraMatrix(Matrix44& cameraMatrix);

	bool insideFrustum(BoundingBox3D& boundingBox);
};

