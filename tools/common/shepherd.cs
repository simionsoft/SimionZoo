using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.IO.Pipes;
using System.Linq;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Text.RegularExpressions;
using System.Threading;
using System.Threading.Tasks;
using System.Xml;
using System.Xml.Linq;
using Herd;

namespace Herd
{
    public class HerdAgentDescription
    {
        private Dictionary<string,string> m_properties;
        public HerdAgentDescription()
        {
            m_properties = new Dictionary<string, string>();
        }
        public void addProperty(string name,string value)
        {
            m_properties.Add(name, value);
        }
        public string getProperty(string name)
        {
            if (m_properties.ContainsKey(name))
                return m_properties[name];
            else return "n/a";
        }
        public void parse(XElement xmlDescription)
        {
            if (xmlDescription.Name.ToString()=="HerdAgent")
            {
                m_properties.Clear();
                foreach (XElement child in xmlDescription.Elements())
                {
                    addProperty(child.Name.ToString(),child.Value);
                }
            }
        }
        public override string ToString()
        {
            string res = "";
            foreach(var property in m_properties)
            {
                res += property.Key + "=\"" + property.Value + "\";";
            }
            return res;
        }
    }

    public class ShepherdUdpState
    {
        public UdpClient client { get; set; }
        public IPEndPoint ip { get; set; }
    }
    public class ShepherdTCPState
    {
        public IPEndPoint ip { get; set; }
    }

    public class Shepherd : CJobDispatcher
    {
        private static Dictionary<IPEndPoint,HerdAgentDescription> m_herdAgentList
            = new Dictionary<IPEndPoint,HerdAgentDescription>();
        UdpClient m_discoverySocket;
        

        public Shepherd()
        {

            //m_xmlStream.resizeBuffer(tcpClient.SendBufferSize);

            m_discoverySocket = new UdpClient();
            m_discoverySocket.EnableBroadcast = true;
        }


        public static void DiscoveryCallback(IAsyncResult ar)
        {
            XMLStream inputXMLStream = new XMLStream();
            UdpClient u = (UdpClient)((ShepherdUdpState)(ar.AsyncState)).client;
            IPEndPoint ip = (IPEndPoint)((ShepherdUdpState)(ar.AsyncState)).ip;
            //IPEndPoint ip= new IPEndPoint();
            XElement xmlDescription;
            string herdAgentXMLDescription;
            try
            {
                Byte[] receiveBytes = u.EndReceive(ar, ref ip);
                herdAgentXMLDescription = Encoding.ASCII.GetString(receiveBytes);
                xmlDescription = XElement.Parse(herdAgentXMLDescription);
                HerdAgentDescription herdAgentDescription = new HerdAgentDescription();
                herdAgentDescription.parse(xmlDescription);
                if (!m_herdAgentList.ContainsKey(ip))
                    m_herdAgentList.Add(ip, herdAgentDescription);
                else
                    m_herdAgentList[ip] = herdAgentDescription;

                u.BeginReceive(new AsyncCallback(DiscoveryCallback), ar.AsyncState);
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.StackTrace);
            }
        }
        public void sendBroadcastHerdAgentQuery()
        {
            var RequestData = Encoding.ASCII.GetBytes(CJobDispatcher.m_discoveryMessage);

            m_discoverySocket.Send(RequestData, RequestData.Length
                , new IPEndPoint(IPAddress.Broadcast, CJobDispatcher.m_discoveryPortHerd));

        }
        public void beginListeningHerdAgentQueryResponses()
        {
            ShepherdUdpState u = new ShepherdUdpState();
            IPEndPoint xxx = new IPEndPoint(0, CJobDispatcher.m_discoveryPortHerd);
            u.ip = xxx;
            u.client = m_discoverySocket;
            m_discoverySocket.BeginReceive(DiscoveryCallback, u);
        }
        public void connectToHerdAgent(IPEndPoint endPoint)
        {
            m_tcpClient = new TcpClient();
            m_tcpClient.Connect(endPoint.Address, Herd.CJobDispatcher.m_comPortHerd);
            m_netStream= m_tcpClient.GetStream();
            //send slave acquire message
            m_xmlStream.writeMessage(m_netStream, CJobDispatcher.m_acquireMessage, true);
        }
        public Dictionary<IPEndPoint, HerdAgentDescription> getHerdAgentList()
        {
            return m_herdAgentList;
        }
        public int getAvailableHerdAgentListAndCores(ref Dictionary<IPEndPoint,int> outHerdAgentList)
        {
            int numCores;
            int numCoresTotal= 0;
            outHerdAgentList.Clear();
            foreach(var agent in m_herdAgentList)
            {
                string numCoresString = agent.Value.getProperty("NumberOfProcessors");
                if (numCoresString!="n/a")
                {
                    try
                    {
                        numCores = Int32.Parse(numCoresString);
                        numCoresTotal += numCores;
                        outHerdAgentList.Add(agent.Key, numCores);
                    }
                    catch (Exception)
                    {
                        //we ignore possible errors parsing the string
                    }
                }
            }
            return numCoresTotal;
        }
        

        public void SendJobQuery(CJob job)
        {
            m_job = job;
            SendJobHeader();
            SendExeFiles(true);
            SendInputFiles(true);
            SendOutputFiles(false);
            SendJobFooter();
        }
        public bool ReceiveJobResult()
        {
            m_job.comLineArgs.Clear();
            m_job.inputFiles.Clear();
            m_job.outputFiles.Clear();

            ReceiveJobHeader();
            ReceiveExeFiles(false, false);
            ReceiveInputFiles(false, false);
            ReceiveOutputFiles(true, false);
            ReceiveJobFooter();

            return true;//if job result properly received. For now, we will assume it}
        }
    }
}