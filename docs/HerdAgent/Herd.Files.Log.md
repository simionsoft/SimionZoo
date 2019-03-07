# Class Herd.Files.Log
> Source: LogFile.cs
## Methods
### ``bool ReadStep(BinaryReader logReader, int numLoggedVariables)``
* **Summary**
  Reads the data from a single step from the log file
* **Parameters**
  * _logReader_: The log reader
  * _numLoggedVariables_: The number of variables in the log file
* **Return Value**
  Reads the data from a single step from the log file
### ``void ReadEpisodeHeader(BinaryReader logReader)``
* **Summary**
  Reads the episode header.
* **Parameters**
  * _logReader_: The log reader.
### ``bool LoadBinaryLog(string LogFileName)``
* **Summary**
  Read the binary log file. To know whether the log information has been succesfully loaded or not, BinFileLoadSuccess can be checked after calling this method.
* **Return Value**
  Read the binary log file. To know whether the log information has been succesfully loaded or not, BinFileLoadSuccess can be checked after calling this method.
