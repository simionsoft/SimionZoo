#include "stdafx.h"
#include "CppUnitTest.h"
#include "../../../tools/GeometryLib/matrix44.h"
#include "../../../tools/GeometryLib/quaternion.h"
#include "../../../tools/GeometryLib/vector3d.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace BasicGeometryChecks
{		
	TEST_CLASS(GeometryLibTest)
	{
	public:
		TEST_METHOD(RotationOnXYPlane)
		{
			Matrix44 rotationMatrix;
			rotationMatrix.setRotation(Quaternion(0.0, 0.0, 1.57));
			Point2D p2D = Point2D(1.0, 1.0);
			Point2D rotatedP2D = rotationMatrix * p2D;
			Point2D expected = Point2D(1.0, -1.0);
			double error = (rotatedP2D - expected).length();
			if (error > 0.1)
				Assert::Fail(L"2D rotations on XY plane fail");

			Vector2D v2D = Vector2D(1.0, 1.0);
			Vector2D rotatedV2D = rotationMatrix * v2D;
			Vector2D expectedV = Vector2D(1.0, -1.0);
			error = (rotatedV2D - expectedV).length();
			if (error > 0.1)
				Assert::Fail(L"2D rotations on XY plane fail");

			Point3D p3D = Point3D(1.0, 1.0, 0.0);
			Point3D rotatedP3D = rotationMatrix * p3D;
			Point3D expectedP3 = Point3D(1.0, -1.0, 0.0);
			error = (rotatedP3D - expectedP3).length();
			if (error > 0.1)
				Assert::Fail(L"3D rotations on XY plane fail");

			Vector3D v3D = Vector3D(1.0, 1.0, 0.0);
			Vector3D rotatedV3D = rotationMatrix * v3D;
			Vector3D expectedV3 = Vector3D(1.0, -1.0, 0.0);
			error = (rotatedV3D - expectedV3).length();
			if (error > 0.1)
				Assert::Fail(L"3D rotations on XY plane fail");
		}
		TEST_METHOD(CameraTransformations)
		{
			Matrix44 cameraRotation;
			cameraRotation.setRotation(Quaternion(3.1415, 1.0, 0.0).inverse());
			Matrix44 cameraTranslation;
			cameraTranslation.setTranslation(Vector3D(-10.0, 10.0, 0.0).inverse());
			Matrix44 cameraTransform = cameraRotation * cameraTranslation;
			Point3D p1 = Point3D(0.0, 0.0, 0.0);
			Point3D p2 = Point3D(10.0, 0.0, 0.0);
			Point3D transP1 = cameraTransform * p1;
			Point3D transP2 = cameraTransform * p2;
			if (abs(transP1.y() - transP2.y()) > 0.1)
				Assert::Fail(L"Camera transformations with non-null yaw and pitch don't work properly");
			if (abs(transP1.x() - transP2.x() - 10.0)> 0.1)
				Assert::Fail(L"Camera transformations with non-null yaw and pitch don't work properly");
		}
		TEST_METHOD(QuaternionInverse)
		{
			double angle = 1.57;
			Quaternion quatYaw = Quaternion(angle, 0.0, 0.0);
			Quaternion quatPitch = Quaternion(0.0, angle, 0.0);
			Quaternion quatRoll = Quaternion(0.0, 0.0, angle);
			double resultAngle, error;
				
			resultAngle= quatYaw.inverse().yaw();
			error = abs(angle+resultAngle);
			if (error > 0.1)
				Assert::Fail(L"Quaternion::inverse() fails for yaw-quaternions");
			resultAngle = quatPitch.inverse().pitch();
			error = abs(angle + resultAngle);
			if (error > 0.1)
				Assert::Fail(L"Quaternion::inverse() fails for pitch-quaternions");
			resultAngle = quatRoll.inverse().roll();
			error = abs(angle + resultAngle);
			if (error > 0.1)
				Assert::Fail(L"Quaternion::inverse() fails for roll-quaternions");

			Quaternion quatYawPitch = Quaternion(angle,angle, 0.0);
			Quaternion inv = quatYawPitch.inverse();
			double invYaw = inv.yaw();
			double invPitch = inv.pitch();
			double invRoll = inv.roll();
			if (abs(angle + invYaw) > 0.01 || abs(angle + invPitch) > 0.01 || abs(0 + invRoll) > 0.01)
				Assert::Fail(L"Quaternion::inverse() failse for yaw-pitch quaternions");
			
			Matrix44 matYaw, matInvYaw;
			Point3D origin = Point3D(1.0,1.0,1.0), rotated;
			matYaw.setRotation(quatYaw);
			matInvYaw.setRotation(quatYaw.inverse());
			rotated = matYaw * matInvYaw * origin;
			error = (rotated - origin).length();
			if (error > 0.01)
				Assert::Fail(L"Quaternion and inverse quaternion matrices don't cancel each other");
			rotated = matInvYaw * matYaw * origin;
			error = (rotated - origin).length();
			if (error > 0.01)
				Assert::Fail(L"Quaternion and inverse quaternion matrices don't cancel each other");
		}
		TEST_METHOD(QuaternionTimesPoint3D)
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
			error = (Point3D(-1.0,1.0,1.0) - rotatedPoint).length();
			if (error>0.1)
				Assert::Fail(L"Yaw rotation doesn't work as expected");
			rotatedPoint = matPitch * point;
			error = (Point3D(1.0, 1.0, -1.0) - rotatedPoint).length();
			if (error>0.1)
				Assert::Fail(L"Pitch rotation doesn't work as expected");
			rotatedPoint = matRoll * point;
			error = (Point3D(1.0, -1.0, 1.0) - rotatedPoint).length();
			if (error>0.1)
				Assert::Fail(L"Roll rotation doesn't work as expected");

			Point3D expectedResult = Point3D(-1.0, 1.0, -1.0);
			rotatedPoint = matPitch * matYaw * point;
			error = (rotatedPoint - expectedResult).length();
			if (error > 0.1)
				Assert::Fail(L"Multiplication of yaw and pitch rotation matrices doesn't work as expected");

			rotatedPoint = matYawPitch * point;
			error = (rotatedPoint - expectedResult).length();
			if (error > 0.1)
				Assert::Fail(L"Yaw-pitch matrix multiplication doesn't work as expected");

			matYawPitch.setRotation(quatYaw * quatPitch);
			rotatedPoint = matYawPitch * point;
			error = (rotatedPoint - expectedResult).length();
			if (error > 0.1)
				Assert::Fail(L"Multiplication of yaw and pitch quaternions doesn't work as expected");
		}
		TEST_METHOD(QuaternionTimesVector3D)
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
			error = (Vector3D(-1.0, 1.0, 1.0) - rotatedPoint).length();
			if (error>0.1)
				Assert::Fail(L"Yaw rotation doesn't work as expected");
			rotatedPoint = matPitch * point;
			error = (Vector3D(1.0, 1.0, -1.0) - rotatedPoint).length();
			if (error>0.1)
				Assert::Fail(L"Pitch rotation doesn't work as expected");
			rotatedPoint = matRoll * point;
			error = (Vector3D(1.0, -1.0, 1.0) - rotatedPoint).length();
			if (error>0.1)
				Assert::Fail(L"Roll rotation doesn't work as expected");

			Vector3D expectedResult = Vector3D(-1.0, 1.0, -1.0);
			rotatedPoint = matPitch * matYaw * point;
			error = (rotatedPoint - expectedResult).length();
			if (error > 0.1)
				Assert::Fail(L"Multiplication of yaw and pitch rotation matrices doesn't work as expected");

			rotatedPoint = matYawPitch * point;
			error = (rotatedPoint - expectedResult).length();
			if (error > 0.1)
				Assert::Fail(L"Yaw-pitch matrix multiplication doesn't work as expected");

			matYawPitch.setRotation(quatYaw * quatPitch);
			rotatedPoint = matYawPitch * point;
			error = (rotatedPoint - expectedResult).length();
			if (error > 0.1)
				Assert::Fail(L"Multiplication of yaw and pitch quaternions doesn't work as expected");
		}
	};
}