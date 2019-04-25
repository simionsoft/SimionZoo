#include <iostream>
#include "Experiment.cpp"
#include "FeatureMaps.cpp"
#include "MemManager.cpp"
#include "NamedVarSets.cpp"
#include "StateActionVFAs.cpp"
#include "Utilities.cpp"
using namespace ExperimentEpisodesSteps;
using namespace VariableCircularity;
using namespace MemManagerTest;
using namespace CNamedVarSets;
using namespace StateActionVFA;
using namespace System;
int main()
{
  bool success= true;

  try
  {
    ExperimentTest::Experiment_Episodes();
    std::cout << "Passed ExperimentTest::Experiment_Episodes()\n";
  }
  catch(std::runtime_error error)
  {
    success= false;
    std::cout << "Failed ExperimentTest::Experiment_Episodes()\n";
  }
  try
  {
    ExperimentTest::Experiment_Episodes2();
    std::cout << "Passed ExperimentTest::Experiment_Episodes2()\n";
  }
  catch(std::runtime_error error)
  {
    success= false;
    std::cout << "Failed ExperimentTest::Experiment_Episodes2()\n";
  }
  try
  {
    ExperimentTest::Experiment_Progress();
    std::cout << "Passed ExperimentTest::Experiment_Progress()\n";
  }
  catch(std::runtime_error error)
  {
    success= false;
    std::cout << "Failed ExperimentTest::Experiment_Progress()\n";
  }
  try
  {
    ExperimentTest::Experiment_OnlyOneEpisode();
    std::cout << "Passed ExperimentTest::Experiment_OnlyOneEpisode()\n";
  }
  catch(std::runtime_error error)
  {
    success= false;
    std::cout << "Failed ExperimentTest::Experiment_OnlyOneEpisode()\n";
  }
  try
  {
    UnitTest1::FeatureMap_RBFGrid_MapUnmapSweep();
    std::cout << "Passed UnitTest1::FeatureMap_RBFGrid_MapUnmapSweep()\n";
  }
  catch(std::runtime_error error)
  {
    success= false;
    std::cout << "Failed UnitTest1::FeatureMap_RBFGrid_MapUnmapSweep()\n";
  }
  try
  {
    UnitTest1::FeatureMap_RBFGrid_VariableCircularity();
    std::cout << "Passed UnitTest1::FeatureMap_RBFGrid_VariableCircularity()\n";
  }
  catch(std::runtime_error error)
  {
    success= false;
    std::cout << "Failed UnitTest1::FeatureMap_RBFGrid_VariableCircularity()\n";
  }
  try
  {
    UnitTest1::FeatureMap_Discrete_MapUnmapSweep();
    std::cout << "Passed UnitTest1::FeatureMap_Discrete_MapUnmapSweep()\n";
  }
  catch(std::runtime_error error)
  {
    success= false;
    std::cout << "Failed UnitTest1::FeatureMap_Discrete_MapUnmapSweep()\n";
  }
  try
  {
    UnitTest1::FeatureMap_Discrete_VariableCircularity();
    std::cout << "Passed UnitTest1::FeatureMap_Discrete_VariableCircularity()\n";
  }
  catch(std::runtime_error error)
  {
    success= false;
    std::cout << "Failed UnitTest1::FeatureMap_Discrete_VariableCircularity()\n";
  }
  try
  {
    UnitTest1::FeatureMap_TileCoding_MapUnmapSweep();
    std::cout << "Passed UnitTest1::FeatureMap_TileCoding_MapUnmapSweep()\n";
  }
  catch(std::runtime_error error)
  {
    success= false;
    std::cout << "Failed UnitTest1::FeatureMap_TileCoding_MapUnmapSweep()\n";
  }
  try
  {
    UnitTest1::MemManager_TotalAllocated();
    std::cout << "Passed UnitTest1::MemManager_TotalAllocated()\n";
  }
  catch(std::runtime_error error)
  {
    success= false;
    std::cout << "Failed UnitTest1::MemManager_TotalAllocated()\n";
  }
  try
  {
    UnitTest1::MemManager_InitValue();
    std::cout << "Passed UnitTest1::MemManager_InitValue()\n";
  }
  catch(std::runtime_error error)
  {
    success= false;
    std::cout << "Failed UnitTest1::MemManager_InitValue()\n";
  }
  try
  {
    UnitTest1::MemManager_Copy();
    std::cout << "Passed UnitTest1::MemManager_Copy()\n";
  }
  catch(std::runtime_error error)
  {
    success= false;
    std::cout << "Failed UnitTest1::MemManager_Copy()\n";
  }
  try
  {
    UnitTest1::MemManager_UpperBound();
    std::cout << "Passed UnitTest1::MemManager_UpperBound()\n";
  }
  catch(std::runtime_error error)
  {
    success= false;
    std::cout << "Failed UnitTest1::MemManager_UpperBound()\n";
  }
  try
  {
    UnitTest1::MemManager_MemLimit();
    std::cout << "Passed UnitTest1::MemManager_MemLimit()\n";
  }
  catch(std::runtime_error error)
  {
    success= false;
    std::cout << "Failed UnitTest1::MemManager_MemLimit()\n";
  }
  try
  {
    UnitTest1::MemManager_MemDiskDump();
    std::cout << "Passed UnitTest1::MemManager_MemDiskDump()\n";
  }
  catch(std::runtime_error error)
  {
    success= false;
    std::cout << "Failed UnitTest1::MemManager_MemDiskDump()\n";
  }
  try
  {
    UnitTest1::NamedVarSet_Circularity();
    std::cout << "Passed UnitTest1::NamedVarSet_Circularity()\n";
  }
  catch(std::runtime_error error)
  {
    success= false;
    std::cout << "Failed UnitTest1::NamedVarSet_Circularity()\n";
  }
  try
  {
    UnitTest1::LinearStateActionVFA_ArgMax();
    std::cout << "Passed UnitTest1::LinearStateActionVFA_ArgMax()\n";
  }
  catch(std::runtime_error error)
  {
    success= false;
    std::cout << "Failed UnitTest1::LinearStateActionVFA_ArgMax()\n";
  }
  try
  {
    UnitTest1::LinearStateActionVFA_FeatureMap();
    std::cout << "Passed UnitTest1::LinearStateActionVFA_FeatureMap()\n";
  }
  catch(std::runtime_error error)
  {
    success= false;
    std::cout << "Failed UnitTest1::LinearStateActionVFA_FeatureMap()\n";
  }
  try
  {
    System_Windows::RLSimion_Utilities_getDirectory();
    std::cout << "Passed System_Windows::RLSimion_Utilities_getDirectory()\n";
  }
  catch(std::runtime_error error)
  {
    success= false;
    std::cout << "Failed System_Windows::RLSimion_Utilities_getDirectory()\n";
  }
  try
  {
    System_Windows::RLSimion_Utilities_getFilename();
    std::cout << "Passed System_Windows::RLSimion_Utilities_getFilename()\n";
  }
  catch(std::runtime_error error)
  {
    success= false;
    std::cout << "Failed System_Windows::RLSimion_Utilities_getFilename()\n";
  }
  try
  {
    System_Windows::RLSimion_Utilities_removeExtension();
    std::cout << "Passed System_Windows::RLSimion_Utilities_removeExtension()\n";
  }
  catch(std::runtime_error error)
  {
    success= false;
    std::cout << "Failed System_Windows::RLSimion_Utilities_removeExtension()\n";
  }
  try
  {
    System_Windows::RLSimion_Utilities_getLastBarPos();
    std::cout << "Passed System_Windows::RLSimion_Utilities_getLastBarPos()\n";
  }
  catch(std::runtime_error error)
  {
    success= false;
    std::cout << "Failed System_Windows::RLSimion_Utilities_getLastBarPos()\n";
  }
  return success;
}
