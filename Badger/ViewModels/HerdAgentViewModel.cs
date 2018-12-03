using System;
using System.Collections.Generic;
using System.Net;
using Caliburn.Micro;
using Herd;

namespace Badger.ViewModels
{
    public class HerdAgentViewModel : PropertyChangedBase
    {
        private HerdAgentInfo m_herdAgentInfo;
        bool m_bLocalAgent = false;

        public HerdAgentViewModel(HerdAgentInfo info)
        {
            m_herdAgentInfo = info;
            IpAddress = m_herdAgentInfo.ipAddress;

            //deselect local agent by default
            m_bLocalAgent = IsLocalIpAddress(IpAddressString);
            IsSelected = !m_bLocalAgent;
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

        private bool m_isSelected = false;
        public bool IsSelected
        {
            get { return m_isSelected; }
            set { m_isSelected = value; NotifyOfPropertyChange(() => IsSelected); }
        }

        //Interfaces to HerdAgentInfo object's properties. This avoids references to Caliburn from the Herd

        private IPEndPoint m_ipAddress;
        public IPEndPoint IpAddress
        {
            get { return m_ipAddress; }
            set
            {
                m_ipAddress = value;
                NotifyOfPropertyChange(() => IpAddress);
                NotifyOfPropertyChange(() => IpAddressString);
            }
        }

        private String m_authenticationCode = "";
        public String AuthenticationCode
        {
            get { return m_authenticationCode; }
            set
            {
                m_authenticationCode = value;
                NotifyOfPropertyChange(() => AuthenticationCode);

            }
        }

        public string IpAddressString { get { return IpAddress.Address.ToString(); } set { } }

        public DateTime lastACK
        {
            get { return m_herdAgentInfo.lastACK; }
            set { m_herdAgentInfo.lastACK = value; }
        }

        public string ProcessorId { get { return m_herdAgentInfo.ProcessorId; } }

        public int NumProcessors { get { if (!m_bLocalAgent) return m_herdAgentInfo.NumProcessors; else return m_herdAgentInfo.NumProcessors - 1; } }

        public string ProcessorArchitecture { get { return m_herdAgentInfo.ProcessorArchitecture; } }

        public string FormatedProcessorInfo
        {
            get
            {
                int cpus = m_herdAgentInfo.NumProcessors;
                string info = m_herdAgentInfo.ProcessorArchitecture + ", ";
                info += cpus + " Core";
                if (cpus > 1) info += 's';
                return info;
            }
        }

        public string ProcessorLoad
        {
            get
            {
                //m_herdAgentInfo.ProcessorLoad is just the number, i.e. 3.723242
                //We want to format it 3.72%
                string processorLoad = m_herdAgentInfo.ProcessorLoad;
                string formattedProcessorLoad;
                int dotPos = processorLoad.LastIndexOf('.');
                if (dotPos > 0)
                    formattedProcessorLoad = processorLoad.Substring(0, Math.Min(processorLoad.Length, dotPos + 3)) + "%";
                else formattedProcessorLoad = processorLoad + "%";
                return formattedProcessorLoad;
            }
        }

        public string Memory
        {
            get { return m_herdAgentInfo.Memory; }
        }

        public bool IsAvailable { get { return m_herdAgentInfo.IsAvailable; } }

        public string Version { get { return m_herdAgentInfo.Version; } }

        public string State
        {
            get { return m_herdAgentInfo.State; }
            set
            {
                m_herdAgentInfo.State = value;
                NotifyOfPropertyChange(() => State);
            }
        }

        public string CUDA
        {
            get
            {
                if (!m_herdAgentInfo.CUDA.Equals(PropValues.None))
                    return m_herdAgentInfo.CUDA;

                return "Not supported";
            }
        }

        public string CUDAColor
        {
            get
            {
                if (!m_herdAgentInfo.CUDA.Equals(PropValues.None))
                    return "Black";

                return "Tomato";
            }
        }

        static public AppVersion BestMatch(List<AppVersion> appVersions, HerdAgentViewModel agent)
        {
            foreach(AppVersion version in appVersions)
            {
                if (version.Requirements.Architecture == agent.ProcessorArchitecture)
                    return version;
            }
            return null;
        }
    }
}
