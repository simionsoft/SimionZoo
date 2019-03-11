# Class FeatureMap
> Source: featuremap.cpp
## Methods
### `void getFeatures(const State* s, const Action* a, FeatureList* outFeatures)`
* *Summary*
  Calculates the features for any given state-action
* *Parameters*
  * _s_: State
  * _a_: Action
  * _outFeatures_: Output feature list
### `void getFeatureStateAction(size_t feature, State* s, Action* a)`
* *Summary*
  Given a feature index, this method returns the state-action to which the feature corresponds. If the feature map uses only states, the output action is left unmodified
* *Parameters*
  * _feature_: Index of the feature
  * _s_: Output state
  * _a_: Output action
