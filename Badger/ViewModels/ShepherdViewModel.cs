/*
	SimionZoo: A framework for online model-free Reinforcement Learning on continuous
	control problems

	Copyright (c) 2016 SimionSoft. https://github.com/simionsoft

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

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
            CaliburnUtility.ShowPopupWindow(new HerdAgentSelectionViewModel(HerdAgentList), "Herd Agent selection");
        }

        public JobDispatcherOptions DispatcherOptions { get; } = new JobDispatcherOptions();
        public void ConfigureJobDispatcher()
        {
            CaliburnUtility.ShowPopupWindow(new JobDispatcherSettingsViewModel(DispatcherOptions), "Job Dispathcer settings");
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
