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
        
        private BindableCollection <HerdAgentDescription> m_herdAgentList= new BindableCollection<HerdAgentDescription>();
        public BindableCollection <HerdAgentDescription> herdAgentList
        {
            get { return m_herdAgentList;  }
            set { }
        }
        private void notifyHerdAgentChanged()
        {
            //we get the agents that sent an ack less than m_agentTimeoutSeconds seconds ago
            lock (m_herdAgentList) { m_shepherd.getHerdAgentList(ref m_herdAgentList, m_agentTimeoutSeconds); }
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
