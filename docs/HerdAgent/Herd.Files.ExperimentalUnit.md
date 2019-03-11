# Class Herd.Files.ExperimentalUnit
> Source: ExperimentalUnit.cs
## Methods
### `public ExperimentalUnit(string name, List<AppVersion> appVersions, RunTimeRequirements runTimeRequirements)`
* *Summary*
  Minimal constructor for testing
* *Parameters*
  * _name_: 
### `public ExperimentalUnit(XmlNode configNode, string baseDirectory, LoadOptions loadOptions)`
* *Summary*
  Constructor used to load the experimental unit from a file
* *Parameters*
  * _configNode_: 
  * _baseDirectory_: 
  * _loadOptions_: 
### `bool LogFileExists(string relExplUnitPath, string baseDirectory)`
* *Summary*
  Use this if you want to load an experimental unit only depending on whether the log file exists or not
* *Parameters*
  * _logFileName_: 
* *Return Value*
  Use this if you want to load an experimental unit only depending on whether the log file exists or not
### `void RequestRuntimeRequirements()`
* *Summary*
  Executes the app to retrive the runtime requirements
### `bool IsHostArchitectureCompatible(AppVersion version)`
* *Summary*
  Returns whether an app version is compatible with the host architecture
* *Parameters*
  * _version_: The appversion.
### `AppVersion BestHostArchitectureMatch(List<AppVersion> appVersions)`
* *Summary*
  Returns the app version from the list that best matches the host architecture
* *Parameters*
  * _appVersions_: The application versions.
* *Return Value*
  Returns the app version from the list that best matches the host architecture
