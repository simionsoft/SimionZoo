# Class Badger.Data.LogFileUtils
> Source: LogFileUtils.cs
## Methods
### `Series GetVariableData(Log.EpisodesData episode, Report trackParameters, int variableIndex)`
* *Summary*
  Gets the data of a variable from an episode using the parameters of the target track
* *Parameters*
  * _episode_: The episode
  * _trackParameters_: The track parameters
  * _variableIndex_: Index of the variable
* *Return Value*
  Gets the data of a variable from an episode using the parameters of the target track
### `double GetEpisodeAverage(Log.EpisodesData episode, int variableIndex, Report trackParameters)`
* *Summary*
  Gets the average value of a variable in an episode using the track parameters
* *Parameters*
  * _episode_: The episode
  * _variableIndex_: Index of the variable
  * _trackParameters_: The track parameters
* *Return Value*
  Gets the average value of a variable in an episode using the track parameters
### `SeriesGroup GetAveragedData(List<Log.EpisodesData> episodes, Report trackParameters, int variableIndex)`
* *Summary*
  Gets the averaged data of the given variable from a list of episodes using the track parameters
* *Parameters*
  * _episodes_: The episode list
  * _trackParameters_: The track parameters
  * _variableIndex_: Index of the variable
* *Return Value*
  Gets the averaged data of the given variable from a list of episodes using the track parameters
### `Track LoadTrackData(LoggedExperimentalUnitViewModel expUnit, List<Report> reports)`
* *Summary*
  Creates a Track object from a logged experimental unit and a list of reports
* *Parameters*
  * _expUnit_: The logged experimental unit
  * _reports_: The list of reports we want
* *Return Value*
  Creates a Track object from a logged experimental unit and a list of reports
