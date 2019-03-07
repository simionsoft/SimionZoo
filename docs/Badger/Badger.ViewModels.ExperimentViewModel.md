# Class Badger.ViewModels.ExperimentViewModel
> Source: ExperimentViewModel.cs
## Methods
### ``public ExperimentViewModel(string appDefinitionFileName, string configFilename)``
* **Summary**
  This constructor builds the whole tree of ConfigNodes either - with default values ("New") or - with a configuration file ("Load")
* **Parameters**
  * _appDefinitionFileName_: 
  * _configFilename_: 
### ``void ShowWires()``
* **Summary**
  Shows a new window with the wires used in the experiment
### ``ConfigNodeViewModel DepthFirstSearch(string nodeName, string alias = "")``
* **Summary**
  Implementation of depth first search algorithm for experiment tree.
* **Parameters**
  * _targetNode_: 
### ``ConfigNodeViewModel DepthFirstSearch(ConfigNodeViewModel targetNode)``
* **Summary**
  Implementation of depth first search algorithm for experiment tree.
* **Parameters**
  * _targetNode_: 
### ``void CheckLinkableNodes(ConfigNodeViewModel originNode, bool link = true)``
* **Summary**
  
* **Parameters**
  * _originNode_: 
  * _link_: 
