# Class ActorCritic
> Source: actor-critic.cpp
## Methods
### ``double selectAction (const State *s, Action *a)``
* **Summary**:  
  Objects that implement both an actor and a critic call the actor's selectAction() method  
* **Parameters**:  
  * _s_: Initial state
  * _a_: Output action
### ``double update (const State *s, const Action *a, const State *s_p, double r, double behaviorProb)``
* **Summary**:  
  Encapsulates the basic Actor-Critic update: the critic calculates the TD error and the actor updates its policy accordingly  
* **Parameters**:  
  * _s_: Initial state
  * _a_: Action
  * _s_p_: Resultant state
  * _r_: Reward
  * _behaviorProb_: Probability by which the actor selected the action. Should be ignored
