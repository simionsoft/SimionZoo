# Class Herd.Network.JobTransmitter
> Source: NetTransfer.cs
## Methods
### ``string getFileTypeXMLTag(FileType type)``
* **Summary**
  Gets the file type's XML tag
* **Parameters**
* * _type_: The file type
* **Return Value**
  Gets the file type's XML tag
### ``void SetLogMessageHandler(Action<string> logMessageHandler)``
* **Summary**
  Sets the log message handler
* **Parameters**
* * _logMessageHandler_: The log message handler that will be used from now on
### ``bool WriteAsync(byte[] buffer, int offset, int length, CancellationToken cancelToken)``
* **Summary**
  Writes asynchronous on the network stream
* **Parameters**
* * _buffer_: The buffer
* * _offset_: The start offset within the buffer
* * _length_: The length
* * _cancelToken_: The cancel token
* **Return Value**
  Writes asynchronous on the network stream
### ``void waitAsyncWriteOpsToFinish()``
* **Summary**
  Waits for the pending asynchronous write operations to finish.
### ``void SendInputFiles(bool sendContent, CancellationToken cancelToken)``
* **Summary**
  Sends the input files to the client
* **Parameters**
* * _sendContent_: true if the contents of the files must also be sent
* * _cancelToken_: The cancel token
### ``void SendOutputFiles(bool sendContent, CancellationToken cancelToken)``
* **Summary**
  Sends the output files to the client
* **Parameters**
* * _sendContent_: true if the content of the file must be sent too
* * _cancelToken_: The cancel token
### ``void SendTask(HerdTask task, CancellationToken cancelToken)``
* **Summary**
  Sends the task to the client
* **Parameters**
* * _task_: The task
* * _cancelToken_: The cancel token
### ``void SendJobHeader(CancellationToken cancelToken)``
* **Summary**
  Sends the job header
* **Parameters**
* * _cancelToken_: The cancel token
### ``void SendJobFooter(CancellationToken cancelToken)``
* **Summary**
  Sends the job footer.
* **Parameters**
* * _cancelToken_: The cancel token.
### ``void SendFile(string fileName, FileType type, bool sendContent
            , bool fromCachedDir, CancellationToken cancelToken, string rename = null)``
* **Summary**
  Sends the file
* **Parameters**
* * _fileName_: Name of the file
* * _type_: The type: input or output
* * _sendContent_: If true, the content is also sent
* * _fromCachedDir_: If true, it will be read from the temp dir
* * _cancelToken_: The cancel token
* * _rename_: The name given to the file remotely
