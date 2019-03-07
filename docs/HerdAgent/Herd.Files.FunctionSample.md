# Class Herd.Files.FunctionSample
> Source: FunctionLog.cs
## Methods
### ``void ReadData(BinaryReader binaryReader, int sizeX, int sizeY)``
* **Summary**
  Reads the data of a function sample with size= sizeX*sizeY
* **Parameters**
  * _binaryReader_: The binary reader
  * _sizeX_: The size x
  * _sizeY_: The size y
### ``void CalculateValueRange(int sizeX, int sizeY, ref double minValue, ref double maxValue)``
* **Summary**
  Calculates the value range of the function
* **Parameters**
  * _sizeX_: The size x
  * _sizeY_: The size y
  * _minValue_: The minimum value. By reference so that we can use it to calculate the absolute min
  * _maxValue_: The maximum value. By reference so that we can use it to calculate the absolute max
### ``void GenerateBitmap(int sizeX, int sizeY, double minValue, double maxValue)``
* **Summary**
  This method must be called after reading the sample header and finding the declaration of the function to request the size of the function.
* **Parameters**
  * _binaryReader_: 
  * _sizeX_: 
  * _sizeY_: 
