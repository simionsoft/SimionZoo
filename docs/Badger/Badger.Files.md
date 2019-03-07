# Class Badger.Files
> Source: Files.cs
## Methods
### ``int SaveExperimentBatchFile(BindableCollection<ExperimentViewModel> experiments,
            string batchFilename, LogFunction log, ProgressUpdateFunction progressUpdateFunction)``
* **Summary**
  Save an experiment batch: the list of (possibly forked) experiments is saved a as set of experiments without forks and a .exp-batch file in the root directory referencing them all and the forks/values each one took.
* **Parameters**
* * _experiments_: 
* * _batchFilename_: 
* * _log_: 
* **Return Value**
  Save an experiment batch: the list of (possibly forked) experiments is saved a as set of experiments without forks and a .exp-batch file in the root directory referencing them all and the forks/values each one took.
### ``SaveFileDialog SaveFileDialog(string description, string filter, string suggestedFileName= null)``
* **Summary**
  Show a dialog used to save a file with an specific extension.
* **Parameters**
* * _description_: Short description of the file type
* * _filter_: Extension
* * _suggestedFileName_: Name suggested for output. Null by default
* **Return Value**
  Show a dialog used to save a file with an specific extension.
### ``string SelectOutputDirectoryDialog(string initialDirectory= null)``
* **Summary**
  Shows a dialog used to select a directory where files are to be saved. If something goes wrong, null is returned
* **Parameters**
* * _initialDirectory_: The directory from which to being browsing
* **Return Value**
  Shows a dialog used to select a directory where files are to be saved. If something goes wrong, null is returned
### ``bool OpenFileDialog(ref string fileName, string description, string extension)``
* **Summary**
  Open a file that fulfills the requirements passed as parameters.
* **Parameters**
* * _fileName_: The name of the file
* * _description_: Description of the file type
* * _extension_: The extension of the file type
* **Return Value**
  Open a file that fulfills the requirements passed as parameters.
