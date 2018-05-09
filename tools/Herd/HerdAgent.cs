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
using System.Globalization;

namespace Herd
{


    public class HerdAgentInfo
    {
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

        /// <summary>
        /// Constructor used for testing purposes
        /// </summary>
        public HerdAgentInfo(string processorId, int numCPUCores, string architecture, string CUDAVersion, string herdAgentVersion)
        {
            m_ipAddress = new IPEndPoint(0, 0);
            m_properties = new Dictionary<string, string>
            {
                [PropNames.ProcessorId] = processorId,
                [PropNames.NumCPUCores] = numCPUCores.ToString(),
                [PropNames.Architecture] = architecture,
                [PropNames.CUDA] = CUDAVersion,
                [PropNames.HerdAgentVersion] = herdAgentVersion
            };
        }

        public void AddProperty(string name, string value)
        {
            m_properties.Add(name, value);
        }

        public string Property(string name)
        {
            if (m_properties.ContainsKey(name))
                return m_properties[name];

            return PropValues.None;
        }


        public void Parse(XElement xmlDescription)
        {
            if (xmlDescription.Name.ToString() == XmlTags.HerdAgentDescription)
            {
                m_properties.Clear();

                foreach (XElement child in xmlDescription.Elements())
                    AddProperty(child.Name.ToString(), child.Value);
            }
        }


        public override string ToString()
        {
            string res = "";
            foreach (var property in m_properties)
                res += property.Key + "=\"" + property.Value + "\";";

            return res;
        }


        public string State { get { return Property(PropNames.State); } set { } }

        public string Version { get { return Property(PropNames.HerdAgentVersion); } set { } }

        public string ProcessorId { get { return Property(PropNames.ProcessorId); } }

        public int NumProcessors
        {
            get
            {
                string prop = Property(PropNames.NumCPUCores);
                return (!prop.Equals(PropValues.None)) ? int.Parse(prop) : 0;
            }
        }

        public string ProcessorArchitecture { get { return Property(PropNames.Architecture); } }

        public double ProcessorLoad
        {
            get
            {
                string prop = Property(PropNames.ProcessorLoad);
                return (!prop.Equals(PropValues.None)) ? double.Parse(prop) : 0.0;
            }
        }

        public double Memory
        {
            get
            {
                string prop = Property(PropNames.TotalMemory);
                return (!prop.Equals(PropValues.None)) ? double.Parse(prop) : 0.0;
            }
        }

        public string CUDA { get { return Property(PropNames.CUDA); } }

        public bool IsAvailable
        {
            get { if (Property(PropNames.State) == PropValues.StateAvailable) return true; return false; }
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
        [DllImport("kernel32.dll")]
        private static extern void GetNativeSystemInfo(ref SYSTEM_INFO lpSystemInfo);

        private const int PROCESSOR_ARCHITECTURE_AMD64 = 9;
        private const int PROCESSOR_ARCHITECTURE_IA64 = 6;
        private const int PROCESSOR_ARCHITECTURE_INTEL = 0;

        [StructLayout(LayoutKind.Sequential)]
        private struct SYSTEM_INFO
        {
            public short wProcessorArchitecture;
            public short wReserved;
            public int dwPageSize;
            public IntPtr lpMinimumApplicationAddress;
            public IntPtr lpMaximumApplicationAddress;
            public IntPtr dwActiveProcessorMask;
            public int dwNumberOfProcessors;
            public int dwProcessorType;
            public int dwAllocationGranularity;
            public short wProcessorLevel;
            public short wProcessorRevision;
        }
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

        private CancellationTokenSource m_cancelTokenSource;

        private PerformanceCounter m_cpuCounter;

        /// <summary>
        ///     HerdAgent class constructor
        /// </summary>
        /// <param name="cancelTokenSource"></param>
        public HerdAgent(CancellationTokenSource cancelTokenSource, Logger.LogFunction logMessageHandler= null)
        {
            //Set invariant culture to use english notation
            Thread.CurrentThread.CurrentCulture = System.Globalization.CultureInfo.InvariantCulture;

            setLogMessageHandler(logMessageHandler);
            m_cancelTokenSource = cancelTokenSource;
            m_state = AgentState.Available;
            m_cpuCounter = new PerformanceCounter("Processor", "% Processor Time", "_Total");
            try
            {
                m_credentials = Credentials.Load(Path.GetDirectoryName(System.Reflection.Assembly.GetEntryAssembly().Location));
            }
            catch
            {
                //we do nothing for now
            }
            InitStaticProperties();
        }

        protected override string TmpDir()
        {
            if (m_credentials == null)
                return base.TmpDir();
            else
                return m_credentials.GetTempDir();
        }

        public string DirPath { get { return m_dirPath; } }

        public void CleanCacheDir()
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
            m_job.Tasks.Clear();
            m_job.InputFiles.Clear();
            m_job.OutputFiles.Clear();

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


        public static Task WaitForExitAsync(Process process, CancellationToken cancellationToken)
        {
            var tcs = new TaskCompletionSource<object>();
            process.EnableRaisingEvents = true;
            process.Exited += (sender, args) => { tcs.TrySetResult(null); };
            //if(cancellationToken != default(CancellationToken))
            cancellationToken.Register(tcs.SetCanceled);
            return tcs.Task;
        }


        public async Task<int> RunTaskAsync(HerdTask task, CancellationToken cancelToken)
        {
            int returnCode = m_noErrorCode;
            NamedPipeServerStream pipeServer = null;
            Process myProcess = new Process();
            if (task.Pipe != "")
                pipeServer = new NamedPipeServerStream(task.Pipe);
            XMLStream xmlStream = new XMLStream();

            try
            {
                //not to read 23.232 as 23232
                Thread.CurrentThread.CurrentCulture = System.Globalization.CultureInfo.InvariantCulture;

                if (m_credentials != null)
                {
                    if (!m_credentials.AuthenticationCode.Equals(task.AuthenticationToken))
                    {
                        logMessage("Authentication tokens did NOT match. Won't execute command: " + getCachedFilename(task.Exe) + " " + task.Arguments);

                        await xmlStream.writeMessageAsync(m_tcpClient.GetStream(),
                       "<" + task.Pipe + "><End>Authentication Error</End></" + task.Pipe + ">", cancelToken);

                        returnCode = -1;
                        throw new AuthenticationException("AuthenticationToken of the task does not match local token.");
                    }

                    myProcess = WinApi.StartUnelevatedProcess(getCachedFilename(task.Exe), task.Arguments, Path.GetDirectoryName((getCachedFilename(task.Exe))));
                    // myProcess = WinApi.StartUnelevatedProcess(@"C:\Users\Roland\Source\Repos\RLSimion\Debug\HerdAgentServiceSettings.exe", task.arguments, Path.GetDirectoryName(@"C:\Users\Roland\Source\Repos\RLSimion\Debug\HerdAgentServiceSettings.exe"));
                }

                myProcess.StartInfo.FileName = getCachedFilename(task.Exe);
                myProcess.StartInfo.Arguments = task.Arguments;
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
                        CheckCancellationRequests();

                        //check if we have been asked to cancel
                        cancelToken.ThrowIfCancellationRequested();

                        int numBytes = await xmlStream.readFromNamedPipeStreamAsync(pipeServer, cancelToken);
                        xmlItem = xmlStream.processNextXMLItem();
                        while (xmlItem != "")
                        {
                            await xmlStream.writeMessageAsync(m_tcpClient.GetStream(),
                                "<" + task.Pipe + ">" + xmlItem + "</" + task.Pipe + ">", cancelToken);

                            xmlItem = xmlStream.processNextXMLItem();
                        }
                    }
                }

                logMessage("Named pipe has been closed for task " + task.Name);
                await WaitForExitAsync(myProcess, cancelToken);

                int exitCode = myProcess.ExitCode;
                logMessage("Process exited in task " + task.Name + ". Return code=" + exitCode);
                //myProcess.WaitForExit();

                if (exitCode < 0)
                {
                    await xmlStream.writeMessageAsync(m_tcpClient.GetStream(),
                        "<" + task.Pipe + "><End>Error</End></" + task.Pipe + ">", cancelToken);
                    returnCode = m_jobInternalErrorCode;
                }
                else
                    await xmlStream.writeMessageAsync(m_tcpClient.GetStream(),
                        "<" + task.Pipe + "><End>Ok</End></" + task.Pipe + ">", cancelToken);
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
                logMessage("Task " + task.Name + " finished");
                //removeSpawnedProcessFromList(myProcess);
                if (pipeServer != null) pipeServer.Close();
            }

            return returnCode;
        }


        public async Task<int> RunJobAsync(CancellationToken cancelToken)
        {
            int returnCode = m_noErrorCode;
            try
            {
                List<Task<int>> taskList = new List<Task<int>>();
                foreach (HerdTask task in m_job.Tasks)
                    taskList.Add(RunTaskAsync(task, cancelToken));
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


        public AgentState State { get { return m_state; } set { m_state = value; } }


        public string StateString()
        {
            if (m_state == AgentState.Available) return PropValues.StateAvailable;
            if (m_state == AgentState.Busy) return PropValues.StateBusy;
            if (m_state == AgentState.Cancelling) return PropValues.StateCancelling;
            return PropValues.None;
        }


         public float CurrentCpuUsage()
         {
            return m_cpuCounter.NextValue();
         }

        static public string ProcessorId()
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

        string HerdAgentVersion = PropValues.None;
        string CPUId = PropValues.None;
        int NumCPUCores = 0;
        string CPUArchitecture = PropValues.None;
        double TotalMemory = 0.0;



        static public string ArchitectureId()
        {
            SYSTEM_INFO si = new SYSTEM_INFO();
            GetNativeSystemInfo(ref si);
            switch (si.wProcessorArchitecture)
            {
                case PROCESSOR_ARCHITECTURE_AMD64:
                case PROCESSOR_ARCHITECTURE_IA64:
                    return PropValues.Win64;

                case PROCESSOR_ARCHITECTURE_INTEL:
                default:
                    return PropValues.Win32;
            }
        }
        /// <summary>
        /// This method should be called once on initialization to set static properties: CUDA support
        /// , number of cores, ... No point checking them every time we get a ping from the client
        /// </summary>
        private void InitStaticProperties()
        {
            logMessage("Initializing Herd Agent static properties:");

            //Herd Agent version
            HerdAgentVersion = System.Reflection.Assembly.GetExecutingAssembly().GetName().Version.ToString();
            //Number of CPU cores
            NumCPUCores = Environment.ProcessorCount;
            //Processor Id
            CPUId = ProcessorId();
            // CPU architecture
           // CPUArchitecture = Environment.GetEnvironmentVariable("PROCESSOR_ARCHITECTURE", EnvironmentVariableTarget.Machine);
            CPUArchitecture = ArchitectureId();
            // Total installed memory
            TotalMemory = GetGlobalMemoryStatusEx();
            //CUDA support
            CUDAVersion= CUDASupport();

            logMessage(AgentDescription());
        }

        string CUDAVersion = "";

        /// <summary>
        ///     Get information about CUDA installation.
        /// </summary>
        /// <returns>The CUDA version installed or -1 if none was found</returns>
        static public string CUDASupport()
        {
            string dllName = @"nvcuda.dll";
            string dir = Environment.SystemDirectory;

            try
            {
                FileVersionInfo myFileVersionInfo = null;

                var dllPath = dir + "\\" + dllName;
                if (System.IO.File.Exists(dllPath))
                {
                    myFileVersionInfo = FileVersionInfo.GetVersionInfo(dllPath);
                    //This is a quick and dirty hack to get the CUDA version without loading the DLL
                    //It seems that nVidia sets the ProductName with the cuda version inside. I hope they
                    //keep doint it
                    string version = myFileVersionInfo.ProductName;
                    return new string(version.Where(c => char.IsDigit(c) || char.IsPunctuation(c)).ToArray());
                }
                else
                {
                    return PropValues.None;
                }
            }
            catch (Exception ex)
            {
                Console.Write(ex.StackTrace);
                return PropValues.None;
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

        public string AgentDescription()
        {
            string description = "<" + XmlTags.HerdAgentDescription + ">"
                // HerdAgent version
                + "<" + PropNames.HerdAgentVersion + ">"
                + HerdAgentVersion
                + "</" + PropNames.HerdAgentVersion + ">"
                // CPU amount of cores
                + "<" + PropNames.ProcessorId + ">" + CPUId + "</" + PropNames.ProcessorId + ">"
                // CPU amount of cores
                + "<" + PropNames.NumCPUCores + ">" + NumCPUCores + "</" + PropNames.NumCPUCores + ">"
                // CPU architecture
                + "<" + PropNames.Architecture + ">" + CPUArchitecture + "</" + PropNames.Architecture + ">"
                // Current processor load
                + "<" + PropNames.ProcessorLoad + ">" + CurrentCpuUsage().ToString(CultureInfo.InvariantCulture) + "</" + PropNames.ProcessorLoad + ">"
                // Total installed memory
                + "<" + PropNames.TotalMemory + ">" + TotalMemory + "</" + PropNames.TotalMemory + ">"
                // CUDA support information
                + "<" + PropNames.CUDA + ">" + CUDAVersion + "</" + PropNames.CUDA + ">"
                // HerdAgent state
                + "<" + PropNames.State + ">" + StateString() + "</" + PropNames.State + ">"
                + "</" + XmlTags.HerdAgentDescription + ">";
            return description;
        }


        public void AcceptJobQuery(IAsyncResult ar)
        {
            m_tcpClient = m_listener.EndAcceptTcpClient(ar);
            m_xmlStream.resizeBuffer(m_tcpClient.ReceiveBufferSize);
            m_netStream = m_tcpClient.GetStream();
        }


        public void CheckCancellationRequests()
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
            if (State != AgentState.Busy)
            {
                AcceptJobQuery(ar);

                try
                {
                    State = AgentState.Busy;

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
                            CleanCacheDir();
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
                                returnCode = await RunJobAsync(m_cancelTokenSource.Token);

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
                    State = AgentState.Available;

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
                        string agentDescription = AgentDescription();
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


        public void StartListening()
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

            m_listener.BeginAcceptTcpClient(CommunicationCallback, tcpState);
        }


        public void StopListening()
        {
            m_discoveryClient.Close();
            m_listener.Stop();
        }
    }

}
