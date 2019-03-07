# Class SimionApp
> Source: app.cpp
## Methods
### ``void printRequirements()``
* **Summary**
  This method prints the run-time requirements of an experiment instead of running it
### ``void registerStateActionFunction(string name, StateActionFunction* pFunction)``
* **Summary**
  Called from function-learning objects to register an instance of a function to log (if configured to do so)
* **Parameters**
  * _name_: Name of the function
  * _pFunction_: Pointer to the function
### ``void wireRegister(string name)``
* **Summary**
  Wires allow us to connect inputs with outputs. This method registers a wire by name
* **Parameters**
  * _name_: The new wire's name
### ``void wireRegister(string name, double minimum, double maximum)``
* **Summary**
  This method registers a wire by name and also sets its value range
* **Parameters**
  * _name_: 
  * _minimum_: 
  * _maximum_: 
### ``void registerTargetPlatformInputFile(const char* targetPlatform, const char* filepath, const char* rename)``
* **Summary**
  Used to register input files to a specific platform.
* **Parameters**
  * _targetPlatform_: Target platform: Win-32, Win-64, Linux-64,...
  * _filepath_: Path to the required file (exe, dll, data file, ...)
  * _rename_: Name given to the required file in the host machine
### ``void registerTargetPlatformOutputFile(const char* targetPlatform, const char* filepath)``
* **Summary**
  Used to register output files to a specific platform
* **Parameters**
  * _targetPlatform_: Target platform: Win-32, Win-64, Linux-64,...
  * _filepath_: Path to the required file (exe, dll, data file, ...)
### ``void registerInputFile(const char* filepath, const char* rename)``
* **Summary**
  Used to register input files common to all the target platforms
* **Parameters**
  * _filepath_: Path to the required file (exe, dll, data file, ...)
  * _rename_: Name given to the required file in the host machine
### ``void registerOutputFile(const char* filepath)``
* **Summary**
  Used to register output files common to all the target platforms
* **Parameters**
  * _filepath_: Path to the required file (exe, dll, data file, ...)
  * _rename_: Name given to the required file in the host machine
### ``void run()``
* **Summary**
  The app's main-loop that starts the simulation and runs until it finishes.
### ``void initRenderer(string sceneFile, State* s, Action* a)``
* **Summary**
  This method is called from run() and initializes the real-time rendering window (if configured to do so)
* **Parameters**
  * _sceneFile_: Name of the file with the definition of the scene
  * _s_: Current state
  * _a_: Current action
### ``void initFunctionSamplers(State* s, Action* a)``
* **Summary**
  After adding all the StateActionFunctions to be logged/drawn, this method is called to initialize them
* **Parameters**
  * _s_: State
  * _a_: Action
### ``void updateScene(State* s, Action* a)``
* **Summary**
  Updates the graphical objects bound in the scene file using the current value of their bounded state/action variables
* **Parameters**
  * _s_: 
  * _a_: 
