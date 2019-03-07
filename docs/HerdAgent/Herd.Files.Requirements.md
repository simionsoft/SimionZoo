# Class Herd.Files.Requirements
> Source: Requirements.cs
## Methods
### ``void AddRenameRule(string original, string rename)``
* **Summary**
  Rename rules: files that must be stored in the remote machine in a different relative location  Example: 64 bit runtime C++ libraries have the same name that 32-bit versions have. In the local machine, 64 bit libraries are in /bin/64, 32 libraries are in /bin, but both must be in the same directory as the .exe using them, so the 64 dll-s must be saved in /bin in the remote machine.
### ``void CommonInit(XmlNode node)``
* **Summary**
  Common initialization used by sub-classes of Requirements
* **Parameters**
  * _node_: 
