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
    
    
    enum AgentState { BUSY, AVAILABLE, DISCOVERED };
    public partial class HerdService : ServiceBase
    {
        const int m_discoveryPort = 2332;
        const int m_comPort = 2333;
        const string m_discoveryMessage = "Slaves, show yourselves!";
        const string m_discoveryAnswer = "At your command, my Master";

        private static AgentState m_state;
        private UdpClient m_discoverySocket;

        public HerdService()
        {
            InitializeComponent();
        }

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

        protected override void OnStart(string[] args)
        {
            m_state = AgentState.AVAILABLE;
            m_discoverySocket = new UdpClient(m_discoveryPort);
            m_discoverySocket.BeginReceive(new AsyncCallback(DiscoveryCallback), m_discoverySocket);
        }

        protected override void OnStop()
        {
            m_discoverySocket.Close();
        }
    }
}
