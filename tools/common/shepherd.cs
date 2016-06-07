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
using Caliburn.Micro;

namespace Herd
{
    public class HerdAgentDescription
    {
        private IPAddress m_ipAddress;
        public IPAddress ipAddress{get{return m_ipAddress;} set{m_ipAddress= value;}}
        public string ipAddressString { get { return m_ipAddress.ToString(); } set { } }
        private DateTime m_lastACK;
        public DateTime lastACK { get { return m_lastACK; } set { m_lastACK = value; } }


        public string state { get { return getProperty(Herd.HerdAgent.m_stateXMLTag); } set { } }
        public string version { get { return "(v" +getProperty(Herd.HerdAgent.m_versionXMLTag) +")"; } set { } }
        public string numProcessors { get { return getProperty(Herd.HerdAgent.m_numProcessorsXMLTag) + " cores"; } set { } }

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
        private object m_agentListLock= new object();
        private Dictionary<IPEndPoint,HerdAgentDescription> m_herdAgentList
            = new Dictionary<IPEndPoint,HerdAgentDescription>();
        public Dictionary<IPEndPoint, HerdAgentDescription> herdAgentList { get { return m_herdAgentList; } set { } } 
        UdpClient m_discoverySocket;

        public delegate void NotifyAgentListChanged();
        private NotifyAgentListChanged m_notifyAgentListChanged;
        public void setNotifyAgentListChangedFunc(NotifyAgentListChanged func)
        {
            m_notifyAgentListChanged = func;
        }

        public Shepherd()
        {

            m_discoverySocket = new UdpClient();
            m_discoverySocket.EnableBroadcast = true;
            m_notifyAgentListChanged = null;
        }


        public void DiscoveryCallback(IAsyncResult ar)
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
                //we copy the ip address into the properties
                herdAgentDescription.ipAddress = ip.Address;
                //we update the ack time
                herdAgentDescription.lastACK = System.DateTime.Now;

                lock (m_agentListLock)
                {
                    if (!m_herdAgentList.ContainsKey(ip))
                        m_herdAgentList.Add(ip, herdAgentDescription);
                    else
                        m_herdAgentList[ip] = herdAgentDescription;
                }
                //notify, if we have to, that the agent list has probably changed
                if (m_notifyAgentListChanged != null)
                    m_notifyAgentListChanged();

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
            m_xmlStream.resizeBuffer(m_tcpClient.ReceiveBufferSize);
            m_netStream= m_tcpClient.GetStream();
            //send slave acquire message
            m_xmlStream.writeMessage(m_netStream, CJobDispatcher.m_acquireMessage, true);
        }
        public void disconnect()
        {
            m_tcpClient.Close();
            m_tcpClient = null;
        }
        public void getHerdAgentList(ref BindableCollection <HerdAgentDescription> outHerdAgentList, int timeoutSeconds)
        {
            outHerdAgentList.Clear();
            lock (m_agentListLock)
            {
                foreach (var agent in m_herdAgentList)
                    if (System.DateTime.Now.Subtract(agent.Value.lastACK).TotalSeconds < timeoutSeconds)
                        outHerdAgentList.Add(agent.Value);
            }
        }
        public void getHerdAgentList(ref Dictionary<IPEndPoint, HerdAgentDescription> outDictionary)
        {
            outDictionary.Clear();
            lock (m_agentListLock)
            {
                foreach (var agent in m_herdAgentList)
                    outDictionary.Add(agent.Key, agent.Value);
            }
        }
        public int getAvailableHerdAgentListAndCores(ref Dictionary<IPEndPoint, int> outHerdAgentList)
        {
            int numCores;
            int numCoresTotal = 0;
            outHerdAgentList.Clear();
            lock (m_agentListLock)
            {
                foreach (var agent in m_herdAgentList)
                {
                    string state = agent.Value.getProperty("State");
                    string numCoresString = agent.Value.getProperty("NumberOfProcessors");
                    if (state == "available" && numCoresString != "n/a")
                    {
                        try
                        {
                            numCores = Int32.Parse(numCoresString);
                            numCoresTotal += numCores-1;
                            outHerdAgentList.Add(agent.Key, numCores-1);
                        }
                        catch (Exception)
                        {
                            //we ignore possible errors parsing the string
                        }
                    }
                }
                return numCoresTotal;
            }
        }        

        public void SendJobQuery(CJob job)
        {
            m_job = job;
            SendJobHeader();
            SendTasks();
            //SendExeFiles(true);
            SendInputFiles(true);
            SendOutputFiles(false);
            SendJobFooter();
        }
        public bool ReceiveJobResult()
        {
            bool bFooterPeeked= false;
            string xmlTag = "";
            m_job.tasks.Clear();
            m_job.inputFiles.Clear();
            m_job.outputFiles.Clear();

            ReceiveJobHeader();

            do
            {
                ReadFromStream();
                xmlTag= m_xmlStream.peekNextXMLTag();
                switch(xmlTag)
                {
                    case "Task": ReceiveTask(); break;
                    case "Input": ReceiveFile(FileType.INPUT, false, false); break;
                    case "Output": ReceiveFile(FileType.OUTPUT, true, false); break;
                    case "/Job": bFooterPeeked= true; break;
                }
            } while (!bFooterPeeked);

            //ReceiveExeFiles(false, false);
            //ReceiveInputFiles(false, false);
            //ReceiveOutputFiles(true, false);
            ReceiveJobFooter();

            return true;//if job result properly received. For now, we will assume it}
        }
    }
}