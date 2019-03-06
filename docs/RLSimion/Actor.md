# Class Actor
> Source: actor.cpp
## Methods
### ``double selectAction (const State *s, Action *a)``
* **Summary**:  
  Iterates over the actor's policy learners so that every one determines its output action  
* **Parameters**:  
  * _s_: Input initial state
  * _a_: Output action
### ``void update (const State* s, const Action* a, const State* s_p, double r, double td)``
* **Summary**:  
  Iterates over all the actor's policy learners so that every one learns from an experience tuple {s,a,s_p,r,td}  
* **Parameters**:  
  * _s_: Initial state
  * _a_: Action
  * _s_p_: Resultant state
  * _r_: Reward
  * _td_: Temporal-Difference error calculated by the critic
