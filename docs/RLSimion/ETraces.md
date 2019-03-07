# Class ETraces
> Source: etraces.cpp
## Methods
### ``void update(double factor)``
* **Summary**
  Etraces implement a technique that updates recently visited states with the current reward. This method updates the factor of each trace, so that they decay with time according to parameter Lambda. Not compatible with Experience-Replay, which is currently favored.
* **Parameters**
  * _factor_: Update factor (depends on the learning algorithm)
### ``void addFeatureList(FeatureList* inList, double factor)``
* **Summary**
  This method adds current state's features to the traces
* **Parameters**
  * _inList_: Features of the current state
  * _factor_: Factor given to these features
