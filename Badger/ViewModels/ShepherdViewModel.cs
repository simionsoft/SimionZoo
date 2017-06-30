using System.Collections.Generic;
using System.Linq;
using System.Net;
using Herd;
using Caliburn.Micro;


namespace Badger.ViewModels
{
    public class ShepherdViewModel : PropertyChangedBase
    {
        // const int m_agentTimeoutSeconds = 10;
        const int m_updateTimeSeconds = 3;
        System.Timers.Timer m_timer;

        private Shepherd m_shepherd;
        public Shepherd shepherd { get { return m_shepherd; } set { } }

        private object m_listsLock = new object();
        private List<HerdAgentInfo> m_innerHerdAgentList =
            new List<HerdAgentInfo>();

        private List<HerdAgentInfo> orderedHerdAgentList;

        private BindableCollection<HerdAgentViewModel> m_herdAgentList
            = new BindableCollection<HerdAgentViewModel>();
        public BindableCollection<HerdAgentViewModel> herdAgentList
        {
            get
            {
                lock (m_listsLock)
                {
                    m_shepherd.getHerdAgentList(ref m_innerHerdAgentList);

                    // Test data
                    m_innerHerdAgentList.Add(new HerdAgentInfo { ipAddress = new IPEndPoint(new IPAddress(0x0811026f), 3128) });
                    m_innerHerdAgentList.Add(new HerdAgentInfo { ipAddress = new IPEndPoint(new IPAddress(0x2414188f), 3128) });
                    m_innerHerdAgentList.Add(new HerdAgentInfo { ipAddress = new IPEndPoint(new IPAddress(0x1914188f), 3128) });

                    // Ordering the inner list by number of processor 
                    orderedHerdAgentList = m_innerHerdAgentList.OrderByDescending(o => o.NumProcessors).ToList();

                    // m_herdAgentList.Clear();
                    int len = m_herdAgentList.Count;

                    foreach (HerdAgentInfo agent in orderedHerdAgentList)
                    {
                        bool found = false;
                        int index = 0;

                        while (!found && index < len)
                        {
                            if (Equals(agent.ipAddress, m_herdAgentList[index].IpAddress))
                            {
                                m_herdAgentList[index].ProcessorLoad = agent.ProcessorLoad.ToString("0.") + "%"; ;
                                found = true;
                            }

                            index++;;
                        }

                        if (!found)
                            m_herdAgentList.Add(new HerdAgentViewModel(agent));
                    }
                }

                return m_herdAgentList;
            }
            set { }
        }

        public int getAvailableHerdAgents(ref List<HerdAgentViewModel> outList)
        {
            //we assume outList needs no synchronization
            int numAvailableCores = 0;
            lock (m_listsLock)
            {
                outList.Clear();
                foreach (HerdAgentViewModel agent in m_herdAgentList)
                {
                    if (agent.IsAvailable && agent.IsSelected)
                    {
                        outList.Add(agent);
                        numAvailableCores += agent.NumProcessors;
                    }
                }
            }
            return numAvailableCores;
        }

        private void notifyHerdAgentChanged()
        {
            NotifyOfPropertyChange(() => herdAgentList);
        }
        private void resendBroadcast(object sender, System.Timers.ElapsedEventArgs e)
        {
            m_shepherd.sendBroadcastHerdAgentQuery();
        }

        public ShepherdViewModel()
        {
            m_shepherd = new Shepherd();
            m_shepherd.setNotifyAgentListChangedFunc(notifyHerdAgentChanged);

            m_timer = new System.Timers.Timer(m_updateTimeSeconds * 1000);
            m_shepherd.sendBroadcastHerdAgentQuery();
            m_shepherd.beginListeningHerdAgentQueryResponses();

            m_timer.AutoReset = true;
            m_timer.Elapsed += new System.Timers.ElapsedEventHandler(resendBroadcast);
            m_timer.Start();
        }



    }
}
