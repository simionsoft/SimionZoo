# Class IncrementalNaturalActorCritic
> Source: actor-critic-inac.cpp
## Methods
### ``double update(const State *s, const Action *a, const State *s_p, double r, double behaviorProb)``
* **Summary**
  Updates the policy and the value function using the Incremental Natural Actor Critic algorithm in "Model-free Reinforcement Learning with Continuous Action in Practice" (Thomas Degris, Patrick M. Pilarski , Richard S. Sutton), 2012 American Control Conference
* **Parameters**
  * _s_: Initial state
  * _a_: Action
  * _s_p_: Resultant state
  * _r_: Reward
  * _behaviorProb_: Probability by which the actor selected the action
* **Return Value**
  Updates the policy and the value function using the Incremental Natural Actor Critic algorithm in "Model-free Reinforcement Learning with Continuous Action in Practice" (Thomas Degris, Patrick M. Pilarski , Richard S. Sutton), 2012 American Control Conference
### ``double selectAction(const State *s, Action *a)``
* **Summary**
  The actor selects an action following the policies it is learning
* **Parameters**
  * _s_: Initial state
  * _a_: Action
* **Return Value**
  The actor selects an action following the policies it is learning
