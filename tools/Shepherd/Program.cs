using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net;
using System.Net.Sockets;
using System.IO;
using NetJobTransfer;
using AppXML.Data;

namespace TESTShepherd
{
    enum AgentState { BUSY, AVAILABLE, DISCOVERED };
    enum FileType { EXE, INPUT, OUTPUT};

    class Program
    {

        private static AgentState m_state;
        private static UdpClient m_discoverySocket;

        public static void method(string filename)
        {

            Shepherd shepherd = new Shepherd();
            m_discoverySocket = new UdpClient();//(m_discoveryPortShepherd);

            var TcpSocket = new TcpListener(IPAddress.Any, CJobDispatcher.m_comPortShepherd);
            TcpSocket.Start();

            var RequestData = Encoding.ASCII.GetBytes(CJobDispatcher.m_discoveryMessage);
            var ServerEp = new IPEndPoint(IPAddress.Any, CJobDispatcher.m_discoveryPortHerd);

            m_discoverySocket.EnableBroadcast = true;
            System.Threading.Thread.Sleep(1000); //so that the shepherd waits for the herd agent to be ready
            m_discoverySocket.Send(RequestData, RequestData.Length, new IPEndPoint(IPAddress.Broadcast, CJobDispatcher.m_discoveryPortHerd));
            IPEndPoint xxx = new IPEndPoint(0, CJobDispatcher.m_discoveryPortHerd);
            string tmp = Encoding.ASCII.GetString(m_discoverySocket.Receive(ref xxx));

            // to do: leer las conexiones que se reciben y ordenarlas en base al nuemro de cores disponibles
            // en la linea de comandos hay que añadir el nombre del pipe y puentearlo
            using (TcpClient comSocket = TcpSocket.AcceptTcpClient())
            {
                using (NetworkStream netStream = comSocket.GetStream())
                {
                    CJob job = new CJob();
                    job.name = filename;
                    job.exeFile = "..\\Debug\\RLSimion.exe";
                    job.comLineArgs.Add(filename);
                    job.inputFiles.Add(filename);
                    List<string> inputs = Utility.getInputs(filename);
                    if(inputs==null)
                        return;
                    foreach(string input in inputs)
                    {
                        job.inputFiles.Add(input);
                    }
                    // job.inputFiles.Add("..\\Debug\\msvcp120d.dll");
                    shepherd.SendJobQuery(netStream, job);
                    shepherd.ReceiveJobResult(netStream);
                }
                comSocket.Close();
            }
            TcpSocket.Stop();
            //var ServerResponseData = Client.Receive(ref ServerEp);
            //var ServerResponse = Encoding.ASCII.GetString(ServerResponseData);
            //Console.WriteLine("Received {0} from {1}", ServerResponse, ServerEp.Address.ToString());

            m_discoverySocket.Close();
        }

        static void Main(string[] args)
        {
            method("..\\experiments\\examples\\uv-pid");
        }   
    }
}
