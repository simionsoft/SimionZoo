# Class Badger.ViewModels.ConfigNodeViewModel
> Source: ConfigNodeViewModel.cs
## Methods
### ``void ForkThisNode(ConfigNodeViewModel originNode)``
* **Summary**
  Forks this node
* **Parameters**
  * _originNode_: The origin node.
### ``void LinkThisNode(ConfigNodeViewModel originNode)``
* **Summary**
  Take the right-clicked node as the origin node to link with all the posible linkable nodes (i.e. nodes of the same class). Linkable nodes CanBeLinked property value are set to true.
* **Parameters**
  * _originNode_: The origin node of the linking process
### ``void CancelLinking(ConfigNodeViewModel originNode)``
* **Summary**
  Cancel a linking process between two nodes.
### ``void Link(ConfigNodeViewModel targetNode)``
* **Summary**
  Actually perform the linking with the node.
* **Parameters**
  * _targetNode_: 
### ``void UnlinkNode()``
* **Summary**
  Unlink the node removing it from its origin linked nodes list and restore it to its original node class.
