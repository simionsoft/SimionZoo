# Class Badger.ViewModels.HerdAgentSelectionViewModel
> Source: HerdAgentSelectionViewModel.cs
## Methods
### `void AddInOrder(BindableCollection<int> intList, int newInt)`
* *Summary*
  Adds in order to a list of ints if input int is not on the list
* *Parameters*
  * _intList_: in-out list where the new integer is added
  * _newInt_: new integer to be added to the list
### `void AddInOrder(BindableCollection<string> stringList, string newString)`
* *Summary*
  Adds in order to a list of strings if input string is not on the list, assuming all strings are formatted using version numbers or ip addresses (xxx.xxx.xxx.xxx)
* *Parameters*
  * _stringList_: in-out string list
  * _newString_: new string to be added
