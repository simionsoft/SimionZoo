# Class Badger.ViewModels.LogQueryViewModel
> Source: LogQueryViewModel.cs
## Methods
### `void AddGroupByFork(string forkName)`
* *Summary*
  Adds the fork to the list of group-by forks
* *Parameters*
  * _forkName_: Name of the fork.
### `bool IsForkUsedToGroup(string forkName)`
* *Summary*
  Returns whether a fork is used to group tracks or not
* *Parameters*
  * _forkName_: Name of the fork
### `void ResetGroupBy()`
* *Summary*
  Resets the forks used to group tracks
### `string GetVariableProcessFunc(string variable)`
* *Summary*
  Gets the process function used for the variable
* *Parameters*
  * _variable_: The variable.
* *Return Value*
  Gets the process function used for the variable
### `bool IsVariableSelected(string variable, string reportType)`
* *Summary*
  Returns whether the specified variable-reportType is selected
* *Parameters*
  * _variable_: The variable
  * _reportType_: Type of the report
### `void AddLogVariables(List<string> variables)`
* *Summary*
  Adds the variables to the list of variables in the log. Called when loading a logged experimental unit
* *Parameters*
  * _variables_: The variables
### `bool LogVariableExists(string variable)`
* *Summary*
  Returns whether the variable exists in th elog or not
* *Parameters*
  * _variable_: The variable.
### `void Validate()`
* *Summary*
  This function validates the current query and sets CanGenerateReports accordingly
### `void Execute(BindableCollection<LoggedExperimentViewModel> experiments
            ,LoadOptions.PerExperimentalUnitFunction OnExpUnitProcessed, out List<TrackGroup> resultTracks, out List<Report> reports)`
* *Summary*
  Executes the specified query.
* *Parameters*
  * _experiments_: The list of logged experiments on which the query will be processed
  * _OnExpUnitProcessed_: Callback function called when an exp unit is processed. Used to update the progress
  * _resultTracks_: Output list of track groups
  * _reports_: Output list of reports
