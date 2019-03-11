# Class Badger.ViewModels.PlotViewModel
> Source: PlotViewModel.cs
## Methods
### `void ResetAxes()`
* *Summary*
  Resets the axes of the plot to the default range [0,1]
### `void InitPlot(string title, string xAxisName, string yAxisName)`
* *Summary*
  Initializes the plot
* *Parameters*
  * _title_: The title
  * _xAxisName_: Name of the x axis
  * _yAxisName_: Name of the y axis
### `int AddLineSeries(string title, string description ="", bool isVisible = true)`
* *Summary*
  Adds a line series to the plot
* *Parameters*
  * _title_: The title of the series
  * _description_: The description of the series
  * _isVisible_: Initial visibility given to the series
* *Return Value*
  Adds a line series to the plot
### `void AddLineSeriesValue(int seriesIndex, double xValue, double yValue)`
* *Summary*
  Adds a vale to a given line series
* *Parameters*
  * _seriesIndex_: Index of the series
  * _xValue_: The x value
  * _yValue_: The y value
### `void ClearLineSeries()`
* *Summary*
  Clears the line series.
### `void HighlightLineSeries(int seriesId)`
* *Summary*
  Identify which LineSeries is hovered and make a call to the dimLineSeriesColor method passing the correct LineSeriesProperties object as parameter. In order to highlight a LineSeries what we actually do is to dim, that is, apply certain opacity, to all the other LineSeries.
* *Parameters*
  * _seriesId_: Id of the LineSeries to be highlighted
### `void ResetLineSeriesColors()`
* *Summary*
  Reset all LineSeries color to its original, removing the opacity in case that some was applied before by the highlightLineSeries method.
