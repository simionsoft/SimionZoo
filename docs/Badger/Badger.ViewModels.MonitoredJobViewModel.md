# Class Badger.ViewModels.MonitoredJobViewModel
> Source: MonitoredJobViewModel.cs
## Methods
### `void OnMessageReceived(string experimentId, string messageId, string messageContent)`
* *Summary*
  Callback method that is called from the job dispatcher when a message is received
* *Parameters*
  * _experimentId_: The experiment identifier
  * _messageId_: The message identifier
  * _messageContent_: Content of the message
### `void OnStateChanged(string experimentId, Monitoring.State state)`
* *Summary*
  Called method executed when the state of an experimental unit changes
* *Parameters*
  * _experimentId_: The experiment identifier
  * _state_: The state
### `void OnAllStatesChanged(Monitoring.State state)`
* *Summary*
  Callback method called when the state of all the experimental unit in a job changes
* *Parameters*
  * _state_: The state.
### `void OnExperimentalUnitLaunched(ExperimentalUnit expUnit)`
* *Summary*
  Callback method executed when an experimental unit is launched
* *Parameters*
  * _expUnit_: The expererimental unit
