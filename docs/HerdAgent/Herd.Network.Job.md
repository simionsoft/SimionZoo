# Class Herd.Network.Job
> Source: Job.cs
## Methods
### `public Job(List<ExperimentalUnit> experimentalUnits, HerdAgentInfo herdAgent)`
* *Summary*
  Constructor used from Network.Dispatcher
### `public Job()`
* *Summary*
  Parameter-less constructor using for reading a job from the network: Network.JobTransmitter
### `void PrepareForExecution()`
* *Summary*
  Prepares the job for execution, creating the appropriate tasks from the experimental units in the job
### `void AddInputFiles(List<string> source)`
* *Summary*
  Adds a list of input files
### `bool AddInputFile(string file)`
* *Summary*
  Adds one input file.
### `bool AddOutputFile(string file)`
* *Summary*
  Adds one output file
### `string RenamedFilename(string filename)`
* *Summary*
  Returns the filename renamed according to the renaming rules
* *Parameters*
  * _filename_: The filename.
* *Return Value*
  Returns the filename renamed according to the renaming rules
### `string OriginalFilename(string filename)`
* *Summary*
  Returns the original name of a renamed file
* *Parameters*
  * _filename_: The renamed filename
* *Return Value*
  Returns the original name of a renamed file
