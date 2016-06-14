using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Herd;
using Caliburn.Micro;
using System.Net;

namespace AppXML.ViewModels
{
    public class ShepherdViewModel : PropertyChangedBase
    {
        const int m_agentTimeoutSeconds = 10;
        const int m_updateTimeSeconds = 5;
        System.Timers.Timer m_timer;

        private Shepherd m_shepherd;
        public Shepherd shepherd { get { return m_shepherd; } set{}}

        private object m_herdAgentListLock = new object();
        private BindableCollection<HerdAgentViewModel> m_innerHerdAgentList =
            new BindableCollection<HerdAgentViewModel>();
        private BindableCollection <HerdAgentViewModel> m_herdAgentList
            = new BindableCollection<HerdAgentViewModel>();
        public BindableCollection<HerdAgentViewModel> herdAgentList
        {
            get
            { 
                lock(m_herdAgentListLock)
                {
                    m_herdAgentList.Clear();
                    foreach (HerdAgentViewModel agent in m_innerHerdAgentList)
                        m_herdAgentList.Add(agent);
                    return m_herdAgentList;
                }
            }
            set {}
        }
        public int getAvailableHerdAgents(ref List<HerdAgentViewModel> outList)
        {
            int numAvailableCores = 0;
            lock (m_herdAgentListLock)
            {
                outList.Clear();
                foreach (HerdAgentViewModel agent in m_innerHerdAgentList)
                {
                    if (agent.isAvailable)
                    {
                        outList.Add(agent);
                        numAvailableCores += agent.numProcessors;
                    }
                }
            }
            return numAvailableCores;
        }

        private void notifyHerdAgentChanged()
        {
            //we get the agents that sent an ack less than m_agentTimeoutSeconds seconds ago
            m_shepherd.getHerdAgentList(ref m_innerHerdAgentList, m_agentTimeoutSeconds);
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

            m_timer = new System.Timers.Timer(m_updateTimeSeconds*1000);
            m_shepherd.sendBroadcastHerdAgentQuery();
            m_shepherd.beginListeningHerdAgentQueryResponses();

            m_timer.AutoReset = true;
            m_timer.Elapsed += new System.Timers.ElapsedEventHandler(resendBroadcast);
            m_timer.Start();
        }
    }
}
