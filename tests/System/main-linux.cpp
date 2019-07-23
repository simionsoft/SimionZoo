#include <iostream>
#include "Process.cpp"
int main()
{
  int retCode= 0;

  try
  {
    SystemTests::System_Tests::Process_Run_Stop();
    std::cout << "Passed Process_Run_Stop()\n";
  }
  catch(std::runtime_error error)
  {
    retCode= 1;
    std::cout << "Failed Process_Run_Stop()\n";
  }
  try
  {
    SystemTests::System_Tests::Process_Run_Wait();
    std::cout << "Passed Process_Run_Wait()\n";
  }
  catch(std::runtime_error error)
  {
    retCode= 1;
    std::cout << "Failed Process_Run_Wait()\n";
  }
  try
  {
    SystemTests::System_Tests::NamedPipes();
    std::cout << "Passed NamedPipes()\n";
  }
  catch(std::runtime_error error)
  {
    retCode= 1;
    std::cout << "Failed NamedPipes()\n";
  }
  try
  {
    SystemTests::System_Tests::CrossPlatform_CStrings();
    std::cout << "Passed CrossPlatform_CStrings()\n";
  }
  catch(std::runtime_error error)
  {
    retCode= 1;
    std::cout << "Failed CrossPlatform_CStrings()\n";
  }
  try
  {
    SystemTests::System_Tests::CrossPlatform_Memory();
    std::cout << "Passed CrossPlatform_Memory()\n";
  }
  catch(std::runtime_error error)
  {
    retCode= 1;
    std::cout << "Failed CrossPlatform_Memory()\n";
  }
  try
  {
    SystemTests::System_Tests::CrossPlatform_Split();
    std::cout << "Passed CrossPlatform_Split()\n";
  }
  catch(std::runtime_error error)
  {
    retCode= 1;
    std::cout << "Failed CrossPlatform_Split()\n";
  }
  return retCode;
}
