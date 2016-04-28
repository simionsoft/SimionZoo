using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Net;
using System.Net.Sockets;

namespace Shepherd
{
    class Program
    {
        static void Main(string[] args)
        {
            const int m_discoveryPortSepherd = 2332;
            const int m_discoveryPortHerd = 2333;
            const int m_comPortShepherd = 2334;
            const int m_comPortHerd = 2335;
            const string m_discoveryMessage = "Slaves, show yourselves!";
            const string m_discoveryAnswer = "At your command, my Master";

            var Client = new UdpClient(m_discoveryPortSepherd);

            var RequestData = Encoding.ASCII.GetBytes(m_discoveryMessage);
            var ServerEp = new IPEndPoint(IPAddress.Any, m_discoveryPortHerd);

            Client.EnableBroadcast = true;
            Client.Send(RequestData, RequestData.Length, new IPEndPoint(IPAddress.Broadcast, m_discoveryPortHerd));

            var ServerResponseData = Client.Receive(ref ServerEp);
            var ServerResponse = Encoding.ASCII.GetString(ServerResponseData);
            Console.WriteLine("Received {0} from {1}", ServerResponse, ServerEp.Address.ToString());

            Client.Close();
        }
    }
}
