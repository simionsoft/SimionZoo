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


//Installing/uninstalling the service: https://msdn.microsoft.com/en-us/library/zt39148a%28v=vs.110%29.aspx?f=255&MSPPError=-2147217396#BK_Install

namespace HerdAgent
{
    
    
    
    public partial class HerdService : ServiceBase
    {
        static HerdAgent herdAgent;
        const string m_discoveryMessage = "Slaves, show yourselves!";
        const string m_discoveryAnswer = "At your command, my Master";
        private static string dirPath;
        private static AgentState m_state;
        private UdpClient m_discoverySocket;

        public HerdService()
        {
            dirPath = Path.GetDirectoryName(System.Reflection.Assembly.GetEntryAssembly().Location) + "\\temp";
            
            InitializeComponent();
        }
        public static void cleanLog()
        {
            File.Delete(dirPath + "//log.txt");
        }
        public static void Log(string logMessage)
        {
            StreamWriter w = File.AppendText(dirPath + "//log.txt");
            w.Write("\r\nLog Entry : ");
            w.WriteLine("{0} {1}", DateTime.Now.ToLongTimeString(),
                DateTime.Now.ToLongDateString());
            w.WriteLine("  :");
            w.WriteLine("  :{0}", logMessage);
            w.WriteLine("-------------------------------");
            w.Close();
        }
        public static void DiscoveryCallback(IAsyncResult ar)
        {

            UdpClient u = (UdpClient)((UdpState)(ar.AsyncState)).u;
            IPEndPoint e = (IPEndPoint)((UdpState)(ar.AsyncState)).e;


            Byte[] receiveBytes = u.EndReceive(ar, ref e);
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
                cleanLog();
                Log("Agent discovered");
                byte[] data = Encoding.ASCII.GetBytes("<Cores>" + Environment.ProcessorCount + "</Cores>");
                u.Send(data, data.Length, e);
                m_state = AgentState.DISCOVERED;
                m_state = AgentState.BUSY;
                var server = new TcpListener(IPAddress.Any, CJobDispatcher.m_comPortHerd);
                server.Start();
                TcpClient m_comSocket = server.AcceptTcpClient();
                NetworkStream netStream = m_comSocket.GetStream();
                try
                {

                    herdAgent = new HerdAgent(m_comSocket, netStream, dirPath);
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
                            Log("Receiving job data from" + m_comSocket.Client.RemoteEndPoint.ToString());
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
                    server.Stop();
                    m_comSocket.Close();
                    m_state = AgentState.AVAILABLE;
                }
                catch (Exception ex)
                {
                    netStream.Close();
                    netStream.Dispose();
                    server.Stop();
                    m_comSocket.Close();
                    m_state = AgentState.AVAILABLE;
                    Log(ex.StackTrace);
                }
               

            }

            u.BeginReceive(new AsyncCallback(DiscoveryCallback), ar.AsyncState);

           
        }
        /*
        public static void DiscoveryCallback(IAsyncResult ar)
        {
            UdpClient socket = ar.AsyncState as UdpClient;
            // points towards whoever had sent the message:
            IPEndPoint source = new IPEndPoint(0, 0);
            // get the actual message and fill out the source:
            string message = Encoding.ASCII.GetString(socket.EndReceive(ar, ref source));

            StreamWriter fileWriter = new StreamWriter("log.txt",true);
            fileWriter.WriteLine("Received message from" + source);
            fileWriter.WriteLine("Message= " + message);

            if (message == m_discoveryMessage && m_state!=AgentState.BUSY)
            {
                fileWriter.WriteLine("Show");
                socket.Send(Encoding.ASCII.GetBytes(m_discoveryAnswer),Encoding.ASCII.GetBytes(m_discoveryAnswer).Length);
            }
            fileWriter.Close();

            socket.BeginReceive(new AsyncCallback(DiscoveryCallback), socket);
        }
        */
        protected override void OnStart(string[] args)
        {
            m_state = AgentState.AVAILABLE;

            UdpClient m_discoverySocket;
            m_discoverySocket = new UdpClient(CJobDispatcher.m_discoveryPortHerd);
            UdpState state = new UdpState();
            IPEndPoint shepherd = new IPEndPoint(0, 0);
            UdpState u = new UdpState();
            u.e = shepherd;
            u.u = m_discoverySocket;
            u.c = herdAgent;

            m_discoverySocket.BeginReceive(DiscoveryCallback, u);

            //m_discoverySocket = new UdpClient(m_discoveryPort);
            //m_discoverySocket.BeginReceive(new AsyncCallback(DiscoveryCallback), m_discoverySocket);
        }

        protected override void OnStop()
        {
            m_discoverySocket.Close();
        }
    }
}
