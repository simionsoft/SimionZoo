# Class Badger.ViewModels.MainWindowViewModel
> Source: MainWindowViewModel.cs
## Methods
### ``public MainWindowViewModel()``
* **Summary**
  Class constructor.
### ``void ShowReportWindow()``
* **Summary**
  Show the report viewer tab
### ``void ShowExperimentMonitor()``
* **Summary**
  Shows the experiment monitor tab
### ``void ShowEditorWindow()``
* **Summary**
  Shows the editor tab
### ``void LogToFile(string logMessage)``
* **Summary**
  Logs a message with the current time-date. If the Badger log file doesn't exist, it creates it. Uses a lock to avoid multi-thread issues
* **Parameters**
  * _logMessage_: The log message.
