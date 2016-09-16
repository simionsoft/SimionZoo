using System;
using System.Net;
using Caliburn.Micro;
using Herd;

namespace AppXML.ViewModels
{
    public class HerdAgentViewModel : PropertyChangedBase
    {
        private HerdAgentInfo m_herdAgentInfo;


        public HerdAgentViewModel(HerdAgentInfo info)
        {
            m_herdAgentInfo = info;
        }

        //Interfaces to HerdAgentInfo object's properties. This avoids references to Caliburn from the Herd
        public IPEndPoint ipAddress { get { return m_herdAgentInfo.ipAddress; }
            set { m_herdAgentInfo.ipAddress = value; } }
        public string ipAddressString { get { return m_herdAgentInfo.ipAddressString; } set { } }
        public DateTime lastACK { get { return m_herdAgentInfo.lastACK; }
            set { m_herdAgentInfo.lastACK = value; } }
        public int numProcessors { get { return m_herdAgentInfo.numProcessors; } }
        public bool isAvailable { get { return m_herdAgentInfo.isAvailable; } }
        public string version { get { return m_herdAgentInfo.version; } }
        public string status {
            get { return m_herdAgentInfo.state; }
            set { m_herdAgentInfo.state = value; NotifyOfPropertyChange(() => status); }
        }
    }
}
