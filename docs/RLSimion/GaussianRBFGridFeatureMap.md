# Class GaussianRBFGridFeatureMap
> Source: featuremap-rbfgrid.cpp
## Methods
### ``void map(vector<SingleDimensionGrid*>& grids, const vector<double>& values, FeatureList* outFeatures)``
* **Summary**
  Implements a Gaussian Radial-Basis feature mapping function that maps state-actions to feature.
* **Parameters**
  * _grids_: Input grids for every state-variable used
  * _values_: The values of every state-variable used
  * _outFeatures_: The output list of features
### ``void unmap(size_t feature, vector<SingleDimensionGrid*>& grids, vector<double>& outValues)``
* **Summary**
  Inverse of the feature mapping operation. Given a feature it returns the state-action to which it corresponds.
* **Parameters**
  * _feature_: The index of the feature
  * _grids_: The set of grids used to discretize each variable
  * _outValues_: The set of output values for every state-action variable
