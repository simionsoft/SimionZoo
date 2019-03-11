# Class World
> Source: world.cpp
## Methods
### `double getDT()`
* *Summary*
  This method returns the Delta_t used in the experiment
* *Return Value*
  This method returns the Delta_t used in the experiment
### `void reset(State *s)`
* *Summary*
  Reset state variables to the initial state from which simulations begin (it may be random)
* *Parameters*
  * _s_: State variable that holds the initial state
### `double executeAction(State *s, Action *a, State *s_p)`
* *Summary*
  Method called every control time-step. Internally it calculates calculates the length of the integration steps and calls several times DynamicModel::executeAction()
* *Parameters*
  * _s_: The variable with the current state values
  * _a_: The action to be executed
  * _s_p_: The variable that will hold the resultant state
* *Return Value*
  Method called every control time-step. Internally it calculates calculates the length of the integration steps and calls several times DynamicModel::executeAction()
