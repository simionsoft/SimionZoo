using System;
using Caliburn.Micro;
using System.Collections.Generic;

namespace Badger.ViewModels
{
    public class MultiStateButtonViewModel: PropertyChangedBase
    {
        private List<string> m_states= null;

        private string m_state = null;
        public string state
        {
            get { return m_state; }
            set { m_state = value;  NotifyOfPropertyChange(() => state); }
        }

        public MultiStateButtonViewModel(List<string> buttonStates)
        {
            if (buttonStates.Count == 0) return;

            m_states = buttonStates;
            state = buttonStates[0];
        }

        public void nextState()
        {
            int stateIndex = m_states.IndexOf(state);
            int nextStateIndex = ++stateIndex % m_states.Count;
            state = m_states[nextStateIndex];
        }
    }
}
