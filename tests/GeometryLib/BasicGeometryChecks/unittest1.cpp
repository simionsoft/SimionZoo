#include "stdafx.h"
#include "CppUnitTest.h"
#include "../../../tools/GeometryLib/matrix44.h"
#include "../../../tools/GeometryLib/quaternion.h"
#include "../../../tools/GeometryLib/vector3d.h"
#include "../../../tools/GeometryLib/frustum.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace BasicGeometryChecks
{		
	TEST_CLASS(GeometryLibTest)
	{
	public:
		TEST_METHOD(Geometry_BoundingBoxInsideFrustum)
		{
			Frustum frustum;
			Matrix44 projection;

			//initialize the frustum
			projection.setPerspective(1.0, 0.75, 1.0, 10.0);
			frustum.fromCameraMatrix(projection);

			//start testing bounding boxes
			BoundingBox3D box;
			box = BoundingBox3D(Vector3D(0.0, 0.0, -1.0), Vector3D(1.0, 1.0, -2.0));
			Assert::IsTrue(frustum.isVisible(box));
			box = BoundingBox3D(Vector3D(-10.0, 0.0, 0.0), Vector3D(-9.0, 1.0, 1.0));
			Assert::IsFalse(frustum.isVisible(box));
		}
		TEST_METHOD(Geometry_BoundingBoxInsideFrustum2)
		{
			Frustum frustum;
			Matrix44 projection, view, cameraMatrix;

			//initialize the frustum
			projection.setPerspective(1.0, 0.75, 1.0, 10.0);
			view.setTranslation(Vector3D(-10.0, 0.0, 0.0));
			cameraMatrix = projection * view;
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
		TEST_METHOD(Geometry_BoundingBoxInsideFrustum3)
		{
			Frustum frustum;
			Matrix44 projection, view, cameraMatrix;

			//initialize the frustum
			projection.setPerspective(1.0, 0.75, 1.0, 10.0);
			view.setTranslation(Vector3D(0.0, 0.0, -11.0));
			cameraMatrix = projection * view;
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
		TEST_METHOD(Geometry_BoundingBoxInsideFrustum4)
		{
			Frustum frustum;
			Matrix44 projection, view, cameraMatrix;

			//initialize the frustum
			projection.setPerspective(1.0, 0.75, 1.0, 10.0);
			view.setRotation(Quaternion(-1.57, 0, 0).inverse());
			cameraMatrix = projection * view;
			frustum.fromCameraMatrix(cameraMatrix);

			//test bounding boxes
			BoundingBox3D box;
			Point3D point;
			Point3D rotatedPoint = cameraMatrix * Point3D(0, 0, 0);
			box = BoundingBox3D(Vector3D(12.0, 0.0, 0.0), Vector3D(13.0, 1.0, 1.0));
			Assert::IsFalse(frustum.isVisible(box));
			box = BoundingBox3D(Vector3D(5.0, 0.0, 0.0), Vector3D(6.0, 1.0, 2.0));
			Assert::IsTrue(frustum.isVisible(box));
			box = BoundingBox3D(Vector3D(0.0, 0.0, -1.5), Vector3D(0.3, 1.0, -2.0));
			Assert::IsFalse(frustum.isVisible(box));
			box = BoundingBox3D(Vector3D(-5.0, 0.0, -3.0), Vector3D(-2.0, 3.0, 6.0));
			Assert::IsFalse(frustum.isVisible(box));

			//transform + box
			Matrix44 translation, rot, transform;
			translation.setTranslation(Vector3D(5.0, 1.0, -1.0));
			rot.setRotation(Quaternion(1.0, 0.3, 0.7));
			transform = translation * rot;
			box = BoundingBox3D(Vector3D(0.0, 0.0, -1.5), Vector3D(0.3, 1.0, -2.0));
			Assert::IsTrue(frustum.isVisible(transform*box));


			//test points
			point = Point3D(5.0, 0.0, 0.0);
			Assert::IsTrue(frustum.isVisible(point));
			point = Point3D(2.0, 0.0, 0.5);
			Assert::IsTrue(frustum.isVisible(point));
			point = Point3D(10.0, 3.0, 0.0);
			Assert::IsTrue(frustum.isVisible(point));
			point = Point3D(15.0, 0.0, 0.0);
			Assert::IsFalse(frustum.isVisible(point));
			point = Point3D(5.0, 10.0, 0.0);
			Assert::IsFalse(frustum.isVisible(point));
			point = Point3D(5.0, 0.0, -10.0);
			Assert::IsFalse(frustum.isVisible(point));
			point = Point3D(-5.0, 0.0, 0.0);
			Assert::IsFalse(frustum.isVisible(point));

			//camera*modelview transform + box
			cameraMatrix = projection * view * transform;
			frustum.fromCameraMatrix(cameraMatrix);
			Assert::IsTrue(frustum.isVisible(box));
		}
		TEST_METHOD(Geometry_PointInsideFrustum)
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
		TEST_METHOD(Geometry_PointInsideFrustum2)
		{
			Frustum frustum;
			Matrix44 perspective, view, cameraMatrix;

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
		TEST_METHOD(Geometry_EulerToQuaternion)
		{
			double yaw, pitch, roll;
			Quaternion rotation;

			yaw = 1.52; pitch = 1.0; roll = 0.0;
			rotation= Quaternion(yaw, pitch, roll);
			double pitch2 = rotation.pitch();
			double roll2 = rotation.roll();
			Assert::AreEqual(yaw, rotation.yaw(), 0.1, L"Euler<->Quaternion conversion error (yaw)");
			Assert::AreEqual(pitch, rotation.pitch(), 0.1, L"Euler<->Quaternion conversion error (pitch)");
			Assert::AreEqual(roll, rotation.roll(), 0.1, L"Euler<->Quaternion conversion error (roll)");
			yaw = 0.4; pitch = 0.0; roll = 1.0;
			rotation = Quaternion(yaw, pitch, roll);
			Assert::AreEqual(yaw, rotation.yaw(), 0.1, L"Euler<->Quaternion conversion error (yaw)");
			Assert::AreEqual(pitch, rotation.pitch(), 0.1, L"Euler<->Quaternion conversion error (pitch)");
			Assert::AreEqual(roll, rotation.roll(), 0.1, L"Euler<->Quaternion conversion error (roll)");
			yaw = 0.0; pitch = -1.0; roll = 1.0;
			rotation = Quaternion(yaw, pitch, roll);
			Assert::AreEqual(yaw, rotation.yaw(), 0.1, L"Euler<->Quaternion conversion error (yaw)");
			Assert::AreEqual(pitch, rotation.pitch(), 0.1, L"Euler<->Quaternion conversion error (pitch)");
			Assert::AreEqual(roll, rotation.roll(), 0.1, L"Euler<->Quaternion conversion error (roll)");
			yaw = -0.3; pitch = 1.0; roll = 0.7;
			rotation = Quaternion(yaw, pitch, roll);
			Assert::AreEqual(yaw, rotation.yaw(), 0.1, L"Euler<->Quaternion conversion error (yaw)");
			Assert::AreEqual(pitch, rotation.pitch(), 0.1, L"Euler<->Quaternion conversion error (pitch)");
			Assert::AreEqual(roll, rotation.roll(), 0.1, L"Euler<->Quaternion conversion error (roll)");
		}
		TEST_METHOD(Geometry_RotationOnXYPlane)
		{
			Matrix44 rotationMatrix;
			rotationMatrix.setRotation(Quaternion(0.0, 0.0, 1.57));
			Point2D p2D = Point2D(1.0, 1.0);
			Point2D rotatedP2D = rotationMatrix * p2D;
			Point2D expected = Point2D(-1.0, 1.0);
			double error = (rotatedP2D - expected).length();
			if (error > 0.1)
				Assert::Fail(L"2D rotations on XY plane fail");

			Vector2D v2D = Vector2D(1.0, 1.0);
			Vector2D rotatedV2D = rotationMatrix * v2D;
			Vector2D expectedV = Vector2D(-1.0, 1.0);
			error = (rotatedV2D - expectedV).length();
			if (error > 0.1)
				Assert::Fail(L"2D rotations on XY plane fail");

			Point3D p3D = Point3D(1.0, 1.0, 0.0);
			Point3D rotatedP3D = rotationMatrix * p3D;
			Point3D expectedP3 = Point3D(-1.0, 1.0, 0.0);
			error = (rotatedP3D - expectedP3).length();
			if (error > 0.1)
				Assert::Fail(L"3D rotations on XY plane fail");

			Vector3D v3D = Vector3D(1.0, 1.0, 0.0);
			Vector3D rotatedV3D = rotationMatrix * v3D;
			Vector3D expectedV3 = Vector3D(-1.0, 1.0, 0.0);
			error = (rotatedV3D - expectedV3).length();
			if (error > 0.1)
				Assert::Fail(L"3D rotations on XY plane fail");
		}
		TEST_METHOD(Geometry_CameraTransformations)
		{
			Quaternion rotation = Quaternion(3.1415, 1.0, 0.0);
			Vector3D translation = Vector3D(-10.0, 10.0, 0.0);
			Matrix44 rotMatrix, tranMatrix;
			rotMatrix.setRotation(rotation);
			tranMatrix.setTranslation(translation);
			Matrix44 cameraRotation;
			cameraRotation.setRotation(rotation.inverse());
			Matrix44 cameraTranslation;
			cameraTranslation.setTranslation(translation.inverse());
			Matrix44 cameraTransform = cameraRotation * cameraTranslation;
			Point3D p1 = Point3D(0.0, 0.0, 0.0);
			Point3D p2 = Point3D(10.0, 0.0, 0.0);
			Point3D transP1 = cameraTransform * p1;
			Point3D transP2 = cameraTransform * p2;
			if (abs(transP1.y() - transP2.y()) > 0.1)
				Assert::Fail(L"Camera transformations with non-null yaw and pitch don't work properly");
			if (abs(transP1.x() - transP2.x() - 10.0)> 0.1)
				Assert::Fail(L"Camera transformations with non-null yaw and pitch don't work properly");

			transP1 = cameraRotation * cameraTranslation * tranMatrix * rotMatrix * p1;
			double error = (transP1 - p1).length();
			Assert::AreEqual(0.0, error, 0.1, L"Camera transformation isn't cancelled by its inverse");
		}
		TEST_METHOD(Geometry_QuaternionInverse)
		{
			Quaternion quatYaw = Quaternion(1.47, 0.2, -0.3);
			Quaternion quatPitch = Quaternion(-2.0, 1.3, 0.15);
			Quaternion quatRoll = Quaternion(0.4, -1.0, 0.3);
			Quaternion result;
			Matrix44 matRot, matInverse;
			Vector3D resultVec, error;
			Vector3D vec;

			vec= Vector3D(1.0, 2.0, -1.0);

			matRot.setRotation(quatYaw);
			matInverse.setRotation(quatYaw.inverse());
			resultVec = matRot * matInverse* vec;
			error = vec - resultVec;
			Assert::AreEqual(0.0, error.length(), 0.1, L"Quaternion::inverse() doesn't work for yaw-quaternions");

			matRot.setRotation(quatPitch);
			matInverse.setRotation(quatPitch.inverse());
			resultVec = matRot * matInverse* vec;
			error = vec - resultVec;
			Assert::AreEqual(0.0, error.length(), 0.1, L"Quaternion::inverse() doesn't work for pitch-quaternions");

			matRot.setRotation(quatRoll);
			matInverse.setRotation(quatRoll.inverse());
			resultVec = matRot * matInverse* vec;
			error = vec - resultVec;
			Assert::AreEqual(0.0, error.length(), 0.1, L"Quaternion::inverse() doesn't work for roll-quaternions");
		}
		TEST_METHOD(Geometry_QuaternionTimesPoint3D)
		{
			Quaternion quatYaw = Quaternion(1.57, 0.0, 0.0);
			Quaternion quatPitch = Quaternion(0.0, 1.57, 0.0);
			Quaternion quatRoll = Quaternion(0.0, 0.0, 1.57);
			Quaternion quatYawPitch = Quaternion(1.57, 1.57, 0.0);
			
			Matrix44 matYaw, matPitch, matRoll, matYawPitch;
			matYaw.setRotation(quatYaw);
			matPitch.setRotation(quatPitch);
			matRoll.setRotation(quatRoll);
			matYawPitch.setRotation(quatYawPitch);

			Point3D point = Point3D(1.0, 1.0, 1.0);
			Point3D rotatedPoint;
			double error;

			rotatedPoint = matYaw * point;
			error = (Point3D(1.0,1.0,-1.0) - rotatedPoint).length();
			if (error>0.1)
				Assert::Fail(L"Yaw rotation doesn't work as expected");
			rotatedPoint = matPitch * point;
			error = (Point3D(1.0, -1.0, 1.0) - rotatedPoint).length();
			if (error>0.1)
				Assert::Fail(L"Pitch rotation doesn't work as expected");
			rotatedPoint = matRoll * point;
			error = (Point3D(-1.0, 1.0, 1.0) - rotatedPoint).length();
			if (error>0.1)
				Assert::Fail(L"Roll rotation doesn't work as expected");

			Point3D expectedResult = Point3D(1.0, 1.0, 1.0);
			rotatedPoint = matPitch * matYaw * point;
			error = (rotatedPoint - expectedResult).length();
			if (error > 0.1)
				Assert::Fail(L"Multiplication of yaw and pitch rotation matrices doesn't work as expected");


			//expectedResult = Point3D(1.0, 1.0, 1.0);
			//rotatedPoint = matYawPitch * point;
			//error = (rotatedPoint - expectedResult).length();
			//if (error > 0.1)
			//	Assert::Fail(L"Yaw-pitch matrix multiplication doesn't work as expected");

			//matYawPitch.setRotation(quatYaw * quatPitch);
			//rotatedPoint = matYawPitch * point;
			//error = (rotatedPoint - expectedResult).length();
			//if (error > 0.1)
			//	Assert::Fail(L"Multiplication of yaw and pitch quaternions doesn't work as expected");
		}
		TEST_METHOD(Geometry_QuaternionTimesVector3D)
		{
			Quaternion quatYaw = Quaternion(1.57, 0.0, 0.0);
			Quaternion quatPitch = Quaternion(0.0, 1.57, 0.0);
			Quaternion quatRoll = Quaternion(0.0, 0.0, 1.57);
			Quaternion quatYawPitch = Quaternion(1.57, 1.57, 0.0);

			Matrix44 matYaw, matPitch, matRoll, matYawPitch;
			matYaw.setRotation(quatYaw);
			matPitch.setRotation(quatPitch);
			matRoll.setRotation(quatRoll);
			matYawPitch.setRotation(quatYawPitch);

			Vector3D point = Vector3D(1.0, 1.0, 1.0);
			Vector3D rotatedPoint;
			double error;

			rotatedPoint = matYaw * point;
			error = (Vector3D(1.0, 1.0, -1.0) - rotatedPoint).length();
			if (error>0.1)
				Assert::Fail(L"Yaw rotation doesn't work as expected");
			rotatedPoint = matPitch * point;
			error = (Vector3D(1.0, -1.0, 1.0) - rotatedPoint).length();
			if (error>0.1)
				Assert::Fail(L"Pitch rotation doesn't work as expected");
			rotatedPoint = matRoll * point;
			error = (Vector3D(-1.0, 1.0, 1.0) - rotatedPoint).length();
			if (error>0.1)
				Assert::Fail(L"Roll rotation doesn't work as expected");

			Vector3D expectedResult = Vector3D(1.0, 1.0, 1.0);
			rotatedPoint = matPitch * matYaw * point;
			error = (rotatedPoint - expectedResult).length();
			if (error > 0.1)
				Assert::Fail(L"Multiplication of yaw and pitch rotation matrices doesn't work as expected");

			//rotatedPoint = matYawPitch * point;
			//error = (rotatedPoint - expectedResult).length();
			//if (error > 0.1)
			//	Assert::Fail(L"Yaw-pitch matrix multiplication doesn't work as expected");

			//matYawPitch.setRotation(quatYaw * quatPitch);
			//rotatedPoint = matYawPitch * point;
			//error = (rotatedPoint - expectedResult).length();
			//if (error > 0.1)
			//	Assert::Fail(L"Multiplication of yaw and pitch quaternions doesn't work as expected");
		}
		TEST_METHOD(Geometry_Rotation2Dvs3D)
		{
			Point3D point3D = Point3D(1.0, 1.0, 1.0);
			Quaternion rollQuat = Quaternion(0.0, 0.0, 1.57);
			Matrix44 rotMatrix;
			rotMatrix.setRotation(rollQuat);
			Point3D rotatedPoint3D = rotMatrix * point3D;

			Point2D point2D = Point2D(1.0, 1.0);
			Point2D rotatedPoint2D = rotMatrix * point2D;

			double error = sqrt(pow(rotatedPoint3D.x() - rotatedPoint2D.x(), 2.0) + pow(rotatedPoint3D.y() - rotatedPoint2D.y(), 2.0));
			Assert::AreEqual(0.0, error, 0.1, L"Missmatch between 2D and 3D roll rotations");

			point3D = Point3D(0.0, 1.0, 0.0);
			rotatedPoint3D = rotMatrix * point3D;
		}
	};
}