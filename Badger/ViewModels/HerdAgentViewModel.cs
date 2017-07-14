using System;
using System.Globalization;
using System.Net;
using Caliburn.Micro;
using Herd;

namespace Badger.ViewModels
{
    public class HerdAgentViewModel : PropertyChangedBase
    {
        private HerdAgentInfo m_herdAgentInfo;


        public HerdAgentViewModel(HerdAgentInfo info)
        {
            m_herdAgentInfo = info;
            ProcessorLoad = m_herdAgentInfo.ProcessorLoad.ToString("0.") + "%";
            State = m_herdAgentInfo.State;
            IpAddress = m_herdAgentInfo.ipAddress;
            IsSelected = true;
        }

        public bool IsSelected { get; set; }

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

        public string IpAddressString { get { return IpAddress.Address.ToString(); } set { } }

        public DateTime lastACK
        {
            get { return m_herdAgentInfo.lastACK; }
            set { m_herdAgentInfo.lastACK = value; }
        }

        public string ProcessorId { get { return m_herdAgentInfo.ProcessorId; } }

        public int NumProcessors { get { return m_herdAgentInfo.NumProcessors; } }

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

        private string m_processorLoad;

        public string ProcessorLoad
        {
            get { return m_processorLoad; }
            set
            {
                m_processorLoad = value;
                NotifyOfPropertyChange(() => ProcessorLoad);
            }
        }

        public string Memory
        {
            get
            {
                double totalMem = m_herdAgentInfo.Memory / 1024 / 1024;

                if (totalMem >= 1024)
                    return (totalMem / 1024).ToString("0.0") + " GB";

                return totalMem.ToString("0.") + " MB";
            }
        }

        public bool IsAvailable { get { return m_herdAgentInfo.IsAvailable; } }

        public string Version { get { return m_herdAgentInfo.Version; } }

        private string m_state;

        public string State
        {
            get { return m_state; }
            set
            {
                m_state = value;
                NotifyOfPropertyChange(() => State);
            }
        }

        public string CudaInfo
        {
            get
            {
                if (!m_herdAgentInfo.CudaInfo.Equals(HerdAgentInfo.NoneProperty))
                    return m_herdAgentInfo.CudaInfo;

                return "Not supported";
            }
        }

        public string CudaInfoColor
        {
            get
            {
                if (!m_herdAgentInfo.CudaInfo.Equals(HerdAgentInfo.NoneProperty))
                    return "Black";

                return "Tomato";
            }
        }
    }
}
