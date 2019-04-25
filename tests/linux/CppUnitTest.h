#pragma once
#include <string>
#include <math.h>
using namespace std;
//license here
namespace Microsoft
{
  namespace VisualStudio
  {
    namespace CppUnitTestFramework
    {
      #define TEST_CLASS(x) class x
      #define TEST_METHOD(x) static void x()

      namespace Assert
      {
        void IsTrue(bool expr)
        {
          if (!expr) throw runtime_error("Assert failed");
        }
        void IsFalse(bool expr)
        {
          if (expr) throw runtime_error("Assert failed");
        }
        void Fail(std::wstring errorMessage)
        {
          throw runtime_error("Assert failed");
        }
        template <typename T>
        void AreEqual(T expected, T actual, double tolerance= 0.0000001, wstring errorMessage= L"")
        {
          if (abs(expected - actual) > abs(tolerance))
            throw runtime_error("Assert failed");
        }
      }
    }
  }
}
