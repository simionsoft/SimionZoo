# Class Experiment
> Source: experiment.cpp
## Methods
### ``double getExperimentProgress()``
* **Summary**
  Returns the progress of the experiment (normalized in range [0,1])
### ``double getTrainingProgress()``
* **Summary**
  The normalized progress taking only into account the training episodes (normalized in range [0,1])
### ``double getEpisodeProgress()``
* **Summary**
  Normalized progress with respect to the current episode in range [0,1]
### ``bool isEvaluationEpisode()``
* **Summary**
  Returns whether the current is an evaluation episode
### ``void reset()``
* **Summary**
  Resets the experiment to the starting conditions
### ``void nextStep()``
* **Summary**
  Increments the current step
### ``bool isValidStep()``
* **Summary**
  Returns whether the current step is valid or we have already finished the episode
### ``bool isValidEpisode()``
* **Summary**
  Returns whether the current episode is valid or we have already finished the experiment
### ``void nextEpisode()``
* **Summary**
  Used to advance the simulation to the next episode
### ``bool isFirstEpisode()``
* **Summary**
  Is this the first episode?
### ``bool isLastEpisode()``
* **Summary**
  Is this the last episode?
### ``void timestep(State* s, Action* a, State* s_p, Reward* r)``
* **Summary**
  Called every time-step. Controls when and what information to log, and also the timers to decide if the progress must be udated
* **Parameters**
  * _s_: Initial state of the last tuple
  * _a_: Action in the last tuple
  * _s_p_: Resultant state of the last tuple
  * _r_: Reward of the last tuple
