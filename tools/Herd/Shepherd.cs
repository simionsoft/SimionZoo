using System;
using System.Collections.Generic;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Xml.Linq;


namespace Herd
{
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
        private object m_listLock = new object();
        private Dictionary<IPEndPoint, HerdAgentInfo> m_herdAgentList
            = new Dictionary<IPEndPoint, HerdAgentInfo>();

        UdpClient m_discoverySocket;

        public delegate void NotifyAgentListChanged();
        private NotifyAgentListChanged m_notifyAgentListChanged;

        private DateTime m_lastHerdAgentListUpdate = DateTime.Now;
        private double m_herdAgentListUpdateTime = 1.0; //update freq. of the herd-agent list (seconds)


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


        public static bool IsLocalIpAddress(string host)
        {
            try
            { // get host IP addresses
                IPAddress[] hostIPs = Dns.GetHostAddresses(host);
                // get local IP addresses
                IPAddress[] localIPs = Dns.GetHostAddresses(Dns.GetHostName());

                // test if any host IP equals to any local IP or to localhost
                foreach (IPAddress hostIP in hostIPs)
                {
                    // is localhost
                    if (IPAddress.IsLoopback(hostIP)) return true;
                    // is local address
                    foreach (IPAddress localIP in localIPs)
                    {
                        if (hostIP.Equals(localIP)) return true;
                    }
                }
            }
            catch { }
            return false;
        }


        public void DiscoveryCallback(IAsyncResult ar)
        {
            UdpClient u = (UdpClient)((ShepherdUdpState)(ar.AsyncState)).client;
            IPEndPoint ip = (IPEndPoint)((ShepherdUdpState)(ar.AsyncState)).ip;

            XMLStream inputXMLStream = new XMLStream();
            //IPEndPoint ip= new IPEndPoint();
            XElement xmlDescription;
            string herdAgentXMLDescription;
            try
            {
                Byte[] receiveBytes = u.EndReceive(ar, ref ip);
                //if (!IsLocalIpAddress(ip.Address.ToString()))
                {
                    herdAgentXMLDescription = Encoding.ASCII.GetString(receiveBytes);
                    xmlDescription = XElement.Parse(herdAgentXMLDescription);
                    HerdAgentInfo herdAgentInfo = new HerdAgentInfo();
                    herdAgentInfo.Parse(xmlDescription);
                    //we copy the ip address into the properties
                    herdAgentInfo.ipAddress = ip;
                    //we update the ack time
                    DateTime now = DateTime.Now;
                    herdAgentInfo.lastACK = now;

                    lock (m_listLock)
                    {
                        m_herdAgentList[ip] = herdAgentInfo;
                    }
                    //check how much time ago the agent list was updated
                    double lastUpdateElapsedTime = (now - m_lastHerdAgentListUpdate).TotalSeconds;
                    //notify, if we have to, that the agent list has probably changed
                    if (lastUpdateElapsedTime > m_herdAgentListUpdateTime)
                    {
                        m_lastHerdAgentListUpdate = now;

                        if (m_notifyAgentListChanged != null)
                            m_notifyAgentListChanged();
                    }
                }

                u.BeginReceive(new AsyncCallback(DiscoveryCallback), ar.AsyncState);
            }
            catch (TaskCanceledException ex)
            {
                logMessage("Task canceled exception in Shepherd");
                logMessage(ex.ToString());
            }
            catch (Exception ex)
            {
                logMessage("Exception in discovery callback function");
                logMessage(ex.StackTrace);
            }
        }


        public void sendBroadcastHerdAgentQuery()
        {
            var RequestData = Encoding.ASCII.GetBytes(m_discoveryMessage);
            m_discoverySocket.Send(RequestData, RequestData.Length,
                new IPEndPoint(IPAddress.Broadcast, m_discoveryPortHerd));
        }


        public void beginListeningHerdAgentQueryResponses()
        {
            ShepherdUdpState u = new ShepherdUdpState();
            IPEndPoint xxx = new IPEndPoint(0, CJobDispatcher.m_discoveryPortHerd);
            u.ip = xxx;
            u.client = m_discoverySocket;
            m_discoverySocket.BeginReceive(DiscoveryCallback, u);
        }


        public bool connectToHerdAgent(IPEndPoint endPoint)
        {
            try
            {
                m_tcpClient = new TcpClient();
                m_tcpClient.Connect(endPoint.Address, Herd.CJobDispatcher.m_comPortHerd);
                m_xmlStream.resizeBuffer(m_tcpClient.ReceiveBufferSize);
                m_netStream = m_tcpClient.GetStream();
                //send slave acquire message
                m_xmlStream.writeMessage(m_netStream, CJobDispatcher.m_acquireMessage, true);
            }
            catch
            {
                return false;
            }
            return true;
        }


        public void disconnect()
        {
            if (m_netStream != null) m_netStream.Dispose();
            if (m_tcpClient != null) m_tcpClient.Close();
            m_tcpClient = null;
        }


        public void getHerdAgentList(ref List<HerdAgentInfo> outHerdAgentList, int timeoutSeconds = 10)
        {
            lock (m_listLock)
            {
                outHerdAgentList.Clear();

                foreach (var agent in m_herdAgentList)
                    if (System.DateTime.Now.Subtract(agent.Value.lastACK).TotalSeconds < timeoutSeconds)
                        outHerdAgentList.Add(agent.Value);
            }
        }


        public void getHerdAgentList(ref Dictionary<IPEndPoint, HerdAgentInfo> outDictionary)
        {
            lock (m_listLock)
            {
                outDictionary.Clear();

                foreach (var agent in m_herdAgentList)
                    outDictionary.Add(agent.Key, agent.Value);
            }
        }


        public int getAvailableHerdAgentListAndCores(ref Dictionary<IPEndPoint, int> outHerdAgentList)
        {
            int numCoresTotal = 0;
            lock (m_listLock)
            {
                outHerdAgentList.Clear();

                foreach (var agent in m_herdAgentList)
                {
                    string state = agent.Value.getProperty("State");
                    string numCoresString = agent.Value.getProperty("NumberOfProcessors");
                    if (state == "available" && numCoresString != "n/a")
                    {
                        try
                        {
                            var numCores = Int32.Parse(numCoresString);
                            numCoresTotal += numCores - 1;
                            outHerdAgentList.Add(agent.Key, numCores - 1);
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

        public void SendJobQuery(CJob job, CancellationToken cancelToken)
        {
            m_job = job;
            SendJobHeader(cancelToken);
            SendTasks(cancelToken);
            //SendExeFiles(true);
            SendInputFiles(true, cancelToken);
            SendOutputFiles(false, cancelToken);
            SendJobFooter(cancelToken);
        }
        public async Task<bool> ReceiveJobResult(CancellationToken cancelToken)
        {
            bool bFooterPeeked = false;
            string xmlTag = "";
            m_job.tasks.Clear();
            m_job.inputFiles.Clear();
            m_job.outputFiles.Clear();

            int ret = await ReceiveJobHeader(cancelToken);
            bool bret;
            do
            {
                xmlTag = m_xmlStream.peekNextXMLTag();
                while (xmlTag == "")
                {
                    ret = await ReadFromStreamAsync(cancelToken);
                    xmlTag = m_xmlStream.peekNextXMLTag();
                }

                switch (xmlTag)
                {
                    case "Task": bret = await ReceiveTask(cancelToken); break;
                    case "Input": bret = await ReceiveFile(FileType.INPUT, false, false, cancelToken); break;
                    case "Output": bret = await ReceiveFile(FileType.OUTPUT, true, false, cancelToken); break;
                    case "/Job": bFooterPeeked = true; break;
                }
            } while (!bFooterPeeked);

            bret = await ReceiveJobFooter(cancelToken);

            //if job result properly received. For now, we will assume it
            return true;
        }
    }
}
