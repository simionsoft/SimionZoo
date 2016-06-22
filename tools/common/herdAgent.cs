using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.IO.Pipes;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading;
using System.Threading.Tasks;
using System.Xml;
using System.Xml.Linq;

namespace Herd
{
    public class HerdAgentUdpState
    {
    //    public HerdAgent herdAgent { get; set; }
        public UdpClient client { get; set; }
        public IPEndPoint ip { get; set; }
    }
    public class HerdAgentTcpState
    {
    //    public HerdAgent herdAgent { get; set; }
        public IPEndPoint ip { get; set; }
    }

    public class HerdAgent : CJobDispatcher
    {
        //private CancellationTokenSource cts;
        //private List<Process> m_spawnedProcesses = new List<Process>();
        private object m_quitExecutionLock = new object();
        public const int m_remotelyCancelledErrorCode = -1;
        public const int m_jobInternalErrorCode = -2;
        public const int m_noErrorCode = 0;
        public enum AgentState { BUSY, AVAILABLE, CANCELING };
        public AgentState m_state;
        private UdpClient m_discoveryClient;
        public UdpClient getUdpClient() { return m_discoveryClient; }
        private TcpListener m_listener;

        private string m_dirPath="";

        public const string m_herdDescriptionXMLTag = "HerdAgent";
        public const string m_versionXMLTag="HerdAgentVersion";
        public const string m_numProcessorsXMLTag= "NumberOfProcessors";
        public const string m_stateXMLTag= "State";
        private CancellationTokenSource m_cancelTokenSource;


        public HerdAgent(CancellationTokenSource cancelTokenSource)
        {
            m_cancelTokenSource= cancelTokenSource;
            m_state = AgentState.AVAILABLE;
        }
        public string getDirPath() { return m_dirPath; }
        //private void killSpawnedProcesses()
        //{
        //    lock (m_quitExecutionLock)
        //    {
        //        foreach (Process p in m_spawnedProcesses)
        //        {
        //            if (!p.HasExited)
        //            {
        //                try
        //                {
        //                    p.Kill();
        //                    p.Dispose();
        //                }
        //                catch
        //                {
        //                    logMessage("Exception: can't kill process");
        //                    //do nothing
        //                }
        //            }
        //        }
        //        m_spawnedProcesses.Clear();
        //    }
        //}
        //private void addSpawnedProcessToList(Process p)
        //{
        //    lock (m_quitExecutionLock)
        //    {
        //        m_spawnedProcesses.Add(p);
        //    }
        //}
        //private void removeSpawnedProcessFromList(Process p)
        //{
        //    lock (m_quitExecutionLock)
        //    {
        //        m_spawnedProcesses.Remove(p);
        //    }
        //}
        //public void stop()
        //{
        //    if (m_cancelTokenSource != null)
        //    {
        //        try
        //        { 
        //            m_cancelTokenSource.Cancel(); 
        //        }
        //        catch (Exception ex)
        //        {
        //            logMessage("Exception stopping processes");
        //            logMessage(ex.ToString());
        //        }
        //    }
        //    killSpawnedProcesses();
        //}

        public void cleanCacheDir()
        {
            Directory.Delete(m_dirPath, true);
            Directory.CreateDirectory(m_dirPath);
        }

        public void SendJobResult(CancellationToken cancelToken)
        {
            SendJobHeader(cancelToken);
            SendOutputFiles(true,cancelToken);
            SendJobFooter(cancelToken);
        }
        public bool ReceiveJobQuery()
        {
            bool bFooterPeeked = false;
            string xmlTag = "";
            m_job.tasks.Clear();
            m_job.inputFiles.Clear();
            m_job.outputFiles.Clear();

            ReceiveJobHeader();

            do
            {
                ReadFromStream();
                xmlTag = m_xmlStream.peekNextXMLTag();
                switch (xmlTag)
                {
                    case "Task": ReceiveTask(); break;
                    case "Input": ReceiveFile(FileType.INPUT,true, true); break;
                    case "Output": ReceiveFile(FileType.OUTPUT, false, true); break;
                    case "/Job": bFooterPeeked = true; break;
                }
            } while (!bFooterPeeked);

            ReceiveJobFooter();
            //we will assume everything went ok for now
            return true;
        }
        //public static Task runProcessAsync(Process process, CancellationToken cancelToken)
        //{
        //    var tcs = new TaskCompletionSource<object>();

        //    process.Exited += (sender, args) =>
        //    {
        //        if (process.ExitCode != 0)
        //        {
        //            var errorMessage = process.StandardError.ReadToEnd();
        //            tcs.SetException(new InvalidOperationException("The process did not exit correctly. " +
        //                "The corresponding error message was: " + errorMessage));
        //        }
        //        else
        //        {
        //            tcs.SetResult(null);
        //        }
        //        process.Dispose();
        //    };
        //    process.Start();
        //    return tcs.Task;
        //}
        public static Task waitForExitAsync(Process process, CancellationToken cancellationToken)
        {
            var tcs = new TaskCompletionSource<object>();
            process.EnableRaisingEvents = true;
            process.Exited += (sender, args) => { tcs.TrySetResult(null); };
            //if(cancellationToken != default(CancellationToken))
            cancellationToken.Register(tcs.SetCanceled);
            return tcs.Task;
        }
        public async Task<int> runTaskAsync(CTask task, CancellationToken cancelToken)
        {
            int returnCode= m_noErrorCode;
            NamedPipeServerStream pipeServer = null;
            Process myProcess = new Process();
            if (task.pipe != "")
                pipeServer = new NamedPipeServerStream(task.pipe);

            try 
            {
                myProcess.StartInfo.FileName = getCachedFilename(task.exe);
                myProcess.StartInfo.Arguments = task.arguments;
                myProcess.StartInfo.WorkingDirectory = Path.GetDirectoryName(myProcess.StartInfo.FileName);
                logMessage("Running command: " + myProcess.StartInfo.FileName + " " + myProcess.StartInfo.Arguments);

                //not to read 23.232 as 23232
                Thread.CurrentThread.CurrentCulture = System.Globalization.CultureInfo.InvariantCulture;

                myProcess.Start();
                //addSpawnedProcessToList(myProcess);

                XMLStream xmlStream = new XMLStream();

                string xmlItem;

                if (pipeServer != null)
                {
                    pipeServer.WaitForConnection();

                    while (pipeServer.IsConnected)
                    {
                        //check if the herd agent sent us a quit message
                        //in case it did, the function will cancel the cancellation token
                        checkCancellationRequests();
                        
                        //check if we have been asked to cancel
                        cancelToken.ThrowIfCancellationRequested();

                        int numBytes= await xmlStream.readFromNamedPipeStreamAsync(pipeServer,cancelToken);
                        xmlItem = xmlStream.processNextXMLItem();
                        while (xmlItem != "")
                        {
                            await xmlStream.writeMessageAsync(m_tcpClient.GetStream(), "<" + task.pipe + ">" + xmlItem + "</" + task.pipe + ">"
                                , cancelToken, false);
                            
                            xmlItem = xmlStream.processNextXMLItem();
                        }
                    }
                }

                await waitForExitAsync(myProcess,cancelToken);

                int exitCode = myProcess.ExitCode;
                //myProcess.WaitForExit();

                if (exitCode < 0)
                {
                    await xmlStream.writeMessageAsync(m_netStream, "<" + task.pipe + "><End>Error</End></" + task.pipe + ">"
                        , cancelToken,false);
                    returnCode = m_jobInternalErrorCode;
                }
                else
                    await xmlStream.writeMessageAsync(m_netStream, "<" + task.pipe + "><End>Ok</End></" + task.pipe + ">"
                        , cancelToken,false);
                logMessage("Exit code: " + myProcess.ExitCode);
            }
            catch (OperationCanceledException)
            {
                logMessage("Thread finished gracefully");
                myProcess.Kill();
                returnCode = m_remotelyCancelledErrorCode;
            }
            catch(Exception ex)
            {
                logMessage("unhandled exception in runTaskRemotely()");
            }
            finally
            {
                logMessage("Task " + task.name + " finished");
                //removeSpawnedProcessFromList(myProcess);
                if (pipeServer!=null) pipeServer.Close();
            }
            return returnCode;
        }
        public async Task<int> runJobAsync(CancellationToken cancelToken)
        {
            int returnCode= m_noErrorCode;
            try
            {
                List<Task<int>> taskList = new List<Task<int>>();
                foreach (CTask task in m_job.tasks)
                    taskList.Add(runTaskAsync(task, cancelToken));
                int[] exitCodes = await Task.WhenAll(taskList);

                if (exitCodes.Any((code) => code == m_remotelyCancelledErrorCode))
                    returnCode = m_remotelyCancelledErrorCode;
                else if (exitCodes.All((code) => code != m_noErrorCode))
                    returnCode = m_jobInternalErrorCode;

                logMessage("All processes finished");
            }
            catch (OperationCanceledException)
            {
                returnCode = m_remotelyCancelledErrorCode;
                logMessage("Job cancelled gracefully");
            }
            catch (Exception ex)
            {
                logMessage(ex.ToString());
                returnCode = m_jobInternalErrorCode;
            }
            finally
            {
                m_cancelTokenSource.Dispose();
                m_cancelTokenSource = new CancellationTokenSource();
            }
            return returnCode;
        }


        public AgentState getState() { return m_state; }
        public string getStateString()
        {
            if (m_state == AgentState.AVAILABLE) return "available";
            if (m_state == AgentState.BUSY) return "busy";
            if (m_state == AgentState.CANCELING) return "canceling";
            return "error";
        }
        public void setState(AgentState s) { m_state = s; }
        public string getAgentDescription()
        {
            string description = "<" + m_herdDescriptionXMLTag + ">";
            description += "<" + m_versionXMLTag +">" + System.Reflection.Assembly.GetExecutingAssembly().GetName().Version
                + "</" + m_versionXMLTag + ">";
            description += "<" + m_numProcessorsXMLTag + ">" + Environment.ProcessorCount + "</" + m_numProcessorsXMLTag + ">";
            description += "<" + m_stateXMLTag + ">" + getStateString() + "</" + m_stateXMLTag + ">";
            description += "</" + m_herdDescriptionXMLTag + ">";
            return description;
        }

        public void acceptJobQuery(IAsyncResult ar)
        {
            m_tcpClient= m_listener.EndAcceptTcpClient(ar);
            m_xmlStream.resizeBuffer(m_tcpClient.ReceiveBufferSize);
            m_netStream = m_tcpClient.GetStream();
        }
        public void checkCancellationRequests()
        {
            int bytes = 0;

            try
            {
                if (!m_netStream.DataAvailable) return;

                XMLStream inputXMLStream = new XMLStream();
                bytes = m_netStream.Read(inputXMLStream.getBuffer(), inputXMLStream.getBufferOffset()
                    , inputXMLStream.getBufferSize() - inputXMLStream.getBufferOffset());

                inputXMLStream.addBytesRead(bytes);
                //we let the xmlstream object know that some bytes have been read in its buffer
                string xmlItem = inputXMLStream.peekNextXMLItem();
                if (xmlItem != "")
                {
                    string xmlItemContent = inputXMLStream.getLastXMLItemContent();
                    if (xmlItemContent == CJobDispatcher.m_quitMessage)
                    {
                        inputXMLStream.addProcessedBytes(bytes);
                        inputXMLStream.discardProcessedData();
                        logMessage("Stopping job execution");
                        m_cancelTokenSource.Cancel();
                    }
                }
            }
            catch (IOException)
            {
                logMessage("IOException in readFromShepherd()");
            }
            catch (OperationCanceledException)
            {
                logMessage("Thread finished gracefully");
            }
            catch (ObjectDisposedException)
            {
                logMessage("Network stream closed: async read finished");
            }
            catch (Exception ex)
            {
                logMessage("Unhandled exception in readFromShepherd");
                logMessage(ex.ToString());
            }
        }

        //public async Task readFromShepherdAsync(CancellationToken ct)
        //{
        //    XMLStream inputXMLStream = new XMLStream();
        //    int bytes = 0;

        //    try
        //    {
        //        bytes = await m_netStream.ReadAsync(inputXMLStream.getBuffer(), inputXMLStream.getBufferOffset()
        //            , inputXMLStream.getBufferSize() - inputXMLStream.getBufferOffset(), ct);

        //        inputXMLStream.addBytesRead(bytes);
        //        //we let the xmlstream object know that some bytes have been read in its buffer
        //        string xmlItem = inputXMLStream.peekNextXMLItem();
        //        if (xmlItem != "")
        //        {
        //            string xmlItemContent = inputXMLStream.getLastXMLItemContent();
        //            if (xmlItemContent == CJobDispatcher.m_quitMessage)
        //            {
        //                inputXMLStream.addProcessedBytes(bytes);
        //                inputXMLStream.discardProcessedData();
        //                logMessage("Stopping job execution");
        //                stop();
        //            }
        //        }
        //    }
        //    catch(IOException)
        //    {
        //        logMessage("could not read from socket in readFromShepherdAsync()");
        //    }
        //    catch (OperationCanceledException)
        //    {
        //        logMessage("Thread finished gracefully");
        //    }
        //    catch (ObjectDisposedException)
        //    {
        //        logMessage("Network stream closed: async read finished");
        //    }
        //    catch (Exception ex)
        //    {
        //        logMessage("Unhandled exception in asyncReadFromClient");
        //        logMessage(ex.ToString());
        //    }
        //}
        //private void listenForQuitCommand(CancellationToken token)
        //{
        //    //Listen for a "quit" message from the client
        //    Task.Run(() => readFromShepherdAsync(token));
        //}
        public async void CommunicationCallback(IAsyncResult ar)
        {
            if (getState() != AgentState.BUSY)
            {
                acceptJobQuery(ar);
 
                try
                {
                    setState(AgentState.BUSY);

                    read();
                    string xmlItem = m_xmlStream.processNextXMLItem();
                    string xmlItemContent;
                    int returnCode;

                    if (xmlItem != "")
                    {
                        xmlItemContent = m_xmlStream.getLastXMLItemContent();
                        if (xmlItemContent == CJobDispatcher.m_cleanCacheMessage)
                        {
                            //not yet implemented in the herd client, just in case...
                            logMessage("Cleaning cache directory");
                            cleanCacheDir();
                        }
                        else if (xmlItemContent == CJobDispatcher.m_acquireMessage)
                        {
                            logMessage("Receiving job data from " 
                                + m_tcpClient.Client.RemoteEndPoint.ToString());
                            if (ReceiveJobQuery())
                            {
                               //listenForQuitCommand(m_cancelTokenSource.Token);

                                //run the job
                                logMessage("Running job");
                                returnCode = await runJobAsync(m_cancelTokenSource.Token);

                                if (returnCode == m_noErrorCode || returnCode == m_jobInternalErrorCode)
                                {
                                    logMessage("Job finished");
                                    await writeMessageAsync(CJobDispatcher.m_endMessage, m_cancelTokenSource.Token,true);

                                    logMessage("Sending job results");
                                    //we will have to enqueue async write operations to wait for them to finish before closing the tcpClient
                                    startEnqueueingAsyncWriteOps();
                                    SendJobResult(m_cancelTokenSource.Token);

                                    logMessage("Job results sent");
                                }
                                else if (returnCode == m_remotelyCancelledErrorCode)
                                {
                                    logMessage("The job was remotely cancelled");
                                    writeMessage(CJobDispatcher.m_errorMessage, false);
                                }
                            }
                        }
                    }
                }
                catch (Exception ex)
                {
                    logMessage("Unhandled exception in the herd agent's communication callback function");
                    logMessage(ex.ToString() + ex.InnerException + ex.StackTrace);
                }
                finally
                {
                    waitAsyncWriteOpsToFinish();
                    m_tcpClient.Close();
                    setState(AgentState.AVAILABLE);

                    //try to recover
                    //start listening again
                    HerdAgentTcpState tcpState = new HerdAgentTcpState();
                    tcpState.ip = new IPEndPoint(0, 0);
                    //m_listener.Start();
                    m_listener.BeginAcceptTcpClient(CommunicationCallback, tcpState);
                }
            }
        }

        public void DiscoveryCallback(IAsyncResult ar)
        {
            IPEndPoint ip = ((HerdAgentUdpState)ar.AsyncState).ip;
            //HerdAgent herdAgent = ((HerdAgentUdpState)ar.AsyncState).herdAgent;
            try
            {
                Byte[] receiveBytes = getUdpClient().EndReceive(ar, ref ip);
                string receiveString = Encoding.ASCII.GetString(receiveBytes);

                if (receiveString == CJobDispatcher.m_discoveryMessage)
                {
                    //if (getState() == AgentState.AVAILABLE)
                    {
                        logMessage("Agent discovered by " + ip.ToString() + ". Current state=" + getStateString());
                        string agentDescription = getAgentDescription();
                        byte[] data = Encoding.ASCII.GetBytes(agentDescription);
                        getUdpClient().Send(data, data.Length, ip);
                    }
                    //else logMessage("Agent contacted by " + ip.ToString() + " but rejected connection because it was busy");
                }
                else logMessage("Message received by " + ip.ToString() + " not understood: " + receiveString);

                getUdpClient().BeginReceive(new AsyncCallback(DiscoveryCallback), ar.AsyncState);
            }
            catch (Exception ex)
            {
                logMessage("Unhandled exception in DiscoveryCallback");
                logMessage(ex.ToString());
            }
        }
        public void startListening()
        {
            //UPD broadcast client
            m_discoveryClient = new UdpClient(CJobDispatcher.m_discoveryPortHerd);
            HerdAgentUdpState state = new HerdAgentUdpState();
            IPEndPoint shepherd = new IPEndPoint(0, 0);
            state.ip = shepherd;
           // state.herdAgent = this;
            m_discoveryClient.BeginReceive(DiscoveryCallback, state);
            

            //TCP communication socket
            m_listener = new TcpListener(IPAddress.Any, CJobDispatcher.m_comPortHerd);
            m_listener.Start();
            HerdAgentTcpState tcpState = new HerdAgentTcpState();
            tcpState.ip = shepherd;
            //tcpState.herdAgent = this;
            
            m_listener.BeginAcceptTcpClient(CommunicationCallback, tcpState);
        }
        public void stopListening()
        {
            m_discoveryClient.Close();
            m_listener.Stop();
        }
    }

}
