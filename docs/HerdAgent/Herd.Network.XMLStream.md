# Class Herd.Network.XMLStream
> Source: XMLStream.cs
## Methods
### `void discardProcessedData()`
* *Summary*
  Discards all already processed data in the buffer
### `void WriteMessage(NetworkStream stream, string message, bool addDefaultMessageType = false)`
* *Summary*
  Writes on the network stream
* *Parameters*
  * _stream_: The stream.
  * _message_: The message.
  * _addDefaultMessageType_: If true, a default header is added to the message
### `Task WriteMessageAsync(NetworkStream stream, string message, CancellationToken cancelToken, bool addDefaultMessageType = false)`
* *Summary*
  Writes on the network stream asynchronously
* *Parameters*
  * _stream_: The stream
  * _message_: The message
  * _cancelToken_: The cancel token
  * _addDefaultMessageType_: If true, a default header is added to the message
* *Return Value*
  Writes on the network stream asynchronously
### `void writeMessage(NamedPipeServerStream stream, string message, bool addDefaultMessageType = false)`
* *Summary*
  Writes the message on the named pipe stream
* *Parameters*
  * _stream_: The stream
  * _message_: The message
  * _addDefaultMessageType_: If true, a default header is added to the message
### `Task writeMessageAsync(NamedPipeServerStream stream, string message, CancellationToken cancelToken, bool addDefaultMessageType = false)`
* *Summary*
  Writes a message on the named pipe stream asynchronously
* *Parameters*
  * _stream_: The stream
  * _message_: The message
  * _cancelToken_: The cancel token
  * _addDefaultMessageType_: If true, a default header is added to the message
* *Return Value*
  Writes a message on the named pipe stream asynchronously
### `string peekNextXMLItem()`
* *Summary*
  Peeks the next XML item without advancing on the buffer
* *Return Value*
  Peeks the next XML item without advancing on the buffer
### `string processNextXMLItem(bool bMarkAsProcessed = true)`
* *Summary*
  returns the next complete xml element (NO ATTRIBUTES!!) in the stream empty string if there was none
* *Parameters*
  * _bMarkAsProcessed_: if true, the element is marked as processed
* *Return Value*
  returns the next complete xml element (NO ATTRIBUTES!!) in the stream empty string if there was none
### `string peekNextXMLTag()`
* *Summary*
  If message "harry potter" is received this method should return "pipe1", not marking those bytes as processed
* *Return Value*
  If message "harry potter" is received this method should return "pipe1", not marking those bytes as processed
### `string processNextXMLTag()`
* *Summary*
  Returns the next complete xml element (NO ATTRIBUTES!!) in the stream, or an empty string if there was none
* *Return Value*
  Returns the next complete xml element (NO ATTRIBUTES!!) in the stream, or an empty string if there was none
### `string getLastXMLItemContent()`
* *Summary*
  Instead of parsing pending info in the buffer, it parses m_lastXMLItem , which is set after a call to processNextXMLTag()
* *Return Value*
  Instead of parsing pending info in the buffer, it parses m_lastXMLItem , which is set after a call to processNextXMLTag()
