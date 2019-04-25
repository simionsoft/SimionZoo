#include <iostream>
#include "Process.cpp"
using namespace SystemTests;
int main()
{
  bool success= true;

  try
  {
    System_Tests::Process_Run_Stop();
    std::cout << "Passed System_Tests::Process_Run_Stop()\n";
  }
  catch(std::runtime_error error)
  {
    success= false;
    std::cout << "Failed System_Tests::Process_Run_Stop()\n";
  }
  try
  {
    System_Tests::Process_Run_Wait();
    std::cout << "Passed System_Tests::Process_Run_Wait()\n";
  }
  catch(std::runtime_error error)
  {
    success= false;
    std::cout << "Failed System_Tests::Process_Run_Wait()\n";
  }
  try
  {
    System_Tests::NamedPipes();
    std::cout << "Passed System_Tests::NamedPipes()\n";
  }
  catch(std::runtime_error error)
  {
    success= false;
    std::cout << "Failed System_Tests::NamedPipes()\n";
  }
  try
  {
    System_Tests::CrossPlatform_CStrings();
    std::cout << "Passed System_Tests::CrossPlatform_CStrings()\n";
  }
  catch(std::runtime_error error)
  {
    success= false;
    std::cout << "Failed System_Tests::CrossPlatform_CStrings()\n";
  }
  try
  {
    System_Tests::CrossPlatform_Memory();
    std::cout << "Passed System_Tests::CrossPlatform_Memory()\n";
  }
  catch(std::runtime_error error)
  {
    success= false;
    std::cout << "Failed System_Tests::CrossPlatform_Memory()\n";
  }
  return success;
}
