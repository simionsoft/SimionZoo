using System;
using Caliburn.Micro;
using System.Collections.Generic;

namespace Badger.ViewModels
{
    public class MonitoredJobStateViewModel: PropertyChangedBase
    {
        public const string ConnectionOK = "ConnectionOK";
        public const string ConnectionError = "ConnectionError";

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
