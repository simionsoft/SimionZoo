#include <iostream>
#include <stdexcept>
#include "Experiment.cpp"
#include "FeatureMaps.cpp"
#include "MemManager.cpp"
#include "NamedVarSets.cpp"
#include "SampleFile.cpp"
#include "StateActionVFAs.cpp"
#include "Utilities.cpp"
int main()
{
  int retCode= 0;

  try
  {
    ExperimentEpisodesSteps::ExperimentTest::Experiment_Episodes();
    std::cout << "Passed Experiment_Episodes()\n";
  }
  catch(std::runtime_error error)
  {
    retCode= 1;
    std::cout << "Failed Experiment_Episodes()\n";
  }
  try
  {
    ExperimentEpisodesSteps::ExperimentTest::Experiment_Episodes2();
    std::cout << "Passed Experiment_Episodes2()\n";
  }
  catch(std::runtime_error error)
  {
    retCode= 1;
    std::cout << "Failed Experiment_Episodes2()\n";
  }
  try
  {
    ExperimentEpisodesSteps::ExperimentTest::Experiment_Progress();
    std::cout << "Passed Experiment_Progress()\n";
  }
  catch(std::runtime_error error)
  {
    retCode= 1;
    std::cout << "Failed Experiment_Progress()\n";
  }
  try
  {
    ExperimentEpisodesSteps::ExperimentTest::Experiment_OnlyOneEpisode();
    std::cout << "Passed Experiment_OnlyOneEpisode()\n";
  }
  catch(std::runtime_error error)
  {
    retCode= 1;
    std::cout << "Failed Experiment_OnlyOneEpisode()\n";
  }
  try
  {
    VariableCircularity::UnitTest1::FeatureMap_RBFGrid_MapUnmapSweep();
    std::cout << "Passed FeatureMap_RBFGrid_MapUnmapSweep()\n";
  }
  catch(std::runtime_error error)
  {
    retCode= 1;
    std::cout << "Failed FeatureMap_RBFGrid_MapUnmapSweep()\n";
  }
  try
  {
    VariableCircularity::UnitTest1::FeatureMap_RBFGrid_VariableCircularity();
    std::cout << "Passed FeatureMap_RBFGrid_VariableCircularity()\n";
  }
  catch(std::runtime_error error)
  {
    retCode= 1;
    std::cout << "Failed FeatureMap_RBFGrid_VariableCircularity()\n";
  }
  try
  {
    VariableCircularity::UnitTest1::FeatureMap_Discrete_MapUnmapSweep();
    std::cout << "Passed FeatureMap_Discrete_MapUnmapSweep()\n";
  }
  catch(std::runtime_error error)
  {
    retCode= 1;
    std::cout << "Failed FeatureMap_Discrete_MapUnmapSweep()\n";
  }
  try
  {
    VariableCircularity::UnitTest1::FeatureMap_Discrete_VariableCircularity();
    std::cout << "Passed FeatureMap_Discrete_VariableCircularity()\n";
  }
  catch(std::runtime_error error)
  {
    retCode= 1;
    std::cout << "Failed FeatureMap_Discrete_VariableCircularity()\n";
  }
  try
  {
    VariableCircularity::UnitTest1::FeatureMap_TileCoding_MapUnmapSweep();
    std::cout << "Passed FeatureMap_TileCoding_MapUnmapSweep()\n";
  }
  catch(std::runtime_error error)
  {
    retCode= 1;
    std::cout << "Failed FeatureMap_TileCoding_MapUnmapSweep()\n";
  }
  try
  {
    MemManagerTest::UnitTest1::MemManager_TotalAllocated();
    std::cout << "Passed MemManager_TotalAllocated()\n";
  }
  catch(std::runtime_error error)
  {
    retCode= 1;
    std::cout << "Failed MemManager_TotalAllocated()\n";
  }
  try
  {
    MemManagerTest::UnitTest1::MemManager_InitValue();
    std::cout << "Passed MemManager_InitValue()\n";
  }
  catch(std::runtime_error error)
  {
    retCode= 1;
    std::cout << "Failed MemManager_InitValue()\n";
  }
  try
  {
    MemManagerTest::UnitTest1::MemManager_Copy();
    std::cout << "Passed MemManager_Copy()\n";
  }
  catch(std::runtime_error error)
  {
    retCode= 1;
    std::cout << "Failed MemManager_Copy()\n";
  }
  try
  {
    MemManagerTest::UnitTest1::MemManager_UpperBound();
    std::cout << "Passed MemManager_UpperBound()\n";
  }
  catch(std::runtime_error error)
  {
    retCode= 1;
    std::cout << "Failed MemManager_UpperBound()\n";
  }
  try
  {
    MemManagerTest::UnitTest1::MemManager_MemLimit();
    std::cout << "Passed MemManager_MemLimit()\n";
  }
  catch(std::runtime_error error)
  {
    retCode= 1;
    std::cout << "Failed MemManager_MemLimit()\n";
  }
  try
  {
    MemManagerTest::UnitTest1::MemManager_MemDiskDump();
    std::cout << "Passed MemManager_MemDiskDump()\n";
  }
  catch(std::runtime_error error)
  {
    retCode= 1;
    std::cout << "Failed MemManager_MemDiskDump()\n";
  }
  try
  {
    CNamedVarSets::UnitTest1::NamedVarSet_Circularity();
    std::cout << "Passed NamedVarSet_Circularity()\n";
  }
  catch(std::runtime_error error)
  {
    retCode= 1;
    std::cout << "Failed NamedVarSet_Circularity()\n";
  }
  try
  {
    SampleFilesTests::SampleFileTest::SampleFile_Small();
    std::cout << "Passed SampleFile_Small()\n";
  }
  catch(std::runtime_error error)
  {
    retCode= 1;
    std::cout << "Failed SampleFile_Small()\n";
  }
  try
  {
    SampleFilesTests::SampleFileTest::SampleFile_Medium();
    std::cout << "Passed SampleFile_Medium()\n";
  }
  catch(std::runtime_error error)
  {
    retCode= 1;
    std::cout << "Failed SampleFile_Medium()\n";
  }
  try
  {
    SampleFilesTests::SampleFileTest::SampleFile_Larger();
    std::cout << "Passed SampleFile_Larger()\n";
  }
  catch(std::runtime_error error)
  {
    retCode= 1;
    std::cout << "Failed SampleFile_Larger()\n";
  }
  try
  {
    StateActionVFA::UnitTest1::LinearStateActionVFA_ArgMax();
    std::cout << "Passed LinearStateActionVFA_ArgMax()\n";
  }
  catch(std::runtime_error error)
  {
    retCode= 1;
    std::cout << "Failed LinearStateActionVFA_ArgMax()\n";
  }
  try
  {
    StateActionVFA::UnitTest1::LinearStateActionVFA_FeatureMap();
    std::cout << "Passed LinearStateActionVFA_FeatureMap()\n";
  }
  catch(std::runtime_error error)
  {
    retCode= 1;
    std::cout << "Failed LinearStateActionVFA_FeatureMap()\n";
  }
  try
  {
    System::System_Windows::RLSimion_Utilities_getDirectory();
    std::cout << "Passed RLSimion_Utilities_getDirectory()\n";
  }
  catch(std::runtime_error error)
  {
    retCode= 1;
    std::cout << "Failed RLSimion_Utilities_getDirectory()\n";
  }
  try
  {
    System::System_Windows::RLSimion_Utilities_getFilename();
    std::cout << "Passed RLSimion_Utilities_getFilename()\n";
  }
  catch(std::runtime_error error)
  {
    retCode= 1;
    std::cout << "Failed RLSimion_Utilities_getFilename()\n";
  }
  try
  {
    System::System_Windows::RLSimion_Utilities_removeExtension();
    std::cout << "Passed RLSimion_Utilities_removeExtension()\n";
  }
  catch(std::runtime_error error)
  {
    retCode= 1;
    std::cout << "Failed RLSimion_Utilities_removeExtension()\n";
  }
  try
  {
    System::System_Windows::RLSimion_Utilities_getLastBarPos();
    std::cout << "Passed RLSimion_Utilities_getLastBarPos()\n";
  }
  catch(std::runtime_error error)
  {
    retCode= 1;
    std::cout << "Failed RLSimion_Utilities_getLastBarPos()\n";
  }
  return retCode;
}
