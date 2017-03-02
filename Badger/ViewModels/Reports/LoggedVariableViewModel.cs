using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;
using System.Dynamic;
using System.Windows;
using Caliburn.Micro;
using System.IO;

namespace Badger.ViewModels
{
    public class LoggedVariableViewModel : PropertyChangedBase
    {
        private bool m_bIsSelected = true;
        public bool bIsSelected
        {
            get { return m_bIsSelected; }
            set
            {
                m_bIsSelected = value;
                m_parent.validateQuery();
                NotifyOfPropertyChange(() => bIsSelected);
            }
        }

        private bool m_bCheckIsVisible = true;
        public bool bCheckIsVisible
        {
            get { return m_bCheckIsVisible; }
            set { m_bCheckIsVisible = value; NotifyOfPropertyChange(() => bCheckIsVisible); }
        }

        private string m_name;
        public string name { get { return m_name; }
            set { m_name = value; NotifyOfPropertyChange(() => name); }
        }

        private ReportsWindowViewModel m_parent = null;

        public enum PlotType { Last,All,Both};

        private MultiStateButtonViewModel m_stateButton =
            new MultiStateButtonViewModel(new List<string>() { PlotType.Last.ToString()
                                                                , PlotType.All.ToString()
                                                                , PlotType.Both.ToString()});

        public MultiStateButtonViewModel stateButton
        {
            get { return m_stateButton; }
            set { }
        }

        public LoggedVariableViewModel(string name,ReportsWindowViewModel parent)
        {
            m_parent = parent;
            m_name = name;
        }
    }
}