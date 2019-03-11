# Class Badger.ViewModels.EditorWindowViewModel
> Source: EditorWindowViewModel.cs
## Methods
### `void NewExperiment()`
* *Summary*
  Creates a new experiment and adds it to the Editor tab
### `void SaveSelectedExperimentOrProject()`
* *Summary*
  Saves the selected experiment or project
### `void LoadExperimentalUnit(string experimentalUnitConfigFile)`
* *Summary*
  Loads an experimental unit on the Editor tab. This method can be used from any child window to load experimental units (i.e, from the ReportViewer)
* *Parameters*
  * _experimentalUnitConfigFile_: The experimental unit file.
### `void LoadExperimentOrProject()`
* *Summary*
  Shows a dialog window where the user can select an experiment or project for loading
### `void ClearExperiments()`
* *Summary*
  Clears the experiments tab
### `void Close(ExperimentViewModel e)`
* *Summary*
  Close a tab (experiment view) and removes it from experiments list.
* *Parameters*
  * _e_: The experiment to be removed
### `void RunExperimentalUnitLocallyWithCurrentParameters(ExperimentViewModel experiment)`
* *Summary*
  Runs locally the experiment with its currently selected parameters
* *Parameters*
  * _experiment_: The experiment to be run
### `void ShowWires(ExperimentViewModel experiment)`
* *Summary*
  Shows the wires defined in the current experiment
* *Parameters*
  * _experiment_: The selected experiment
### `void RunExperimentsInEditor()`
* *Summary*
  Runs all the experiments open in the editor. Saves a batch file read by the experiment monitor, and also a project to be able to modify the experiment and rerun it later
