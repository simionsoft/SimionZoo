using System;
using Caliburn.Micro;
using System.Collections.Generic;

namespace Badger.ViewModels
{
    public class MonitoredJobStateViewModel: PropertyChangedBase
    {
        string m_icon = null;
        public string Icon
        {
            get { return m_icon; }
            set
            {
                m_icon = value;
                NotifyOfPropertyChange(() => Icon);
            }
        }

        public MonitoredJobStateViewModel()
        {
        }
    }
}
