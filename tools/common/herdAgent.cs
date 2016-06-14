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
        private CancellationTokenSource cts;
        private List<Process> m_spawnedProcesses = new List<Process>();
        private object m_quitExecutionLock = new object();
        public const int m_remotelyCancelledErrorCode = -1;
        public const int m_jobInternalErrorCode = -2;
        public const int m_noErrorCode = 0;
        public enum AgentState { BUSY, AVAILABLE, CANCELING };
        public AgentState m_state;
        private UdpClient m_discoveryClient;
        public UdpClient getUdpClient() { return m_discoveryClient; }
        private TcpListener m_listener;
        public TcpListener getTcpServer() {return m_listener;}
        private string m_dirPath="";

        public const string m_herdDescriptionXMLTag = "HerdAgent";
        public const string m_versionXMLTag="HerdAgentVersion";
        public const string m_numProcessorsXMLTag= "NumberOfProcessors";
        public const string m_stateXMLTag= "State";


        public HerdAgent()
        {
            m_state = AgentState.AVAILABLE;
            
        }
        public string getDirPath() { return m_dirPath; }
        private void killSpawnedProcesses()
        {
            lock (m_quitExecutionLock)
            {
                foreach (Process p in m_spawnedProcesses)
                {
                    if (!p.HasExited)
                    {
                        try
                        {
                            p.Kill();
                            p.Dispose();
                        }
                        catch
                        {
                            m_logMessageHandler("Exception received while killing process");
                            //do nothing
                        }
                    }
                }
                m_spawnedProcesses.Clear();
            }
        }
        private void addSpawnedProcessToList(Process p)
        {
            lock (m_quitExecutionLock)
            {
                m_spawnedProcesses.Add(p);
            }
        }
        private void removeSpawnedProcessFromList(Process p)
        {
            lock (m_quitExecutionLock)
            {
                m_spawnedProcesses.Remove(p);
            }
        }
        public void stop()
        {
            if (cts != null)
            {
                try
                { 
                    cts.Cancel(); 
                }
                catch (Exception ex)
                {
                    m_logMessageHandler(ex.ToString());
                }
            }
            killSpawnedProcesses();
        }

        public void cleanCacheDir()
        {
            Directory.Delete(m_dirPath, true);
            Directory.CreateDirectory(m_dirPath);
        }

        public void SendJobResult()
        {
            SendJobHeader();
            //SendExeFiles(false);
            //SendInputFiles(false);
            SendOutputFiles(true);
            SendJobFooter();
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


        public int RunJob()
        {
            int returnCode = 0;
            cts = new CancellationTokenSource();
            ParallelOptions po = new ParallelOptions();
            po.MaxDegreeOfParallelism = Environment.ProcessorCount-1;
            po.CancellationToken = cts.Token;
            try
            {
                Parallel.ForEach(m_job.tasks, po, (task) =>
                {

                NamedPipeServerStream server= null;
                Process myProcess = new Process();
                //string[] arguments = args.Split(' ');
                if (task.pipe!="")
                        server= new NamedPipeServerStream(task.pipe);//arguments[1]);

                try
                {
                    myProcess.StartInfo.FileName = getCachedFilename(task.exe);
                    myProcess.StartInfo.Arguments = task.arguments;
                    myProcess.StartInfo.WorkingDirectory = Path.GetDirectoryName(myProcess.StartInfo.FileName);
                    m_logMessageHandler("Running command: " + myProcess.StartInfo.FileName + " " + myProcess.StartInfo.Arguments);

                    //not to read 23.232 as 23232
                    Thread.CurrentThread.CurrentCulture = System.Globalization.CultureInfo.InvariantCulture;

                    myProcess.Start();
                    addSpawnedProcessToList(myProcess);//m_spawnedProcesses.Add(myProcess);

                            
                    XMLStream xmlStream = new XMLStream();

                    string xmlItem;
                    if (server != null)
                    {
                        server.WaitForConnection();

                        while (server.IsConnected)
                        {
                            //check if we have been asked to cancel
                            po.CancellationToken.ThrowIfCancellationRequested();

                            xmlStream.readFromNamedPipeStream(server);
                            xmlItem = xmlStream.processNextXMLItem();
                            if (xmlItem != "")
                            {
                                //checkConnection(m_tcpClient);
                                xmlStream.writeMessage(m_tcpClient.GetStream(), "<" + task.pipe + ">" + xmlItem + "</" + task.pipe + ">", false);
                            }
                            po.CancellationToken.ThrowIfCancellationRequested();
                        }
                    }
                    myProcess.WaitForExit();

                    if (myProcess.ExitCode < 0)
                    {
                        xmlStream.writeMessage(m_tcpClient.GetStream(), "<" + task.pipe + "><End>Error</End></" + task.pipe + ">", false);
                        returnCode = m_jobInternalErrorCode;
                    }
                    else
                        xmlStream.writeMessage(m_tcpClient.GetStream(), "<" + task.pipe + "><End>Ok</End></" + task.pipe + ">", false);
                    m_logMessageHandler("Exit code: " + myProcess.ExitCode);
                }
                catch (OperationCanceledException)
                {
                    m_logMessageHandler("Thread finished gracefully");
                    returnCode = m_remotelyCancelledErrorCode;
                }
                finally
                {
                    removeSpawnedProcessFromList(myProcess);
                    if (server!=null) server.Close();
                }                    
                });
            }
            catch (OperationCanceledException)
            {
                m_logMessageHandler("Job cancelled gracefully");
            }
            catch (Exception ex)
            {
                m_logMessageHandler(ex.ToString());
                returnCode = m_remotelyCancelledErrorCode;
            }
            finally
            {
                cts.Dispose();
                cts = null;
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
        public async Task asyncReadFromClient(NetworkStream netStream, CancellationToken ct)
        {
            XMLStream inputXMLStream = new XMLStream();
            int bytes = 0;

            try
            {
                while (true)
                {
                    bytes = await netStream.ReadAsync(inputXMLStream.getBuffer(), inputXMLStream.getBufferOffset()
                        , inputXMLStream.getBufferSize() - inputXMLStream.getBufferOffset(), ct);

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
                            stop();
                        }
                    }
                };
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
                logMessage(ex.ToString());
            }
        }
        private void listenForQuitCommand(CancellationToken token)
        {
            //Listen for a "quit" message from the client
            Task.Factory.StartNew(() => asyncReadFromClient(m_netStream, token));
        }
        public void CommunicationCallback(IAsyncResult ar)
        {
            //HerdAgent herdAgent = ((HerdAgentTcpState)(ar.AsyncState)).herdAgent;
            if (getState() != AgentState.BUSY)
            {
                acceptJobQuery(ar);
 
                CancellationTokenSource cancelToken = new CancellationTokenSource();
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
                            logMessage("Receiving job data from" 
                                + getTcpClient().Client.RemoteEndPoint.ToString());
                            if (ReceiveJobQuery())
                            {
                                listenForQuitCommand(cancelToken.Token);

                                //run the job
                                logMessage("Running job");
                                returnCode = RunJob();

                                if (returnCode == m_noErrorCode)
                                {
                                    logMessage("Job finished");
                                    writeMessage(CJobDispatcher.m_endMessage, true);

                                    logMessage("Sending job results");
                                    SendJobResult();

                                    logMessage("Job results sent");
                                }
                                else if (returnCode == m_jobInternalErrorCode)
                                {
                                    logMessage("The job returned an error code");
                                    writeMessage(CJobDispatcher.m_errorMessage, true);
                                }
                                else if (returnCode == m_remotelyCancelledErrorCode)
                                {
                                    logMessage("The job was remotely cancelled");
                                }
                            }
                        }
                    }
                }
                catch (Exception ex)
                {
                    logMessage(ex.ToString() + ex.InnerException + ex.StackTrace);
                }
                finally
                {
                    cancelToken.Cancel();
                    cancelToken.Dispose();
                    getTcpClient().Close();
                    setState(AgentState.AVAILABLE);

                    //try to recover
                    //start listening again
                    HerdAgentTcpState tcpState = new HerdAgentTcpState();
                    tcpState.ip = new IPEndPoint(0, 0);
                    getTcpServer().Start();
                    getTcpServer().BeginAcceptTcpClient(CommunicationCallback, tcpState);
                }
            }
        }

        public void DiscoveryCallback(IAsyncResult ar)
        {
            IPEndPoint ip = ((HerdAgentUdpState)ar.AsyncState).ip;
            //HerdAgent herdAgent = ((HerdAgentUdpState)ar.AsyncState).herdAgent;

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
