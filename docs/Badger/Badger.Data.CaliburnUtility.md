# Class Badger.Data.CaliburnUtility
> Source: CaliburnUtility.cs
## Methods
### ``void ShowPopupWindow(PropertyChangedBase viewModel, string windowHeader, bool isDialog = true)``
* **Summary**
  Show a pop-up window, can be a dialog, which once showed up does not allow interaction with the background window. It also can be an independent window, which does allow interaction with any other window of the application.
* **Parameters**
  * _viewModel_: ViewModel to be shown in the pop-up window
  * _windowHeader_: Title of the window.
  * _isDialog_: Whether its a dialog or a window.
### ``string SelectFolder(string initialDirectory)``
* **Summary**
  Show an emergent dialog to allow users to select directory paths.
* **Parameters**
  * _initialDirectory_: Where everything starts.
* **Return Value**
  Show an emergent dialog to allow users to select directory paths.
