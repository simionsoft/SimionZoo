# Class Badger.ViewModels.LinkedNodeViewModel
> Source: LinkedNodeViewModel.cs
## Methods
### ``public LinkedNodeViewModel(ExperimentViewModel parentExperiment, ConfigNodeViewModel originNode,
            ConfigNodeViewModel targetNode)``
* **Summary**
  Constructor used from the experiment editor
* **Parameters**
  * _parentExperiment_: 
  * _originNode_: 
  * _targetNode_: 
### ``public LinkedNodeViewModel(ExperimentViewModel parentExperiment, ConfigNodeViewModel parentNode,
            XmlNode classDefinition, XmlNode configNode = null)``
* **Summary**
  Constructor called when loading an experiment config file
* **Parameters**
  * _parentExperiment_: 
  * _parentNode_: 
  * _classDefinition_: Class of the node in app definitions
  * _parentXPath_: 
  * _configNode_: Node in simion.exp file with the configuration for a node class
### ``void CreateLinkedNode()``
* **Summary**
  
