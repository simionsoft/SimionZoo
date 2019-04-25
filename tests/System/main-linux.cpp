#include <iostream>
#include "Process.cpp"
int main()
{
  bool success= true;

  try
  {
    SystemTests::System_Tests::Process_Run_Stop();
    std::cout << "Passed Process_Run_Stop()\n";
  }
  catch(std::runtime_error error)
  {
    success= false;
    std::cout << "Failed Process_Run_Stop()\n";
  }
  try
  {
    SystemTests::System_Tests::Process_Run_Wait();
    std::cout << "Passed Process_Run_Wait()\n";
  }
  catch(std::runtime_error error)
  {
    success= false;
    std::cout << "Failed Process_Run_Wait()\n";
  }
  try
  {
    SystemTests::System_Tests::NamedPipes();
    std::cout << "Passed NamedPipes()\n";
  }
  catch(std::runtime_error error)
  {
    success= false;
    std::cout << "Failed NamedPipes()\n";
  }
  try
  {
    SystemTests::System_Tests::CrossPlatform_CStrings();
    std::cout << "Passed CrossPlatform_CStrings()\n";
  }
  catch(std::runtime_error error)
  {
    success= false;
    std::cout << "Failed CrossPlatform_CStrings()\n";
  }
  try
  {
    SystemTests::System_Tests::CrossPlatform_Memory();
    std::cout << "Passed CrossPlatform_Memory()\n";
  }
  catch(std::runtime_error error)
  {
    success= false;
    std::cout << "Failed CrossPlatform_Memory()\n";
  }
  return success;
}
