# Class Badger.ViewModels.MonitorWindowViewModel
> Source: MonitorWindowViewModel.cs
## Methods
### `public MonitorWindowViewModel()`
* *Summary*
  Constructor.
* *Parameters*
  * _MainWindowViewModel.Instance.LogToFile_: 
  * _batchFileName_: 
### `void CleanExperimentBatch()`
* *Summary*
  Deletes all the log files in the batch
### `void RunExperimentBatch()`
* *Summary*
  Runs the selected experiment in the experiment editor.
### `void ShowReports()`
* *Summary*
  Shows a Report window with the data of the currently finished experiment(s) already load and ready to make reports.
### `void SelectExperimentBatchFile()`
* *Summary*
  Shows a dialog to select which experiment batch file to load and loads it
### `bool LoadExperimentBatch(string batchFileName)`
* *Summary*
  Initializes the experiments to be monitored through a batch file that contains all required data for the task.
* *Parameters*
  * _batchFileName_: The batch file with experiment data
### `void DispatchOnMessageReceived(Job job, string experimentId, string messageId, string messageContent)`
* *Summary*
  Callback method on message received. It dispatches it to the job view model of the job passes as argument
* *Parameters*
  * _job_: The job
  * _experimentId_: The identifier of the experimental unit
  * _messageId_: The message identifier
  * _messageContent_: Content of the message
### `void DispatchOnStateChanged(Job job, string experimentId, Monitoring.State state)`
* *Summary*
  Callback method executed when the state of an experimental unit changes. This method dispatches it to the correct job view model handling that job
* *Parameters*
  * _job_: The job
  * _experimentId_: The experiment identifier
  * _state_: The state
### `void DispatchOnAllStatesChanged(Job job, Monitoring.State state)`
* *Summary*
  Callback method called when all the states of a job change. It dispatches the change to the correct job view model handling that job
* *Parameters*
  * _job_: The job
  * _state_: The state
### `void DispatchOnExperimentalUnitLaunched(Job job, ExperimentalUnit expUnit)`
* *Summary*
  Callback method on experimental unit launched that dispatches the event to the correct job view model
* *Parameters*
  * _job_: The job
  * _expUnit_: The exp unit
### `void OnJobAssigned(Job job)`
* *Summary*
  Callback method used to inform the monitor window view model when that a job has been assigned
* *Parameters*
  * _job_: The job.
### `void OnJobFinished(Job job)`
* *Summary*
  Callback method executed when a job is finished
* *Parameters*
  * _job_: The job.
### `void SetRunning(bool running)`
* *Summary*
  Sets the state as running
### `void RunExperimentsAsync(List<HerdAgentInfo> freeHerdAgents)`
* *Summary*
  Async method that runs all the experiments using the free herd agents
* *Parameters*
  * _freeHerdAgents_: The free herd agents.
### `bool ExistsRequiredFile(string file)`
* *Summary*
  Checks whether a required file to run an experiment exits or not.
* *Parameters*
  * _file_: The file to check
* *Return Value*
  Checks whether a required file to run an experiment exits or not.
### `double CalculateGlobalProgress()`
* *Summary*
  Calculate the global progress of experiments in queue.
* *Return Value*
  Calculate the global progress of experiments in queue.
### `void StopExperiments()`
* *Summary*
  Stops all experiments in progress.
