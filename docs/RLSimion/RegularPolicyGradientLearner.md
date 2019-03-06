# Class RegularPolicyGradientLearner
> Source: actor-regular.cpp
## Methods
### ``void update (const State *s, const Action *a, const State *s_p, double r, double td)``
* **Summary**:  
  Updates the policies using a regular gradient-descent update rule  
* **Parameters**:  
  * _s_: Initial state
  * _a_: Action
  * _s_p_: Resultant state
  * _r_: Reward
  * _behaviorProb_: Probability by which the actor selected the action
