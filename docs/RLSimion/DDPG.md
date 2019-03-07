# Class DDPG
> Source: DDPG.cpp
## Methods
### ``double selectAction(const State * s, Action * a)``
* **Summary**
  Implements action selection for the DDPG algorithm adding the output of the actor and exploration noise signal
* **Parameters**
  * _s_: State
  * _a_: Output action
### ``double update(const State * s, const Action * a, const State * s_p, double r, double behaviorProb)``
* **Summary**
  Updates the critic and actor using the DDPG algorithm
* **Parameters**
  * _s_: Initial state
  * _a_: Action
  * _s_p_: Resultant state
  * _r_: Reward
