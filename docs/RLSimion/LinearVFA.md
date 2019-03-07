# Class LinearVFA
> Source: vfa.cpp
## Methods
### ``double get(const FeatureList *pFeatures,bool bUseFrozenWeights)``
* **Summary**
  Returns the value of the linear Value Function Approximator for the input state-action given as a list of features.
* **Parameters**
* * _pFeatures_: Input list of features
* * _bUseFrozenWeights_: Flag used to determine whether to use the online or target function
* **Return Value**
  Returns the value of the linear Value Function Approximator for the input state-action given as a list of features.
### ``void saturateOutput(double min, double max)``
* **Summary**
  Sets the function to saturate its output in range [min,max]
### ``void setIndexOffset(unsigned int offset)``
* **Summary**
  Sets the index offset used. Handy if we want to represent f(s,a) with two different feature maps: one for the state and another one for the action
* **Parameters**
* * _offset_: Offset added to feature indices
### ``void add(const FeatureList* pFeatures, double alpha)``
* **Summary**
  Adds a feature list (each feature has an index and a factor) to the weights in the function. Some of the indices might not belong to this function
* **Parameters**
* * _pFeatures_: Feature list to be added
* * _alpha_: Gain parameter used to move current weights toward those in the feature list
### ``void set(size_t feature, double value)``
* **Summary**
  Sets the value of a function weight
