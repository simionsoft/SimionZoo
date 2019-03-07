# Class Herd.Network.Shepherd
> Source: Shepherd.cs
## Methods
### ``bool IsLocalIpAddress(string host)``
* **Summary**
  Determines whether if the local IP address is local
### ``void CallHerd()``
* **Summary**
  Calls the herd using a UDP broadcast. Any agent in the same subnet should reply with its properties
### ``bool ConnectToHerdAgent(IPEndPoint endPoint)``
* **Summary**
  Connects to a herd agent via TCP
* **Parameters**
* * _endPoint_: The end point of the agent
* **Return Value**
  Connects to a herd agent via TCP
### ``void Disconnect()``
* **Summary**
  Disconnects from the herd agent
### ``void GetHerdAgentList(ref List<HerdAgentInfo> outHerdAgentList, int timeoutSeconds = 10)``
* **Summary**
  Gets a list of the herd agent discovered last time the herd was called
* **Parameters**
* * _outHerdAgentList_: The out herd agent list
* * _timeoutSeconds_: The timeout seconds
### ``void SendJobQuery(Job job, CancellationToken cancelToken)``
* **Summary**
  Sends a job query to the herd agent we connected to
* **Parameters**
* * _job_: The job
* * _cancelToken_: The cancel token
