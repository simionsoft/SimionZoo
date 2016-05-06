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
            TcpClient m_comSocket=null;
            m_state = AgentState.AVAILABLE;
            m_discoverySocket = new UdpClient(CJobDispatcher.m_discoveryPortHerd);
            IPEndPoint shepherd= new IPEndPoint(0,0);
            HerdAgent herdAgent;
            while (true)
            {
                byte[] message = m_discoverySocket.Receive(ref shepherd);
                System.Console.WriteLine("Message received from " + shepherd);
                System.Console.WriteLine(Encoding.ASCII.GetString(message));
                System.Console.WriteLine();
                byte[] data = Encoding.ASCII.GetBytes("<Cores>" + Environment.ProcessorCount + "</Cores>");
                m_discoverySocket.Send(data,data.Length,shepherd);
                var server = new TcpListener(IPAddress.Any, 4444);
                server.Start();
                m_comSocket = server.AcceptTcpClient();
                NetworkStream netStream = m_comSocket.GetStream();
                byte[] doIHaveToWork = new byte[24];
                netStream.Read(doIHaveToWork, 0, 24);
                if(Encoding.ASCII.GetString(doIHaveToWork).Split('\n')[0].Equals("You are free"))
                {

                }
                else
                {
                    herdAgent = new HerdAgent();
                    if (herdAgent.ReceiveJobQuery(netStream))
                    {
                        herdAgent.RunJob(netStream);
                        byte[] stopM= Encoding.ASCII.GetBytes("There is no more data");
                        netStream.Write(stopM, 0, stopM.Length);
                        herdAgent.SendJobResult(netStream);
                    }
                }
                netStream.Close();
                netStream.Dispose();
                server.Stop();
                m_comSocket.Close();
                

            }

        }


    }
}
