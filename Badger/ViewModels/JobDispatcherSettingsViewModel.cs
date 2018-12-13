using System;

using Caliburn.Micro;
using Herd.Network;

namespace Badger.ViewModels
{
    public class JobDispatcherSettingsViewModel : PropertyChangedBase
    {
        JobDispatcherOptions m_options;

        public JobDispatcherSettingsViewModel(JobDispatcherOptions options)
        {
            m_options = options;
        }

        public bool LeaveOneFreeCore
        {
            get { return m_options.LeaveOneFreeCore; }
            set { m_options.LeaveOneFreeCore = value; NotifyOfPropertyChange(() => LeaveOneFreeCore); }
        }
        public bool BalanceLoad
        {
            get { return m_options.BalanceLoad; }
            set { m_options.BalanceLoad = value; NotifyOfPropertyChange(() => BalanceLoad); }
        }
    }
}
