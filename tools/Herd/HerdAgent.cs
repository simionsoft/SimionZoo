using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.IO.Pipes;
using System.Linq;
using System.Management;
using System.Net;
using System.Net.Sockets;
using System.Runtime.InteropServices;
using System.Text;
using System.Xml.Linq;
using System.Threading;
using System.Threading.Tasks;
using System.Security.Authentication;

namespace Herd
{
    public class HerdAgentInfo
    {
        public const string NoneProperty = "N/A";

        private IPEndPoint m_ipAddress;
        public IPEndPoint ipAddress { get { return m_ipAddress; } set { m_ipAddress = value; } }

        public string ipAddressString
        {
            get
            {
                return m_ipAddress.Address.ToString();
            }
        }

        public DateTime lastACK { get { return m_lastACK; } set { m_lastACK = value; } }
        private DateTime m_lastACK;
        private Dictionary<string, string> m_properties;

        public HerdAgentInfo()
        {
            m_properties = new Dictionary<string, string>();
        }

        public void addProperty(string name, string value)
        {
            m_properties.Add(name, value);
        }

        public string getProperty(string name)
        {
            if (m_properties.ContainsKey(name))
                return m_properties[name];

            return NoneProperty;
        }


        public void Parse(XElement xmlDescription)
        {
            if (xmlDescription.Name.ToString() == "HerdAgent")
            {
                m_properties.Clear();

                foreach (XElement child in xmlDescription.Elements())
                    addProperty(child.Name.ToString(), child.Value);
            }
        }


        public override string ToString()
        {
            string res = "";
            foreach (var property in m_properties)
                res += property.Key + "=\"" + property.Value + "\";";

            return res;
        }


        public string State { get { return getProperty(HerdAgent.m_stateXMLTag); } set { } }

        public string Version { get { return getProperty(HerdAgent.m_versionXMLTag); } set { } }

        public string ProcessorId { get { return getProperty(HerdAgent.m_processorIdTag); } }

        public int NumProcessors
        {
            get
            {
                string prop = getProperty(HerdAgent.m_numProcessorsXMLTag);
                return (!prop.Equals(NoneProperty)) ? int.Parse(prop) : 0;
            }
        }

        public string ProcessorArchitecture { get { return getProperty(HerdAgent.ProcessorArchitectureTag); } }

        public double ProcessorLoad
        {
            get
            {
                string prop = getProperty(HerdAgent.ProcessorLoadTag);
                return (!prop.Equals(NoneProperty)) ? double.Parse(prop) : 0.0;
            }
        }

        public double Memory
        {
            get
            {
                string prop = getProperty(HerdAgent.TotalMemoryTag);
                return (!prop.Equals(NoneProperty)) ? double.Parse(prop) : 0.0;
            }
        }

        public string CudaInfo { get { return getProperty(HerdAgent.CudaInfoTag); } }

        public bool IsAvailable
        {
            get { if (getProperty(HerdAgent.m_stateXMLTag) == "available") return true; return false; }
            set { }
        }
    }


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

        public const string FirewallExceptionNameTCP = "HerdAgentFirewallExceptionTCP";
        public const string FirewallExceptionNameUDP = "HerdAgentFirewallExceptionUDP";
        private object m_quitExecutionLock = new object();
        public const int m_remotelyCancelledErrorCode = -1;
        public const int m_jobInternalErrorCode = -2;
        public const int m_noErrorCode = 0;

        public enum AgentState { Busy, Available, Cancelling };

        public AgentState m_state;

        private UdpClient m_discoveryClient;
        public UdpClient getUdpClient() { return m_discoveryClient; }
        private TcpListener m_listener;

        private Credentials m_credentials;

        private string m_dirPath = "";

        public const string m_herdDescriptionXMLTag = "HerdAgent";
        public const string m_versionXMLTag = "HerdAgentVersion";
        public const string m_processorIdTag = "ProccesorId";
        public const string m_numProcessorsXMLTag = "NumberOfProcessors";
        public const string m_stateXMLTag = "State";
        public const string TotalMemoryTag = "Memory";
        public const string ProcessorArchitectureTag = "ProcessorArchitecture";
        public const string ProcessorLoadTag = "ProcessorLoad";
        public const string CudaInfoTag = "CudaInfo";

        private CancellationTokenSource m_cancelTokenSource;

        private PerformanceCounter m_cpuCounter;

        /// <summary>
        ///     HerdAgent class constructor
        /// </summary>
        /// <param name="cancelTokenSource"></param>
        public HerdAgent(CancellationTokenSource cancelTokenSource)
        {
            m_cancelTokenSource = cancelTokenSource;
            m_state = AgentState.Available;
            m_cpuCounter = new PerformanceCounter("Processor", "% Processor Time", "_Total");
            m_credentials = Credentials.Load(Path.GetDirectoryName(System.Reflection.Assembly.GetEntryAssembly().Location));
        }

        protected override string getTmpDir()
        {
            if (m_credentials == null)
                return base.getTmpDir();
            else
                return m_credentials.GetTempDir();
        }

        public string getDirPath() { return m_dirPath; }

        public void cleanCacheDir()
        {
            Directory.Delete(m_dirPath, true);
            Directory.CreateDirectory(m_dirPath);
        }

        public void SendJobResult(CancellationToken cancelToken)
        {
            SendJobHeader(cancelToken);
            SendOutputFiles(true, cancelToken);
            SendJobFooter(cancelToken);
        }
        public async Task<bool> ReceiveJobQuery(CancellationToken cancelToken)
        {
            bool bFooterPeeked = false;
            string xmlTag = "";
            m_job.tasks.Clear();
            m_job.inputFiles.Clear();
            m_job.outputFiles.Clear();

            int ret = await ReceiveJobHeader(cancelToken);
            bool bret;
            do
            {
                //ReadFromStream();
                xmlTag = m_xmlStream.peekNextXMLTag();
                while (xmlTag == "")
                {
                    ret = await ReadFromStreamAsync(cancelToken);
                    xmlTag = m_xmlStream.peekNextXMLTag();
                }
                switch (xmlTag)
                {
                    case "Task": bret = await ReceiveTask(cancelToken); break;
                    case "Input": bret = await ReceiveFile(FileType.INPUT, true, true, cancelToken); break;
                    case "Output": bret = await ReceiveFile(FileType.OUTPUT, false, true, cancelToken); break;
                    case "/Job": bFooterPeeked = true; break;
                    default: logMessage("WARNING: Unexpected xml tag received: " + xmlTag); break;
                }
            } while (!bFooterPeeked);

            logMessage("Waiting for job footer");
            bret = await ReceiveJobFooter(cancelToken);
            logMessage("Job footer received");

            return true;
        }


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
            int returnCode = m_noErrorCode;
            NamedPipeServerStream pipeServer = null;
            Process myProcess = new Process();
            if (task.pipe != "")
                pipeServer = new NamedPipeServerStream(task.pipe);
            XMLStream xmlStream = new XMLStream();

            try
            {
                //not to read 23.232 as 23232
                Thread.CurrentThread.CurrentCulture = System.Globalization.CultureInfo.InvariantCulture;

                if (m_credentials != null)
                {
                    if (!m_credentials.AuthenticationCode.Equals(task.authenticationToken))
                    {
                        logMessage("Authentication tokens did NOT match. Won't execute command: " + getCachedFilename(task.exe) + " " + task.arguments);

                        await xmlStream.writeMessageAsync(m_tcpClient.GetStream(),
                       "<" + task.pipe + "><End>Authentication Error</End></" + task.pipe + ">", cancelToken);

                        returnCode = -1;
                        throw new AuthenticationException("AuthenticationToken of the task does not match local token.");
                    }

                    myProcess = WinApi.StartUnelevatedProcess(getCachedFilename(task.exe), task.arguments, Path.GetDirectoryName((getCachedFilename(task.exe))));
                    // myProcess = WinApi.StartUnelevatedProcess(@"C:\Users\Roland\Source\Repos\RLSimion\Debug\HerdAgentServiceSettings.exe", task.arguments, Path.GetDirectoryName(@"C:\Users\Roland\Source\Repos\RLSimion\Debug\HerdAgentServiceSettings.exe"));
                }

                myProcess.StartInfo.FileName = getCachedFilename(task.exe);
                myProcess.StartInfo.Arguments = task.arguments;
                myProcess.StartInfo.WorkingDirectory = Path.GetDirectoryName(myProcess.StartInfo.FileName);

                if (m_credentials == null)
                {
                    myProcess.Start();
                }

                logMessage("Running command: " + myProcess.StartInfo.FileName + " " + myProcess.StartInfo.Arguments);

                //addSpawnedProcessToList(myProcess);

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

                        int numBytes = await xmlStream.readFromNamedPipeStreamAsync(pipeServer, cancelToken);
                        xmlItem = xmlStream.processNextXMLItem();
                        while (xmlItem != "")
                        {
                            await xmlStream.writeMessageAsync(m_tcpClient.GetStream(),
                                "<" + task.pipe + ">" + xmlItem + "</" + task.pipe + ">", cancelToken);

                            xmlItem = xmlStream.processNextXMLItem();
                        }
                    }
                }

                logMessage("Named pipe has been closed for task " + task.name);
                await waitForExitAsync(myProcess, cancelToken);

                int exitCode = myProcess.ExitCode;
                logMessage("Process exited in task " + task.name + ". Return code=" + exitCode);
                //myProcess.WaitForExit();

                if (exitCode < 0)
                {
                    await xmlStream.writeMessageAsync(m_tcpClient.GetStream(),
                        "<" + task.pipe + "><End>Error</End></" + task.pipe + ">", cancelToken);
                    returnCode = m_jobInternalErrorCode;
                }
                else
                    await xmlStream.writeMessageAsync(m_tcpClient.GetStream(),
                        "<" + task.pipe + "><End>Ok</End></" + task.pipe + ">", cancelToken);
                logMessage("Exit code: " + myProcess.ExitCode);
            }
            catch (OperationCanceledException)
            {
                logMessage("Thread finished gracefully");
                if (myProcess != null) myProcess.Kill();
                returnCode = m_remotelyCancelledErrorCode;
            }
            catch (AuthenticationException ex){
                logMessage(ex.ToString());
            }
            catch (Exception ex)
            {
                logMessage("unhandled exception in runTaskAsync()");
                logMessage(ex.ToString());
                if (myProcess != null) myProcess.Kill();
                returnCode = m_jobInternalErrorCode;
            }
            finally
            {
                logMessage("Task " + task.name + " finished");
                //removeSpawnedProcessFromList(myProcess);
                if (pipeServer != null) pipeServer.Close();
            }

            return returnCode;
        }


        public async Task<int> runJobAsync(CancellationToken cancelToken)
        {
            int returnCode = m_noErrorCode;
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
            if (m_state == AgentState.Available) return "available";
            if (m_state == AgentState.Busy) return "busy";
            if (m_state == AgentState.Cancelling) return "cancelling";
            return "error";
        }


        public void setState(AgentState s) { m_state = s; }


        public float GetCurrentCpuUsage()
        {
            return m_cpuCounter.NextValue();
        }

        public string GetProcessorId()
        {
            var mbs = new ManagementObjectSearcher("Select ProcessorId From Win32_processor");
            ManagementObjectCollection mbsList = mbs.Get();
            string id = "";
            foreach (ManagementObject mo in mbsList)
            {
                id = mo["ProcessorId"].ToString();
                break;
            }

            return id;
        }

        /// <summary>
        ///     Get information about CUDA installation.
        /// </summary>
        /// <returns>The CUDA version installed or -1 if none was found</returns>
        public string GetCudaInfo()
        {
            string[] dllNames = { @"nvcuda.dll", @"nvcuda64.dll" };
            string dir = Environment.SystemDirectory;

            try
            {
                FileVersionInfo myFileVersionInfo = null;
                bool bCudaCapable = false;
                foreach (string dll in dllNames)
                {
                    var dllPath = dir + "\\" + dll;
                    if (System.IO.File.Exists(dllPath))
                    {
                        myFileVersionInfo = FileVersionInfo.GetVersionInfo(dllPath);
                        bCudaCapable = true;
                    }
                    else
                    {
                        bCudaCapable = false;
                        break;
                    }
                }

                if (bCudaCapable)
                    return myFileVersionInfo.ProductVersion;

                return HerdAgentInfo.NoneProperty;
            }
            catch (Exception ex)
            {
                Console.Write(ex.StackTrace);
                return HerdAgentInfo.NoneProperty;
            }
        }


        [StructLayout(LayoutKind.Sequential)]
        struct MEMORYSTATUSEX
        {
            internal uint dwLength;
            uint dwMemoryLoad;
            internal ulong ullTotalPhys;
            internal ulong ullAvailPhys;
            internal ulong ullTotalPageFile;
            ulong ullAvailPageFile;
            ulong ullTotalVirtual;
            ulong ullAvailVirtual;
            ulong ullAvailExtendedVirtual;
        }

        [return: MarshalAs(UnmanagedType.Bool)]
        [DllImport("kernel32.dll", CharSet = CharSet.Auto, SetLastError = true)]
        static extern bool GlobalMemoryStatusEx(ref MEMORYSTATUSEX lpBuffer);

        public static double GetGlobalMemoryStatusEx()
        {
            MEMORYSTATUSEX statEx = new MEMORYSTATUSEX();
            statEx.dwLength = (uint)Marshal.SizeOf(typeof(MEMORYSTATUSEX));
            GlobalMemoryStatusEx(ref statEx);

            return statEx.ullTotalPhys;
        }

        public string GetAgentDescription()
        {
            string description = "<" + m_herdDescriptionXMLTag + ">"
                // HerdAgent version
                + "<" + m_versionXMLTag + ">"
                + System.Reflection.Assembly.GetExecutingAssembly().GetName().Version
                + "</" + m_versionXMLTag + ">"
                // CPU amount of cores
                + "<" + m_processorIdTag + ">" + GetProcessorId() + "</" + m_processorIdTag + ">"
                // CPU amount of cores
                + "<" + m_numProcessorsXMLTag + ">" + Environment.ProcessorCount + "</" + m_numProcessorsXMLTag + ">"
                // CPU architecture
                + "<" + ProcessorArchitectureTag + ">"
                + Environment.GetEnvironmentVariable("PROCESSOR_ARCHITECTURE", EnvironmentVariableTarget.Machine)
                + "</" + ProcessorArchitectureTag + ">"
                // Current processor load
                + "<" + ProcessorLoadTag + ">" + GetCurrentCpuUsage() + "</" + ProcessorLoadTag + ">"
                // Total installed memory
                + "<" + TotalMemoryTag + ">" + GetGlobalMemoryStatusEx() + "</" + TotalMemoryTag + ">"
                // CUDA support information
                + "<" + CudaInfoTag + ">" + GetCudaInfo() + "</" + CudaInfoTag + ">"
                // HerdAgent state
                + "<" + m_stateXMLTag + ">" + getStateString() + "</" + m_stateXMLTag + ">"
                + "</" + m_herdDescriptionXMLTag + ">";
            return description;
        }


        public void acceptJobQuery(IAsyncResult ar)
        {
            m_tcpClient = m_listener.EndAcceptTcpClient(ar);
            m_xmlStream.resizeBuffer(m_tcpClient.ReceiveBufferSize);
            m_netStream = m_tcpClient.GetStream();
        }


        public void checkCancellationRequests()
        {
            try
            {
                if (!m_netStream.DataAvailable) return;

                XMLStream inputXMLStream = new XMLStream();
                var bytes = m_netStream.Read(inputXMLStream.getBuffer(), inputXMLStream.getBufferOffset(),
                    inputXMLStream.getBufferSize() - inputXMLStream.getBufferOffset());

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


        public async void CommunicationCallback(IAsyncResult ar)
        {
            if (getState() != AgentState.Busy)
            {
                acceptJobQuery(ar);

                try
                {
                    setState(AgentState.Busy);

                    int ret = await readAsync(m_cancelTokenSource.Token);
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
                            bool bret = await ReceiveJobQuery(m_cancelTokenSource.Token);
                            if (bret)
                            {
                                //run the job
                                logMessage("Job received");
                                logMessage(m_job.ToString());
                                logMessage("Running job");
                                returnCode = await runJobAsync(m_cancelTokenSource.Token);

                                if (returnCode == m_noErrorCode || returnCode == m_jobInternalErrorCode)
                                {
                                    logMessage("Job finished. Code=" + returnCode );
                                    await writeMessageAsync(CJobDispatcher.m_endMessage, m_cancelTokenSource.Token, true);

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
                    logMessage("Waiting for queued async write operations to finish");
                    waitAsyncWriteOpsToFinish();
                    logMessage("Closing the TCP connection");
                    m_tcpClient.Close();
                    setState(AgentState.Available);

                    //try to recover
                    //start listening again
                    HerdAgentTcpState tcpState = new HerdAgentTcpState();
                    tcpState.ip = new IPEndPoint(0, 0);

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
                        //logMessage("Agent discovered by " + ip + ". Current state=" + getStateString());
                        string agentDescription = GetAgentDescription();
                        byte[] data = Encoding.ASCII.GetBytes(agentDescription);
                        getUdpClient().Send(data, data.Length, ip);
                    }
                    //else logMessage("Agent contacted by " + ip.ToString() + " but rejected connection because it was busy");
                }
                else logMessage("Message received by " + ip + " not understood: " + receiveString);

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
