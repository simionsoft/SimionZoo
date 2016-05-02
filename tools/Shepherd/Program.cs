using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net;
using System.Net.Sockets;
using System.IO;
using NetJobTransfer;

namespace TESTShepherd
{
    enum AgentState { BUSY, AVAILABLE, DISCOVERED };
    enum FileType { EXE, INPUT, OUTPUT};

    class Program
    {

        private static AgentState m_state;
        private static UdpClient m_discoverySocket;


        
        static void Main(string[] args)
        {
            Shepherd shepherd= new Shepherd();
            m_discoverySocket = new UdpClient();//(m_discoveryPortShepherd);

            var TcpSocket = new TcpListener(IPAddress.Any, CJobDispatcher.m_comPortShepherd);
            TcpSocket.Start();

            var RequestData = Encoding.ASCII.GetBytes(CJobDispatcher.m_discoveryMessage);
            var ServerEp = new IPEndPoint(IPAddress.Any, CJobDispatcher.m_discoveryPortHerd);

            m_discoverySocket.EnableBroadcast = true;
            System.Threading.Thread.Sleep(1000); //so that the shepherd waits for the herd agent to be ready
            m_discoverySocket.Send(RequestData, RequestData.Length, new IPEndPoint(IPAddress.Broadcast, CJobDispatcher.m_discoveryPortHerd));

            // to do: leer las conexiones que se reciben y ordenarlas en base al nuemro de cores disponibles
            using (TcpClient comSocket= TcpSocket.AcceptTcpClient())
            {
                using (NetworkStream netStream = comSocket.GetStream())
                {
                    CJob job= new CJob();
                    job.name = "test-job";
                    job.exeFile = "..\\Debug\\RLSimion.exe";
                    job.comLineArgs = "..\\experiments\\examples\\uv-pid.node";
                    job.inputFiles.Add(job.comLineArgs);
                   // job.inputFiles.Add("..\\Debug\\msvcp120d.dll");
                    job.inputFiles.Add("..\\config\\world\\underwater-vehicle.xml");
                    job.inputFiles.Add("..\\config\\world\\underwater-vehicle\\setpoint.txt");
                    job.inputFiles.Add("..\\data\\vidal-80.d_T_g.weights");
                    job.outputFiles.Add("..\\experiments\\examples\\log-eval-epis-1.txt");
                    job.outputFiles.Add("..\\experiments\\examples\\log-eval-epis-2.txt");
                    job.outputFiles.Add("..\\experiments\\examples\\log-eval-exp.txt");
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
    }
}
