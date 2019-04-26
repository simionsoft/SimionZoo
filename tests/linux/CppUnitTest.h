#pragma once
#include <string.h>
#include <cmath>
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
        void Fail(std::wstring errorMessage=L"")
        {
          throw runtime_error("Assert failed");
        }
        template <typename T>
        void AreEqual(T expected, T actual, T tolerance, wstring errorMessage)
        {
          if (std::abs(expected - actual) > tolerance)
            throw runtime_error("Assert failed");
        }
        template <typename T>
        void AreEqual(T expected, T actual, wstring errorMessage)
        {
          if (expected != actual)
            throw runtime_error("Assert failed");
        }
        template <typename T>
        void AreEqual(T expected, T actual)
        {
          if (expected != actual)
            throw runtime_error("Assert failed");
        }
        void AreEqual(const char* expected, const char* actual, wstring errorMessage= L"")
        {
          if (strcmp(expected,actual))
            throw runtime_error("Assert failed");
        }
        void AreEqual(string expected, string actual, wstring errorMessage= L"")
        {
          if (expected != actual)
            throw runtime_error("Assert failed");
        }
      }
    }
  }
}
