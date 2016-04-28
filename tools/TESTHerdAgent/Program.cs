using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net;
using System.Net.Sockets;
using System.IO;

namespace TESTHerdAgent
{
    enum AgentState { BUSY, AVAILABLE, DISCOVERED };

    class Program
    {
        const int m_discoveryPortShepherd = 2332;
        const int m_discoveryPortHerd = 2333;
        const int m_comPortShepherd = 2334;
        const int m_comPortHerd= 2335;
        const string m_discoveryMessage = "Slaves, show yourselves!";
        const string m_discoveryAnswer = "At your command, my Master";

        private static AgentState m_state;
        private static UdpClient m_discoverySocket;

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
             
            m_state = AgentState.AVAILABLE;
            m_discoverySocket = new UdpClient(m_discoveryPortHerd);
            IPEndPoint herdClient= new IPEndPoint(0,0);
           
           // m_discoverySocket.BeginReceive(new AsyncCallback(DiscoveryCallback), m_discoverySocket);

            while (true)
            {
                byte[] message = m_discoverySocket.Receive(ref herdClient);
                System.Console.WriteLine("Message received from " + herdClient);
                System.Console.WriteLine(Encoding.ASCII.GetString(message));
                System.Console.WriteLine();

                m_discoverySocket.Send(Encoding.ASCII.GetBytes(m_discoveryAnswer)
                    ,Encoding.ASCII.GetBytes(m_discoveryAnswer).Length,herdClient);
            }
        }


    }
}
