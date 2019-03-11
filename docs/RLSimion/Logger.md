# Class Logger
> Source: logger-functions.cpp
## Methods
### `void openFunctionLogFile(const char* filename)`
* *Summary*
  Creates a file where functions will be logged
* *Parameters*
  * _filename_: Path to the output file
### `void closeFunctionLogFile()`
* *Summary*
  Closes the file used for logging functions
### `void writeFunctionLogSample()`
* *Summary*
  Adds a sample from each function to the log file
### `void setOutputFilenames()`
* *Summary*
  Registers the output files
### `bool isEpisodeTypeLogged(bool evalEpisode)`
* *Summary*
  Returns whether the given type of episode is being logged
* *Parameters*
  * _evalEpisode_: true if we want to query about evaluation episodes, false otherwise
### `void writeLogFileXMLDescriptor(const char* filename)`
* *Summary*
  Creates an XML file with the description of the log file: variables, scene file...
* *Parameters*
  * _filename_: 
### `void firstEpisode()`
* *Summary*
  Must be called before the first episode begins to initialize log files, timers,...
### `void firstStep()`
* *Summary*
  Must be called before the first step to write episode headers and the initial state in the log file. It also takes a snapshot of the functions and logs them if we are logging functions
### `void lastStep()`
* *Summary*
  Must be called after the last step in an episode has finished. The episode is marked as finished in the log file and, if the current is an evaluation episode, the progress is updated
### `void timestep(State* s, Action* a, State* s_p, Reward* r)`
* *Summary*
  Logs if needed a new step {s,a,s_p,r}, and adds a new sample to statistics
* *Parameters*
  * _s_: Initial state
  * _a_: Action
  * _s_p_: Resultant state
  * _r_: Reward
### `void logMessage(MessageType type, const char* message)`
* *Summary*
  Logging function that formats and dispatches different types of messages: info/warnings/errors, progress, and evaluation. Depending on whether the app is connected via a named pipe, the message is either sent via the pipe or printed on the system console. Error log messages throw an exception to terminate the program
* *Parameters*
  * _type_: 
  * _message_: 
