# Class Herd.Network.HerdAgent
> Source: HerdAgent.cs
## Methods
### `public HerdAgent(CancellationTokenSource cancelTokenSource)`
* *Summary*
  HerdAgent class constructor
* *Parameters*
  * _cancelTokenSource_: 
### `void SendJobResult(CancellationToken cancelToken)`
* *Summary*
  Sends the outputs of the job to the client (Badger)
* *Parameters*
  * _cancelToken_: The cancel token.
### `Task WaitForExitAsync(Process process, CancellationToken cancellationToken)`
* *Summary*
  Waits for the process to exit asynchronously
* *Parameters*
  * _process_: The process
  * _cancellationToken_: The cancellation token
* *Return Value*
  Waits for the process to exit asynchronously
