using System;
using Caliburn.Micro;
using System.Collections.Generic;

namespace Badger.ViewModels
{
    public class MonitoredExperimentStateViewModel: PropertyChangedBase
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


        public MonitoredExperimentStateViewModel()
        {
        }

        //public void OnClick()
        //{
        //    switch (Icon)
        //    {
        //        case "Running": Icon = "Finished";break;
        //        case "Error": Icon = "Sending";break;
        //        case "Sending": Icon = "Receiving"; break;
        //        case "Receiving": Icon = "Awaiting"; break;
        //        case "Awaiting": Icon = "Running"; break;
        //        default: Icon = "Running"; break;
        //    }
        //}
    }
}
