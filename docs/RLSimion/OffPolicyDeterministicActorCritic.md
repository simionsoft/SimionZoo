# Class OffPolicyDeterministicActorCritic
> Source: actor-critic-opdac.cpp
## Methods
### ``double update(const State *s, const Action *a, const State *s_p, double r, double behaviorProb)``
* **Summary**
  Updates the policy and the value function using the Incremental Natural Actor Critic algorithm in "Off-policy deterministic actorcritic (OPDAC)" in "Deterministic Policy Gradient Algorithms" (David Silver, Guy Lever, Nicolas Heess, Thomas Degris, Daan Wierstra, Martin Riedmiller). Proceedings of the 31 st International Conference on Machine Learning, Beijing, China, 2014. JMLR: WCP volume 32
* **Parameters**
* * _s_: Initial state
* * _a_: Action
* * _s_p_: Resultant state
* * _r_: Reward
* * _behaviorProb_: Probability by which the actor selected the action
* **Return Value**
  Updates the policy and the value function using the Incremental Natural Actor Critic algorithm in "Off-policy deterministic actorcritic (OPDAC)" in "Deterministic Policy Gradient Algorithms" (David Silver, Guy Lever, Nicolas Heess, Thomas Degris, Daan Wierstra, Martin Riedmiller). Proceedings of the 31 st International Conference on Machine Learning, Beijing, China, 2014. JMLR: WCP volume 32
### ``double selectAction(const State *s, Action *a)``
* **Summary**
  The actor selects an action following the policies it is learning
* **Parameters**
* * _s_: Initial state
* * _a_: Action
* **Return Value**
  The actor selects an action following the policies it is learning
