#include <iostream>
#include "unittest1.cpp"
using namespace BasicGeometryChecks;
int main()
{
  bool success= true;

  try
  {
    GeometryLibTest::Geometry_BoundingBoxInsideFrustum();
    std::cout << "Passed GeometryLibTest::Geometry_BoundingBoxInsideFrustum()\n";
  }
  catch(std::runtime_error error)
  {
    success= false;
    std::cout << "Failed GeometryLibTest::Geometry_BoundingBoxInsideFrustum()\n";
  }
  try
  {
    GeometryLibTest::Geometry_BoundingBoxInsideFrustum2();
    std::cout << "Passed GeometryLibTest::Geometry_BoundingBoxInsideFrustum2()\n";
  }
  catch(std::runtime_error error)
  {
    success= false;
    std::cout << "Failed GeometryLibTest::Geometry_BoundingBoxInsideFrustum2()\n";
  }
  try
  {
    GeometryLibTest::Geometry_BoundingBoxInsideFrustum3();
    std::cout << "Passed GeometryLibTest::Geometry_BoundingBoxInsideFrustum3()\n";
  }
  catch(std::runtime_error error)
  {
    success= false;
    std::cout << "Failed GeometryLibTest::Geometry_BoundingBoxInsideFrustum3()\n";
  }
  try
  {
    GeometryLibTest::Geometry_BoundingBoxInsideFrustum4();
    std::cout << "Passed GeometryLibTest::Geometry_BoundingBoxInsideFrustum4()\n";
  }
  catch(std::runtime_error error)
  {
    success= false;
    std::cout << "Failed GeometryLibTest::Geometry_BoundingBoxInsideFrustum4()\n";
  }
  try
  {
    GeometryLibTest::Geometry_PointInsideFrustum();
    std::cout << "Passed GeometryLibTest::Geometry_PointInsideFrustum()\n";
  }
  catch(std::runtime_error error)
  {
    success= false;
    std::cout << "Failed GeometryLibTest::Geometry_PointInsideFrustum()\n";
  }
  try
  {
    GeometryLibTest::Geometry_PointInsideFrustum2();
    std::cout << "Passed GeometryLibTest::Geometry_PointInsideFrustum2()\n";
  }
  catch(std::runtime_error error)
  {
    success= false;
    std::cout << "Failed GeometryLibTest::Geometry_PointInsideFrustum2()\n";
  }
  try
  {
    GeometryLibTest::Geometry_EulerToQuaternion();
    std::cout << "Passed GeometryLibTest::Geometry_EulerToQuaternion()\n";
  }
  catch(std::runtime_error error)
  {
    success= false;
    std::cout << "Failed GeometryLibTest::Geometry_EulerToQuaternion()\n";
  }
  try
  {
    GeometryLibTest::Geometry_RotationOnXYPlane();
    std::cout << "Passed GeometryLibTest::Geometry_RotationOnXYPlane()\n";
  }
  catch(std::runtime_error error)
  {
    success= false;
    std::cout << "Failed GeometryLibTest::Geometry_RotationOnXYPlane()\n";
  }
  try
  {
    GeometryLibTest::Geometry_CameraTransformations();
    std::cout << "Passed GeometryLibTest::Geometry_CameraTransformations()\n";
  }
  catch(std::runtime_error error)
  {
    success= false;
    std::cout << "Failed GeometryLibTest::Geometry_CameraTransformations()\n";
  }
  try
  {
    GeometryLibTest::Geometry_QuaternionInverse();
    std::cout << "Passed GeometryLibTest::Geometry_QuaternionInverse()\n";
  }
  catch(std::runtime_error error)
  {
    success= false;
    std::cout << "Failed GeometryLibTest::Geometry_QuaternionInverse()\n";
  }
  try
  {
    GeometryLibTest::Geometry_QuaternionTimesPoint3D();
    std::cout << "Passed GeometryLibTest::Geometry_QuaternionTimesPoint3D()\n";
  }
  catch(std::runtime_error error)
  {
    success= false;
    std::cout << "Failed GeometryLibTest::Geometry_QuaternionTimesPoint3D()\n";
  }
  try
  {
    GeometryLibTest::Geometry_QuaternionTimesVector3D();
    std::cout << "Passed GeometryLibTest::Geometry_QuaternionTimesVector3D()\n";
  }
  catch(std::runtime_error error)
  {
    success= false;
    std::cout << "Failed GeometryLibTest::Geometry_QuaternionTimesVector3D()\n";
  }
  try
  {
    GeometryLibTest::Geometry_Rotation2Dvs3D();
    std::cout << "Passed GeometryLibTest::Geometry_Rotation2Dvs3D()\n";
  }
  catch(std::runtime_error error)
  {
    success= false;
    std::cout << "Failed GeometryLibTest::Geometry_Rotation2Dvs3D()\n";
  }
  return success;
}
