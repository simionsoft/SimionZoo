# Class CACLALearner
> Source: actor-cacla.cpp
## Methods
### ``void update (const State *s, const Action *a, const State *s_p, double r, double td)``
* **Summary**:  
  Updates the policy using the CACLA update rule  
* **Parameters**:  
  * _s_: Initial state
  * _a_: Action
  * _s_p_: Resultant state
  * _r_: Reward
  * _td_: Temporal-Difference error
