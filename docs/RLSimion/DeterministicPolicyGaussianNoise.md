# Class DeterministicPolicyGaussianNoise
> Source: vfa-policy.cpp
## Methods
### ``double selectAction(const State *s, Action *a)``
* **Summary**
  A DeterministicPolicyGaussianNoise policy uses a single function representing the deterministic output. In training episodes, the noise signal used for exploration is sampled and added to the deterministic output
* **Parameters**
  * _s_: Initial state
  * _a_: Output state
### ``void getFeatures(const State* state, FeatureList* outFeatureList)``
* **Summary**
  Uses the policy's feature map to return the features representing the state
* **Parameters**
  * _state_: State
  * _outFeatureList_: Output feature list
