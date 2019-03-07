# Class ExperienceReplay
> Source: experience-replay.cpp
## Methods
### ``bool bUsing()``
* **Summary**
  Returns whether Experience-Replay is enabled or not
### ``size_t getUpdateBatchSize()``
* **Summary**
  Returns the size of each update batch
### ``bool bHaveEnoughTuples()``
* **Summary**
  Returns whether there are enough tuples in the buffer to run a batch
### ``void addTuple(const State* s, const  Action* a, const State* s_p, double r, double probability)``
* **Summary**
  Adds an experience tuple to the circular buffer used
* **Parameters**
* * _s_: Initial state
* * _a_: Action
* * _s_p_: Resultant state
* * _r_: Reward
* * _probability_: Probability by which the action was taken
