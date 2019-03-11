# Class TDCLambdaCritic
> Source: critic-tdc-lambda.cpp
## Methods
### `double update(const State *s, const Action *a, const State *s_p, double r, double rho)`
* *Summary*
  Updates the value function using the TDC update rule
* *Parameters*
  * _s_: Initial state
  * _a_: Action
  * _s_p_: Resultant state
  * _r_: Reward
  * _rho_: Importance sampling
* *Return Value*
  Updates the value function using the TDC update rule
