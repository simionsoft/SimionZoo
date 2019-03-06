# Class SimGod
> Source: simgod.cpp
## Methods
### ``double selectAction (State* s, Action* a)``
* **Summary**:  
  Iterates over all the Simions to let each of them select their actions  
* **Parameters**:  
  * _s_: Initial state
  * _a_: Action variable where Simions write their selected actions
### ``void update (State* s, Action* a, State* s_p, double r, double probability)``
* **Summary**:  
  Iterates over all the Simions to let them learn from the last real-time experience tuple  
* **Parameters**:  
  * _s_: Initial state
  * _a_: Action
  * _s_p_: Resultant state
  * _r_: Reward
  * _probability_: Probability by which the action was taken. Should be ignored
### ``void postUpdate ()``
* **Summary**:  
  If Experience-Replay is enabled, several tuples are taken from the buffer and given to the Simions to learn from them  
### ``void deferredLoad ()``
* **Summary**:  
  Iterates over all the objects implementing DeferredLoad to do all the heavyweight-lifting stuff  
### ``double getGamma ()``
* **Summary**:  
  Returns the value of gamma  
### ``int getTargetFunctionUpdateFreq ()``
* **Summary**:  
  Returns the number of steps after which deferred V-Function updates are to be done. 0 if we don't use Freeze-V-Function  
### ``bool bUpdateFrozenWeightsNow ()``
* **Summary**:  
  Returns whether we need to update the "frozen" copies of any function using Freeze-Target-Function  
### ``size_t getExperienceReplayUpdateSize ()``
* **Summary**:  
  Returns the number of each update batch using Experience-Replay  
