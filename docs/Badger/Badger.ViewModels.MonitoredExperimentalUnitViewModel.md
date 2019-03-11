# Class Badger.ViewModels.MonitoredExperimentalUnitViewModel
> Source: MonitoredExperimentalUnitViewModel.cs
## Methods
### `public MonitoredExperimentalUnitViewModel(ExperimentalUnit expUnit, PlotViewModel plot)`
* *Summary*
  Constructor
* *Parameters*
  * _expUnit_: The model: an instance of ExperimentalUnit
  * _plot_: The plot used to monitor data
### `void AddEvaluationValue(double xNorm, double y)`
* *Summary*
  Adds a (xNorm,y) value to the series of evaluations.
* *Parameters*
  * _xNorm_: The normalized value in x (0 is the beginning and 1 the end of the experiment)
  * _y_: The average reward obtained in this evaluation
