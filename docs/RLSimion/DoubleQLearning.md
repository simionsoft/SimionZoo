# Class DoubleQLearning
> Source: q-learners.cpp
## Methods
### `double update(const State *s, const Action *a, const State *s_p, double r, double probability)`
* *Summary*
  Updates the estimate of the Q-function using the Double Q-Learning update rule with tuple {s,a,s_p,r}. The main difference with respect to Q-function is that it uses two different sets of weights for the function, updating a random set of weights toward the other set of weights. Should offer better stability than regular Q-Learning
* *Parameters*
  * _s_: Initial state
  * _a_: Action
  * _s_p_: Resultant state
  * _r_: Reward
* *Return Value*
  Updates the estimate of the Q-function using the Double Q-Learning update rule with tuple {s,a,s_p,r}. The main difference with respect to Q-function is that it uses two different sets of weights for the function, updating a random set of weights toward the other set of weights. Should offer better stability than regular Q-Learning
