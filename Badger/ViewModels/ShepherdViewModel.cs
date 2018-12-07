using System.Collections.Generic;
using System.Runtime.CompilerServices;

using Caliburn.Micro;

using Badger.Data;

using Herd.Network;

namespace Badger.ViewModels
{
    public class ShepherdViewModel : PropertyChangedBase
    {
        private Shepherd m_shepherd;
        public Shepherd shepherd { get { return m_shepherd; } set { } }

        public BindableCollection<HerdAgentViewModel> HerdAgentList
        {
            get; set;
        } = new BindableCollection<HerdAgentViewModel>();

        public int GetAvailableHerdAgents(ref List<HerdAgentInfo> outList)
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
                        outList.Add(agent.Info);
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



        public void SelectHerdAgents()
        {
            CaliburnUtility.ShowPopupWindow(new HerdAgentSelectionViewModel(HerdAgentList), "Herd Aget selection tool");
        }


        public ShepherdViewModel()
        {
            HerdAgentList.Clear();

            m_shepherd = new Shepherd();
            m_shepherd.SetOnHerdAgentDiscoveryFunc(OnHerdAgetDiscovery);
            m_shepherd.CallHerd();
        }
    }
}
