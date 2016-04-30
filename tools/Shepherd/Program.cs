using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net;
using System.Net.Sockets;
using System.IO;
using NetJobTransfer;

namespace Shepherd
{
    enum AgentState { BUSY, AVAILABLE, DISCOVERED };
    enum FileType { EXE, INPUT, OUTPUT};

    class Program
    {

        private static AgentState m_state;
        private static UdpClient m_discoverySocket;


        
        static void Main(string[] args)
        {
            CJobDispatcher m_jobSender= new CJobDispatcher();
            m_discoverySocket = new UdpClient();//(m_discoveryPortShepherd);

            var TcpSocket = new TcpListener(IPAddress.Any, CJobDispatcher.m_comPortShepherd);
            TcpSocket.Start();

            var RequestData = Encoding.ASCII.GetBytes(CJobDispatcher.m_discoveryMessage);
            var ServerEp = new IPEndPoint(IPAddress.Any, CJobDispatcher.m_discoveryPortHerd);

            m_discoverySocket.EnableBroadcast = true;
            System.Threading.Thread.Sleep(1000); //so that the shepherd waits for the herd agent to be ready
            m_discoverySocket.Send(RequestData, RequestData.Length, new IPEndPoint(IPAddress.Broadcast, CJobDispatcher.m_discoveryPortHerd));


            using (TcpClient comSocket= TcpSocket.AcceptTcpClient())
            {
                using (NetworkStream netStream = comSocket.GetStream())
                {
                    CJob job= new CJob();
                    job.name = "test-job";
                    job.exeFile = "..\\Debug\\RLSimion.exe";
                    job.inputFiles.Add("..\\experiments\\examples\\uv-pid.node");
                    m_jobSender.SendJobQuery(netStream, job);
                }
                comSocket.Close();
            }
            TcpSocket.Stop();
            //var ServerResponseData = Client.Receive(ref ServerEp);
            //var ServerResponse = Encoding.ASCII.GetString(ServerResponseData);
            //Console.WriteLine("Received {0} from {1}", ServerResponse, ServerEp.Address.ToString());

            m_discoverySocket.Close();
        }
    }
}
