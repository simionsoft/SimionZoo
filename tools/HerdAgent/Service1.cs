using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Linq;
using System.ServiceProcess;
using System.Text;
using System.Threading.Tasks;
using System.Threading;
using System.Net;
using System.Net.Sockets;
using System.IO;
using Herd;

//Installing/uninstalling the service: https://msdn.microsoft.com/en-us/library/zt39148a%28v=vs.110%29.aspx?f=255&MSPPError=-2147217396#BK_Install

namespace Herd
{
    
    
    
    public partial class HerdService : ServiceBase
    {
        private static HerdAgent m_herdAgent;
        private static UdpClient m_discoveryClient;
        private static TcpListener m_listener;
        private static string m_dirPath;
        private static AgentState m_state;
        private static object m_logFileLock= new object();

        public HerdService()
        {
            m_dirPath = Path.GetDirectoryName(System.Reflection.Assembly.GetEntryAssembly().Location) + "\\temp";
            Directory.CreateDirectory(m_dirPath);
            
            InitializeComponent();
        }
        public static string getLogFilename()
        { return m_dirPath + "//log.txt"; }
        public static void cleanCacheDir()
        {
            Directory.Delete(m_dirPath, true);
            Directory.CreateDirectory(m_dirPath);
        }
        public static void cleanLog()
        {
            string logFile = getLogFilename();
            lock (m_logFileLock)
            {
                FileStream file = File.Create(logFile);
                file.Close();
            }
        }
        public static void Log(string logMessage)
        {
            lock (m_logFileLock)
            {
                if (File.Exists(getLogFilename()))
                {
                    string text = DateTime.Now.ToLongTimeString() + " " +
                        DateTime.Now.ToLongDateString() + ": " + logMessage;
                    StreamWriter w = File.AppendText(getLogFilename());

                    w.WriteLine(text);
                    w.Close();
                    Console.WriteLine(text);
                }
            }
        }
        public class TCPCallbackInfo
        {
            public HerdAgent herdAgent { get; set; }
            public NetworkStream netStream { get; set; }
            public XMLStream xmlStream { get; set; }
        }
        public static async Task asyncReadFromClient(NetworkStream netStream, CancellationToken ct)
        {
            XMLStream inputXMLStream= new XMLStream();
            int bytes = 0;

            try
            {
                while (true)
                {
                    bytes = await netStream.ReadAsync(inputXMLStream.getBuffer(), inputXMLStream.getBufferOffset()
                        , inputXMLStream.getBufferSize()-inputXMLStream.getBufferOffset(), ct);

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
                            Log("Stopping job execution");
                            m_herdAgent.Stop();
                        }
                    }
                };
            }
            catch (OperationCanceledException)
            {
                Log("Thread finished gracefully");
            }
            catch(ObjectDisposedException)
            {
                Log("Network stream closed: async read finished");
            }
            catch (Exception ex)
            {
                Log(ex.ToString());
            }
        }
        //public static void QuitCallback(IAsyncResult callbackInfo)
        //{
        //    HerdAgent herdAgent= ((TCPCallbackInfo)callbackInfo.AsyncState).herdAgent;
        //    XMLStream xmlStream= ((TCPCallbackInfo)callbackInfo.AsyncState).xmlStream;
        //    NetworkStream netStream = ((TCPCallbackInfo)callbackInfo.AsyncState).netStream;
        //    try
        //    {
        //        int bytes = netStream.EndRead(callbackInfo);
        //        if (bytes == 0)
        //        {
        //            netStream.Close();
        //            netStream.Dispose();
        //            return;
        //        }

        //        xmlStream.addBytesRead(bytes); // we let the xmlstream object know that some bytes have been read in its buffer
        //        string xmlItem = xmlStream.peekNextXMLItem();
        //        if (xmlItem != "")
        //        {
        //            string xmlItemContent = xmlStream.getLastXMLItemContent();
        //            if (xmlItemContent == CJobDispatcher.m_quitMessage)
        //            {
        //                Log("Stopping job execution");
        //                m_herdAgent.Stop();
        //            }
        //        }
        //    }
        //    catch(Exception ex)
        //    {
        //        Log("Error reading from the client");
        //    }
        //}
        public static void CommunicationCallback(IAsyncResult ar)
        {
            if (m_state != AgentState.BUSY)
            {
                TcpClient comSocket = m_listener.EndAcceptTcpClient(ar);
                NetworkStream netStream = comSocket.GetStream();
                CancellationTokenSource cancelToken = new CancellationTokenSource();
                try
                {
                    m_state = AgentState.BUSY;
                    m_herdAgent = new HerdAgent(comSocket, netStream, m_dirPath,Log);
                    m_herdAgent.read();
                    string xmlItem = m_herdAgent.processNextXMLItem();
                    string xmlItemContent;
                    int returnCode;

                    if (xmlItem != "")
                    {
                        xmlItemContent = m_herdAgent.getLastXMLItemContent();
                        if (xmlItemContent==CJobDispatcher.m_cleanCacheMessage)
                        {
                            //not yet implemented in the herd client, just in case...
                            Log("Cleaning cache directory");
                            cleanLog();
                        }
                        else if (xmlItemContent == CJobDispatcher.m_aquireMessage)
                        {
                            Log("Receiving job data from" + comSocket.Client.RemoteEndPoint.ToString());
                            if (m_herdAgent.ReceiveJobQuery())
                            {
                                //Listen for a "quit" message from the client
                                //TCPCallbackInfo callbackInfo= new TCPCallbackInfo();
                                //callbackInfo.herdAgent = m_herdAgent;
                                //callbackInfo.netStream = netStream;
                                //callbackInfo.xmlStream = new XMLStream();
                                //netStream.BeginRead(callbackInfo.xmlStream.getBuffer(), 0, callbackInfo.xmlStream.getBufferSize()
                                //    , QuitCallback, (object)callbackInfo);
                                Task.Factory.StartNew(() => asyncReadFromClient(netStream,cancelToken.Token));

                                //run the job
                                Log("Running job");
                                returnCode= m_herdAgent.RunJob(netStream);

                                if (returnCode == HerdAgent.m_noErrorCode)
                                {
                                    Log("Job finished");
                                    m_herdAgent.writeMessage(CJobDispatcher.m_endMessage, true);

                                    Log("Sending job results");
                                    m_herdAgent.SendJobResult();

                                    Log("Job results sent");
                                }
                                else if (returnCode==HerdAgent.m_jobInternalErrorCode)
                                {
                                    Log("The job returned an error code");
                                    m_herdAgent.writeMessage(CJobDispatcher.m_errorMessage, true);
                                }
                                else if (returnCode==HerdAgent.m_remotelyCancelledErrorCode)
                                {
                                    Log("The job was remotely cancelled");
                                }
                            }
                        }
                    }
                }
                catch (Exception ex)
                {
                    Log(ex.ToString() + ex.InnerException + ex.StackTrace);
                }
                finally
                {
                    cancelToken.Cancel();
                    cancelToken.Dispose();
                    comSocket.Close();
                    m_state = AgentState.AVAILABLE;

                    //try to recover
                    //start listening again
                    TCPState tcpState = new TCPState();
                    tcpState.ip = new IPEndPoint(0, 0);
                    m_listener.Start();
                    m_listener.BeginAcceptTcpClient(CommunicationCallback, tcpState);
                }
            }
        }

        public static void DiscoveryCallback(IAsyncResult ar)
        {
            IPEndPoint ip = ((UdpState)ar.AsyncState).ip;


            Byte[] receiveBytes = m_discoveryClient.EndReceive(ar, ref ip);
            string receiveString = Encoding.ASCII.GetString(receiveBytes);

            if (receiveString == CJobDispatcher.m_discoveryMessage)
            {
                if (m_state == AgentState.AVAILABLE)
                {
                    Log("Agent discovered by " + ip.ToString());
                    byte[] data = Encoding.ASCII.GetBytes("<Cores>" + Environment.ProcessorCount + "</Cores>");
                    m_discoveryClient.Send(data, data.Length, ip);
                }
                else Log("Agent contacted by " + ip.ToString() + " but rejected connection because it was busy");
            }
            else Log("Message received by " + ip.ToString() + " not understood: " + receiveString);

            m_discoveryClient.BeginReceive(new AsyncCallback(DiscoveryCallback), ar.AsyncState);
        }
       
        protected override void OnStart(string[] args)
        {

            DoStart();

        }
        public void DoStart()
        {
            m_state = AgentState.AVAILABLE;
            cleanLog();
            Log("Herd agent started");

            //UPD broadcast client

            m_discoveryClient = new UdpClient(CJobDispatcher.m_discoveryPortHerd);
            UdpState state = new UdpState();
            IPEndPoint shepherd = new IPEndPoint(0, 0);
            UdpState u = new UdpState();
            u.ip = shepherd;

            m_discoveryClient.BeginReceive(DiscoveryCallback, u);

            //TCP communication socket

            m_listener = new TcpListener(IPAddress.Any, CJobDispatcher.m_comPortHerd);
            m_listener.Start();
            TCPState tcpState = new TCPState();
            tcpState.ip = shepherd;
            m_listener.BeginAcceptTcpClient(CommunicationCallback, tcpState);
        }

        protected override void OnStop()
        {
            DoStop();
        }
        public void DoStop()
        {
            Log("Herd Agent stopped");
            m_discoveryClient.Close();
            m_listener.Stop();
        }
    }
}
