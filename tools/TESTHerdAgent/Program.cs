using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net;
using System.Net.Sockets;
using System.IO;
using System.Text.RegularExpressions;
using NetJobTransfer;

namespace TESTHerdAgent
{
    enum AgentState { BUSY, AVAILABLE, DISCOVERED };
    enum FileType { EXE, INPUT, OUTPUT};

    class Program
    {
        
       





      

        //public static void DiscoveryCallback(IAsyncResult ar)
        //{
        //    UdpClient socket = ar.AsyncState as UdpClient;
        //    // points towards whoever had sent the message:
        //    IPEndPoint source = new IPEndPoint(0, 0);
        //    // get the actual message and fill out the source:
        //    string message = Encoding.ASCII.GetString(socket.EndReceive(ar, ref source));

        //    StreamWriter fileWriter = new StreamWriter("log.txt",true);
        //    fileWriter.WriteLine("Received message from" + source);
        //    fileWriter.WriteLine("Message= " + message);

        //    if (message == m_discoveryMessage && m_state!=AgentState.BUSY)
        //    {
        //        fileWriter.WriteLine("Show");
        //        socket.Send(Encoding.ASCII.GetBytes(m_discoveryAnswer),Encoding.ASCII.GetBytes(m_discoveryAnswer).Length);
        //    }
        //    fileWriter.Close();

        //    socket.BeginReceive(new AsyncCallback(DiscoveryCallback), socket);
        //}
        static void Main(string[] args)
        {
            AgentState m_state;
            UdpClient m_discoverySocket;
            TcpClient m_comSocket;
            m_state = AgentState.AVAILABLE;
            m_discoverySocket = new UdpClient(CJobDispatcher.m_discoveryPortHerd);
            IPEndPoint shepherd= new IPEndPoint(0,0);
            HerdAgent herdAgent;
           // m_discoverySocket.BeginReceive(new AsyncCallback(DiscoveryCallback), m_discoverySocket);

            while (true)
            {
                byte[] message = m_discoverySocket.Receive(ref shepherd);
                System.Console.WriteLine("Message received from " + shepherd);
                System.Console.WriteLine(Encoding.ASCII.GetString(message));
                System.Console.WriteLine();

                using (m_comSocket = new TcpClient(shepherd.Address.ToString(), CJobDispatcher.m_comPortShepherd))
                {
                    NetworkStream netStream = m_comSocket.GetStream();

                    herdAgent = new HerdAgent();
                    if (herdAgent.ReceiveJobQuery(netStream))
                    {
                        herdAgent.RunJob();
                        herdAgent.SendJobResult(netStream);
                    }
                    m_comSocket.Close();
                }
            }

            m_discoverySocket.Close();
        }


    }
}
