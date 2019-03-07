# Class Herd.Network.HerdAgentInfo
> Source: HerdAgentInfo.cs
## Methods
### ``public HerdAgentInfo(string processorId, int numCPUCores, string architecture, string CUDAVersion, string herdAgentVersion)``
* **Summary**
  Constructor used for testing purposes
### ``void AddProperty(string name, string value)``
* **Summary**
  Adds a property to the agent description
* **Parameters**
* * _name_: The name of the property
* * _value_: The value of the property
### ``string Property(string name)``
* **Summary**
  Gets the value of the specified property
* **Parameters**
* * _name_: The name of the property
* **Return Value**
  Gets the value of the specified property
### ``void Parse(XElement xmlDescription)``
* **Summary**
  Parses the specified XML herd agent description filling the properties of the herd agent
* **Parameters**
* * _xmlDescription_: The XML description.
### ``AppVersion BestMatch(ExperimentalUnit experimentalUnit)``
* **Summary**
  Returns the app version of an experimental unit that best matches the herd agent
* **Parameters**
* * _experimentalUnit_: The experimental unit
* **Return Value**
  Returns the app version of an experimental unit that best matches the herd agent
