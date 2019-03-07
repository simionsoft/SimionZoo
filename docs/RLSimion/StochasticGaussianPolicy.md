# Class StochasticGaussianPolicy
> Source: vfa-policy.cpp
## Methods
### ``double selectAction(const State *s, Action *a)``
* **Summary**
  A DeterministicPolicyGaussianNoise policy uses a function to represent the mean value of the function at each state and a second function to represent the variance of the output at each state. To calculate the policy's output, the two functions are evaluated for a state, and then the values used to sample a normal distribution, which is the actual output of the policy
* **Parameters**
* * _s_: Initial state
* * _a_: Output state
### ``void getFeatures(const State* state, FeatureList* outFeatureList)``
* **Summary**
  Uses the policy's feature map to return the features representing the state
* **Parameters**
* * _state_: State
* * _outFeatureList_: Output feature list
