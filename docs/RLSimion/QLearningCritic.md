# Class QLearningCritic
> Source: q-learners.cpp
## Methods
### ``double update(const State *s, const Action *a, const State *s_p, double r, double probability)``
* **Summary**
  Updates the estimate of the Q-function using the Q-Learning update rule with tuple {s,a,s_p,r}
* **Parameters**
  * _s_: Initial state
  * _a_: Action
  * _s_p_: Resultant state
  * _r_: Reward
* **Return Value**
  Updates the estimate of the Q-function using the Q-Learning update rule with tuple {s,a,s_p,r}
