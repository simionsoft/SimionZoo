# Class DQN
> Source: DQN.cpp
## Methods
### `double selectAction(const State * s, Action * a)`
* *Summary*
  Implements the action selection algorithm for a Q-based Deep RL algorithm
* *Parameters*
  * _s_: State
  * _a_: Output action
### `double update(const State * s, const Action * a, const State * s_p, double r, double behaviorProb)`
* *Summary*
  Implements DQL algorithm update using only one Neural Network for both evaluation and update
* *Parameters*
  * _s_: Initial state
  * _a_: Action
  * _s_p_: Resultant state
  * _r_: Reward
