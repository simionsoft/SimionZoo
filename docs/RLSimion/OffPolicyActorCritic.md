# Class OffPolicyActorCritic
> Source: actor-critic-offpac.cpp
## Methods
### ``double update (const State *s, const Action *a, const State *s_p, double r, double behaviorProb)``
* **Summary**:  
  Updates the policy and the value function using the Incremental Natural Actor Critic algorithm in "Off-Policy Actor-Critic" (Thomas Degris, Martha White, Richard S. Sutton), Proceedings of the 29 th International Conference on Machine Learning, Edinburgh, Scotland, UK, 2012. arXiv:1205.4839v5 [cs.LG] 20 Jun 2013  
* **Parameters**:  
  * _s_: Initial state
  * _a_: Action
  * _s_p_: Resultant state
  * _r_: Reward
  * _behaviorProb_: Probability by which the actor selected the action
### ``double selectAction (const State *s, Action *a)``
* **Summary**:  
  The actor selects an action following the policies it is learning  
* **Parameters**:  
  * _s_: Initial state
  * _a_: Action
