# Class Badger.Data.TrackGroup
> Source: TrackGroup.cs
## Methods
### `void Consolidate(string inGroupSelectionFunction, string inGroupSelectionVariable, string inGroupSelectionReportType
            , BindableCollection<string> groupBy)`
* *Summary*
  When grouping tracks by a fork, this function must be called to select a track inside the group. We call this "consolidating" the track group.
* *Parameters*
  * _inGroupSelectionFunction_: The function used to compare tracks inside the group
  * _inGroupSelectionVariable_: The variable used to evaluate tracks
  * _groupBy_: The list of forks used to group tracks
