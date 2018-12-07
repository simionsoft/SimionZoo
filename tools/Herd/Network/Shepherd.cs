using System;
using System.Collections.Generic;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Xml.Linq;
using System.Net.NetworkInformation;
using System.Linq;

namespace Herd.Network
{
    public class ShepherdUdpState
    {
        public UdpClient Client { get; set; }
        public IPEndPoint Ip { get; set; }
    }


    public class ShepherdTCPState
    {
        public IPEndPoint Ip { get; set; }
    }


    public class Shepherd : JobTransmitter
    {
        private object m_listLock = new object();
        private Dictionary<IPEndPoint, HerdAgentInfo> m_herdAgentList
            = new Dictionary<IPEndPoint, HerdAgentInfo>();

        public delegate void NotifyAgentListChanged(HerdAgentInfo newAgent);
        private NotifyAgentListChanged m_notifyAgentListChanged;

        private DateTime m_lastHerdAgentListUpdate = DateTime.Now;
        private double m_herdAgentListUpdateTime = 1.0; //update freq. of the herd-agent list (seconds)


        public void SetOnHerdAgentDiscoveryFunc(NotifyAgentListChanged func)
        {
            m_notifyAgentListChanged = func;
        }


        public Shepherd()
        {
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


        void DiscoveryCallback(IAsyncResult ar)
        {
            UdpClient udpClient = (UdpClient)((ShepherdUdpState)(ar.AsyncState)).Client;
            IPEndPoint ip = (IPEndPoint)((ShepherdUdpState)(ar.AsyncState)).Ip;

            XMLStream inputXMLStream = new XMLStream();
            
            XElement xmlDescription;
            string herdAgentXMLDescription;
            try
            {
                Byte[] receiveBytes = udpClient.EndReceive(ar, ref ip);
                {
                    herdAgentXMLDescription = Encoding.ASCII.GetString(receiveBytes);
                    if (herdAgentXMLDescription.IndexOf('<') == 0)
                    {
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
                        }
                        m_notifyAgentListChanged?.Invoke(herdAgentInfo);
                    }
                }

                udpClient.BeginReceive(new AsyncCallback(DiscoveryCallback), ar.AsyncState);
            }
            catch (TaskCanceledException ex)
            {
                LogMessage("Task canceled exception in Shepherd");
                LogMessage(ex.ToString());
            }
            catch (Exception ex)
            {
                LogMessage("Exception in discovery callback function");
                LogMessage(ex.StackTrace);
            }
        }

        List<UdpClient> m_broadcastInterfacesUdpClients = new List<UdpClient>();

        void FindBroadcastInterfaces()
        {
            m_broadcastInterfacesUdpClients.Clear();

            //create an udpClient for each of the interfaces to send the broadcast message through all
            foreach (NetworkInterface ni in NetworkInterface.GetAllNetworkInterfaces())
            {
                if (ni.OperationalStatus == OperationalStatus.Up && ni.SupportsMulticast && ni.GetIPProperties().GetIPv4Properties() != null
                    && ni.NetworkInterfaceType == NetworkInterfaceType.Ethernet)
                {
                    int id = ni.GetIPProperties().GetIPv4Properties().Index;
                    if (NetworkInterface.LoopbackInterfaceIndex != id)
                    {
                        foreach (UnicastIPAddressInformation uip in ni.GetIPProperties().UnicastAddresses)
                        {
                            if (uip.Address.AddressFamily == AddressFamily.InterNetwork)
                            {
                                IPEndPoint local = new IPEndPoint(uip.Address, m_discoveryPortHerd);
                                UdpClient udpClient = new UdpClient(local);
                                udpClient.Client.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.Broadcast, 1);
                                udpClient.Client.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.DontRoute, 1);
                                m_broadcastInterfacesUdpClients.Add(udpClient);
                            }
                        }
                    }
                }
            }
        }

        public void CallHerd()
        {
            byte[] DiscoveryMessage = Encoding.ASCII.GetBytes(m_discoveryMessage);
            try
            {
                if (m_broadcastInterfacesUdpClients.Count == 0) FindBroadcastInterfaces();
                
                IPEndPoint target = new IPEndPoint(IPAddress.Broadcast, m_discoveryPortHerd);
                foreach (UdpClient client in m_broadcastInterfacesUdpClients)
                {
                    //send broadcast to interface
                    client.Send(DiscoveryMessage, DiscoveryMessage.Length, target);
                    //listen to herd agent responses
                    ShepherdUdpState udpState = new ShepherdUdpState();
                    IPEndPoint localIP = new IPEndPoint(0, m_discoveryPortHerd);
                    udpState.Ip = localIP;
                    udpState.Client = client;
                    client.BeginReceive(DiscoveryCallback, udpState);
                }
            }
            catch (Exception ex)
            {
                LogMessage("Unhandled error in SendBroadcastHerdAgentQuery: " + ex.ToString());
            }

        }


        public bool ConnectToHerdAgent(IPEndPoint endPoint)
        {
            try
            {
                m_tcpClient = new TcpClient();
                m_tcpClient.Connect(endPoint.Address, m_comPortHerd);
                m_xmlStream.resizeBuffer(m_tcpClient.ReceiveBufferSize);
                m_netStream = m_tcpClient.GetStream();
                //send slave acquire message
                m_xmlStream.WriteMessage(m_netStream, m_acquireMessage, true);
            }
            catch
            {
                return false;
            }
            return true;
        }


        public void Disconnect()
        {
            if (m_netStream != null) m_netStream.Dispose();
            if (m_tcpClient != null) m_tcpClient.Close();
            m_tcpClient = null;
        }


        public void GetHerdAgentList(ref List<HerdAgentInfo> outHerdAgentList, int timeoutSeconds = 10)
        {
            lock (m_listLock)
            {
                outHerdAgentList.Clear();

                foreach (var agent in m_herdAgentList)
                    if (DateTime.Now.Subtract(agent.Value.lastACK).TotalSeconds < timeoutSeconds)
                        outHerdAgentList.Add(agent.Value);
            }
        }


        public void GetHerdAgentList(ref Dictionary<IPEndPoint, HerdAgentInfo> outDictionary)
        {
            lock (m_listLock)
            {
                outDictionary.Clear();

                foreach (var agent in m_herdAgentList)
                    outDictionary.Add(agent.Key, agent.Value);
            }
        }

        public void SendJobQuery(Job job, CancellationToken cancelToken)
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
            m_job.Tasks.Clear();
            m_job.InputFiles.Clear();
            m_job.OutputFiles.Clear();

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
