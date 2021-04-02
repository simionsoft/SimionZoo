#include <iostream>
#include <stdexcept>
#include "unittest1.cpp"
int main()
{
  int retCode= 0;

  try
  {
    BasicGeometryChecks::GeometryLibTest::Geometry_BoundingBoxInsideFrustum();
    std::cout << "Passed Geometry_BoundingBoxInsideFrustum()\n";
  }
  catch(std::runtime_error error)
  {
    retCode= 1;
    std::cout << "Failed Geometry_BoundingBoxInsideFrustum()\n";
  }
  try
  {
    BasicGeometryChecks::GeometryLibTest::Geometry_BoundingBoxInsideFrustum2();
    std::cout << "Passed Geometry_BoundingBoxInsideFrustum2()\n";
  }
  catch(std::runtime_error error)
  {
    retCode= 1;
    std::cout << "Failed Geometry_BoundingBoxInsideFrustum2()\n";
  }
  try
  {
    BasicGeometryChecks::GeometryLibTest::Geometry_BoundingBoxInsideFrustum3();
    std::cout << "Passed Geometry_BoundingBoxInsideFrustum3()\n";
  }
  catch(std::runtime_error error)
  {
    retCode= 1;
    std::cout << "Failed Geometry_BoundingBoxInsideFrustum3()\n";
  }
  try
  {
    BasicGeometryChecks::GeometryLibTest::Geometry_BoundingBoxInsideFrustum4();
    std::cout << "Passed Geometry_BoundingBoxInsideFrustum4()\n";
  }
  catch(std::runtime_error error)
  {
    retCode= 1;
    std::cout << "Failed Geometry_BoundingBoxInsideFrustum4()\n";
  }
  try
  {
    BasicGeometryChecks::GeometryLibTest::Geometry_PointInsideFrustum();
    std::cout << "Passed Geometry_PointInsideFrustum()\n";
  }
  catch(std::runtime_error error)
  {
    retCode= 1;
    std::cout << "Failed Geometry_PointInsideFrustum()\n";
  }
  try
  {
    BasicGeometryChecks::GeometryLibTest::Geometry_PointInsideFrustum2();
    std::cout << "Passed Geometry_PointInsideFrustum2()\n";
  }
  catch(std::runtime_error error)
  {
    retCode= 1;
    std::cout << "Failed Geometry_PointInsideFrustum2()\n";
  }
  try
  {
    BasicGeometryChecks::GeometryLibTest::Geometry_EulerToQuaternion();
    std::cout << "Passed Geometry_EulerToQuaternion()\n";
  }
  catch(std::runtime_error error)
  {
    retCode= 1;
    std::cout << "Failed Geometry_EulerToQuaternion()\n";
  }
  try
  {
    BasicGeometryChecks::GeometryLibTest::Geometry_RotationOnXYPlane();
    std::cout << "Passed Geometry_RotationOnXYPlane()\n";
  }
  catch(std::runtime_error error)
  {
    retCode= 1;
    std::cout << "Failed Geometry_RotationOnXYPlane()\n";
  }
  try
  {
    BasicGeometryChecks::GeometryLibTest::Geometry_CameraTransformations();
    std::cout << "Passed Geometry_CameraTransformations()\n";
  }
  catch(std::runtime_error error)
  {
    retCode= 1;
    std::cout << "Failed Geometry_CameraTransformations()\n";
  }
  try
  {
    BasicGeometryChecks::GeometryLibTest::Geometry_QuaternionInverse();
    std::cout << "Passed Geometry_QuaternionInverse()\n";
  }
  catch(std::runtime_error error)
  {
    retCode= 1;
    std::cout << "Failed Geometry_QuaternionInverse()\n";
  }
  try
  {
    BasicGeometryChecks::GeometryLibTest::Geometry_QuaternionTimesPoint3D();
    std::cout << "Passed Geometry_QuaternionTimesPoint3D()\n";
  }
  catch(std::runtime_error error)
  {
    retCode= 1;
    std::cout << "Failed Geometry_QuaternionTimesPoint3D()\n";
  }
  try
  {
    BasicGeometryChecks::GeometryLibTest::Geometry_QuaternionTimesVector3D();
    std::cout << "Passed Geometry_QuaternionTimesVector3D()\n";
  }
  catch(std::runtime_error error)
  {
    retCode= 1;
    std::cout << "Failed Geometry_QuaternionTimesVector3D()\n";
  }
  try
  {
    BasicGeometryChecks::GeometryLibTest::Geometry_Rotation2Dvs3D();
    std::cout << "Passed Geometry_Rotation2Dvs3D()\n";
  }
  catch(std::runtime_error error)
  {
    retCode= 1;
    std::cout << "Failed Geometry_Rotation2Dvs3D()\n";
  }
  return retCode;
}
