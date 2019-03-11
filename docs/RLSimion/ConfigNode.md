# Class ConfigNode
> Source: config.cpp
## Methods
### `int countChildren(const char* name)`
* *Summary*
  Returns the number of children this node has with the given name
### `bool getConstBoolean(const char* paramName, bool defaultValue)`
* *Summary*
  Retrieves the value of a parameter as a boolean
* *Parameters*
  * _paramName_: The name of the parameter
  * _defaultValue_: Its default value (will be used if the parameter is not found)
* *Return Value*
  Retrieves the value of a parameter as a boolean
### `int getConstInteger(const char* paramName, int defaultValue)`
* *Summary*
  Retrieves the value of a parameter as an integer
* *Parameters*
  * _paramName_: The name of the parameter
  * _defaultValue_: Its default value (will be used if the parameter is not found)
* *Return Value*
  Retrieves the value of a parameter as an integer
### `double getConstDouble(const char* paramName, double defaultValue)`
* *Summary*
  Retrieves the value of a parameter as a double
* *Parameters*
  * _paramName_: The name of the parameter
  * _defaultValue_: Its default value (will be used if the parameter is not found)
* *Return Value*
  Retrieves the value of a parameter as a double
### `void saveFile(const char* pFilename)`
* *Summary*
  Saves all the configuration nodes below the current to a file
* *Parameters*
  * _pFilename_: The path to the file
### `void saveFile(FILE* pFile)`
* *Summary*
  Saves all the configuration nodes below the current to an already open file
* *Parameters*
  * _pFile_: The handle to the already open file
### `void clone(ConfigFile* parameterFile)`
* *Summary*
  Makes a shallow copy of the a configuration file
* *Parameters*
  * _parameterFile_: 
