# Class Herd.Network.JobDispatcher
> Source: JobDispatcher.cs
## Methods
### ``void AssignExperiments(ref List<ExperimentalUnit> pendingExperiments
            , ref List<HerdAgentInfo> freeHerdAgents, ref List<Job> assignedJobs, JobDispatcherOptions options= null)``
* **Summary**
  Assigns experiments to available herd agents.
* **Parameters**
* * _freeHerdAgents_: 
### ``ExperimentalUnit FirstFittingExperiment(List<ExperimentalUnit> pendingExperiments, int numFreeCores, bool bAgentUsed, HerdAgentInfo agent)``
* **Summary**
  Returns the first experimental unit that fits the agent
* **Parameters**
* * _pendingExperiments_: The pending experimental units
* * _numFreeCores_: The number agent's free cores
* * _bAgentUsed_: Is the agent already being used for another experimental unit?
* * _agent_: The herd agent
* **Return Value**
  Returns the first experimental unit that fits the agent
