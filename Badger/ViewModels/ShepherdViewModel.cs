using System.Collections.Generic;
using System.Linq;
using Herd;
using Caliburn.Micro;
using System;
using Badger.Data;
using System.Runtime.CompilerServices;

namespace Badger.ViewModels
{
    public class ShepherdViewModel : PropertyChangedBase//, IDisposable
    {
        //protected virtual void Dispose(bool disposing)
        //{
        //    if (disposing)
        //    {
        //        // dispose managed resources
        //        m_timer.Dispose();
        //    }
        //}
        //public void Dispose()
        //{
        //    Dispose(true);
        //    GC.SuppressFinalize(this);
        //}

        // const int m_agentTimeoutSeconds = 10;
        //const int m_updateTimeSeconds = 3;
        //System.Timers.Timer m_timer;

        private Shepherd m_shepherd;
        public Shepherd shepherd { get { return m_shepherd; } set { } }

        public BindableCollection<HerdAgentViewModel> HerdAgentList
        {
            get; set;
        } = new BindableCollection<HerdAgentViewModel>();

        public int getAvailableHerdAgents(ref List<HerdAgentViewModel> outList)
        {
            //we assume outList needs no synchronization
            int numAvailableCores = 0;
            //lock (m_listsLock)
            {
                outList.Clear();
                foreach (HerdAgentViewModel agent in HerdAgentList)
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

        [MethodImpl(MethodImplOptions.Synchronized)]
        private void OnHerdAgetDiscovery(HerdAgentInfo newHerdAgent)
        {
            int i = 0;
            while (i < HerdAgentList.Count - 1 && HerdAgentList[i].NumProcessors >= newHerdAgent.NumProcessors) i++;

            if (i <= HerdAgentList.Count - 1)
                HerdAgentList.Insert(i, new HerdAgentViewModel(newHerdAgent));
            else HerdAgentList.Add(new HerdAgentViewModel(newHerdAgent));

            NotifyOfPropertyChange(() => HerdAgentList);
        }

        public void SendAgentDiscoveryBroadcast()
        {
            HerdAgentList.Clear();
            m_shepherd.SendBroadcastHerdAgentQuery();
        }

        public void SelectHerdAgents()
        {
            CaliburnUtility.ShowPopupWindow(new HerdAgentSelectionViewModel(HerdAgentList), "Herd Aget selection tool");
        }


        public ShepherdViewModel()
        {
            m_shepherd = new Shepherd();
            m_shepherd.SetOnHerdAgentDiscoveryFunc(OnHerdAgetDiscovery);

            SendAgentDiscoveryBroadcast();
            m_shepherd.BeginListeningHerdAgentQueryResponses();
        }
    }
}
