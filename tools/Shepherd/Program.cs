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
            const int m_discoveryPort = 2332;
            const int m_comPort = 2333;

            var Client = new UdpClient();
            var RequestData = Encoding.ASCII.GetBytes("ShowYourselves");
            var ServerEp = new IPEndPoint(IPAddress.Any, 0);

            Client.EnableBroadcast = true;
            Client.Send(RequestData, RequestData.Length, new IPEndPoint(IPAddress.Broadcast, m_discoveryPort));

            var ServerResponseData = Client.Receive(ref ServerEp);
            var ServerResponse = Encoding.ASCII.GetString(ServerResponseData);
            Console.WriteLine("Recived {0} from {1}", ServerResponse, ServerEp.Address.ToString());

            Client.Close();
        }
    }
}
