using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Linq;
using System.ServiceProcess;
using System.Text;
using System.Threading.Tasks;
using System.Net;
using System.Net.Sockets;
using System.IO;
using Herd;

//Installing/uninstalling the service: https://msdn.microsoft.com/en-us/library/zt39148a%28v=vs.110%29.aspx?f=255&MSPPError=-2147217396#BK_Install

namespace Herd
{
    
    
    
    public partial class HerdService : ServiceBase
    {
        private static HerdAgent herdAgent;
        private static UdpClient m_discoveryClient;
        private static TcpListener m_listener;
        private static string dirPath;
        private static AgentState m_state;

        public HerdService()
        {
            dirPath = Path.GetDirectoryName(System.Reflection.Assembly.GetEntryAssembly().Location) + "\\temp";
            Directory.CreateDirectory(dirPath);
            
            InitializeComponent();
        }
        public static string getLogFilename()
        { return dirPath + "//log.txt"; }
        public static void cleanLog()
        {
            string logFile = getLogFilename();
            if (File.Exists(logFile))
                File.Delete(logFile);
        }
        public static void Log(string logMessage)
        {
            if (File.Exists(getLogFilename()))
            {
                StreamWriter w = File.AppendText(getLogFilename());
                w.Write("\r\nLog Entry : ");
                w.WriteLine("{0} {1}", DateTime.Now.ToLongTimeString(),
                    DateTime.Now.ToLongDateString());
                w.WriteLine("  :");
                w.WriteLine("  :{0}", logMessage);
                w.WriteLine("-------------------------------");
                w.Close();
            }
        }
        public static void CommunicationCallback(IAsyncResult ar)
        {
            if (m_state != AgentState.BUSY)
            {
                TcpClient comSocket = m_listener.EndAcceptTcpClient(ar);
                NetworkStream netStream = comSocket.GetStream();

                try
                {
                    m_state = AgentState.BUSY;
                    herdAgent = new HerdAgent(comSocket, netStream, dirPath);
                    herdAgent.read();
                    string xmlItem = herdAgent.processNextXMLItem();
                    string xmlItemContent;
                    if (xmlItem != "")
                    {
                        xmlItemContent = herdAgent.getLastXMLItemContent();
                        if (xmlItemContent == CJobDispatcher.m_freeMessage)
                        {
                            //we do nothing and keep listening
                            Log("This slave was discovered but not used");
                        }
                        else if (xmlItemContent == CJobDispatcher.m_aquireMessage)
                        {
                            Log("Receiving job data from" + comSocket.Client.RemoteEndPoint.ToString());
                            if (herdAgent.ReceiveJobQuery())
                            {
                                Log("Running job");
                                herdAgent.RunJob(netStream);

                                Log("Job finished");
                                herdAgent.writeMessage(CJobDispatcher.m_endMessage, true);

                                Log("Sending job results");
                                herdAgent.SendJobResult();

                                Log("Job results sent");
                            }
                        }
                    }
                    netStream.Close();
                    netStream.Dispose();
                    m_listener.Stop();
                    comSocket.Close();
                    m_state = AgentState.AVAILABLE;
                }
                catch (Exception ex)
                {
                    netStream.Close();
                    netStream.Dispose();
                    m_listener.Stop();
                    comSocket.Close();
                    m_state = AgentState.AVAILABLE;
                    Log(ex.StackTrace);
                }
            }
            //start listening again
            m_listener.BeginAcceptTcpClient(CommunicationCallback,ar);
        }
        public static void DiscoveryCallback(IAsyncResult ar)
        {
            IPEndPoint ip = ((UdpState)ar.AsyncState).ip;


            Byte[] receiveBytes = m_discoveryClient.EndReceive(ar, ref ip);
            string receiveString = Encoding.ASCII.GetString(receiveBytes);

            if (receiveString == "QUIT")
            {
                Console.WriteLine("Quit message received");
                if (m_state == AgentState.BUSY)
                {
                    m_state = AgentState.CANCELING;
                    herdAgent.stop();
                    m_state = AgentState.AVAILABLE;
                }


            }
            else if (m_state == AgentState.AVAILABLE)
            {

                Log("Agent discovered");
                byte[] data = Encoding.ASCII.GetBytes("<Cores>" + Environment.ProcessorCount + "</Cores>");
                m_discoveryClient.Send(data, data.Length, ip);
            }

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
            m_discoveryClient.Close();
            m_listener.Stop();
        }
    }
}
