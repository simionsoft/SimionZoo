#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "../../../tools/GeometryLib/frustum.h"
#include "../../../tools/GeometryLib/matrix44.h"


namespace insideFrustum
{		
	TEST_CLASS(UnitTest1)
	{
	public:

		TEST_METHOD(BoundingBoxInsideFrustum)
		{
			Frustum frustum;
			Matrix44 projection;

			//initialize the frustum
			projection.setPerspective(1.0, 0.75, 1.0, 10.0);
			frustum.fromCameraMatrix(projection);

			//start testing bounding boxes
			BoundingBox3D box;
			box= BoundingBox3D(Vector3D(0.0, 0.0, -1.0),Vector3D(1.0, 1.0, -2.0));
			Assert::IsTrue(frustum.isVisible(box));
			box = BoundingBox3D(Vector3D(-10.0, 0.0, 0.0), Vector3D(-9.0, 1.0, 1.0));
			Assert::IsFalse(frustum.isVisible(box));
		}
		TEST_METHOD(BoundingBoxInsideFrustum2)
		{
			Frustum frustum;
			Matrix44 projection, view, cameraMatrix;

			//initialize the frustum
			projection.setPerspective(1.0, 0.75, 1.0, 10.0);
			view.setTranslation(Vector3D(-10.0, 0.0, 0.0));
			cameraMatrix = projection*view;
			frustum.fromCameraMatrix(cameraMatrix);

			//start testing bounding boxes
			BoundingBox3D box;
			box = BoundingBox3D(Vector3D(0.0, 0.0, -1.0), Vector3D(1.0, 1.0, -2.0));
			Assert::IsFalse(frustum.isVisible(box));
			box = BoundingBox3D(Vector3D(10.0, 0.0, 0.0), Vector3D(11.0, 1.0, 1.0));
			Assert::IsFalse(frustum.isVisible(box));
			box = BoundingBox3D(Vector3D(10.0, 0.0, -1.5), Vector3D(11.0, 1.0, -2.0));
			Assert::IsTrue(frustum.isVisible(box));
		}
		TEST_METHOD(BoundingBoxInsideFrustum3)
		{
			Frustum frustum;
			Matrix44 projection, view, cameraMatrix;

			//initialize the frustum
			projection.setPerspective(1.0, 0.75, 1.0, 10.0);
			view.setTranslation(Vector3D(0.0, 0.0, -11.0));
			cameraMatrix = projection*view;
			frustum.fromCameraMatrix(cameraMatrix);

			//start testing bounding boxes
			BoundingBox3D box;
			box = BoundingBox3D(Vector3D(0.0, 0.0, -1.0), Vector3D(1.0, 1.0, -2.0));
			Assert::IsFalse(frustum.isVisible(box));
			box = BoundingBox3D(Vector3D(0.0, 0.0, 0.0), Vector3D(1.0, 1.0, 2.0));
			Assert::IsTrue(frustum.isVisible(box));
			box = BoundingBox3D(Vector3D(0.0, 0.0, -1.5), Vector3D(1.0, 1.0, -2.0));
			Assert::IsFalse(frustum.isVisible(box));
		}
		TEST_METHOD(BoundingBoxInsideFrustum4)
		{
			Frustum frustum;
			Matrix44 projection, view, cameraMatrix;

			//initialize the frustum
			projection.setPerspective(1.0, 0.75, 1.0, 10.0);
			view.setRotation(Quaternion(-1.57,0,0));
			cameraMatrix = projection*view;
			frustum.fromCameraMatrix(cameraMatrix);

			//start testing bounding boxes
			BoundingBox3D box;
			box = BoundingBox3D(Vector3D(10.0, 0.0, 0.0), Vector3D(11.0, 1.0, 1.0));
			Assert::IsTrue(frustum.isVisible(box));
			box = BoundingBox3D(Vector3D(1.0, 0.0, 0.0), Vector3D(2.0, 1.0, 2.0));
			Assert::IsTrue(frustum.isVisible(box));
			box = BoundingBox3D(Vector3D(11.0, 0.0, -1.5), Vector3D(13.0, 1.0, -2.0));
			Assert::IsFalse(frustum.isVisible(box));
		}
		TEST_METHOD(PointInsideFrustum)
		{
			Frustum frustum;
			Matrix44 cameraMatrix;

			//initialize the frustum: from z=-1 (near plane) to z=-10 (far plane)
			cameraMatrix.setPerspective(1.0, 0.75, 1.0, 10.0);
			frustum.fromCameraMatrix(cameraMatrix);

			//start testing bounding boxes
			Point3D point;
			point = Point3D(0.0, 0.0, -1.1);
			Assert::IsTrue(frustum.isVisible(point));
			point = Point3D(0.0, 0.0, -0.5);
			Assert::IsFalse(frustum.isVisible(point));
			point = Point3D(0.0, 0.0, -10.5);
			Assert::IsFalse(frustum.isVisible(point));
			point = Point3D(-2.0, 0.0, -1.5);
			Assert::IsFalse(frustum.isVisible(point));
		}
		TEST_METHOD(PointInsideFrustum2)
		{
			Frustum frustum;
			Matrix44 perspective,view,cameraMatrix;

			//initialize the frustum: from z=-1 (near plane) to z=-10 (far plane)
			perspective.setPerspective(1.0, 0.75, 1.0, 10.0);
			view.setTranslation(Vector3D(-10.0, 0.0, 0.0));
			cameraMatrix = perspective * view;
			frustum.fromCameraMatrix(cameraMatrix);

			//start testing bounding boxes
			Point3D point;
			point = Point3D(10.0, 0.0, -1.1);
			Assert::IsTrue(frustum.isVisible(point));
			point = Point3D(10.0, 0.0, -0.5);
			Assert::IsFalse(frustum.isVisible(point));
			point = Point3D(10.0, 0.0, -10.5);
			Assert::IsFalse(frustum.isVisible(point));
			point = Point3D(-2.0, 0.0, -1.5);
			Assert::IsFalse(frustum.isVisible(point));
		}
	};
}