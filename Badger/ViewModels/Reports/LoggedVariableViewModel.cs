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
    public class LoggedVariableViewModel:PropertyChangedBase
    {
        private bool m_bIsSelected = true;
        public bool bIsSelected {
            get { return m_bIsSelected; }
            set {m_bIsSelected= value;
                m_parent.validateQuery();
                NotifyOfPropertyChange(()=>bIsSelected);
            }}

        private string m_name;
        public string name { get { return m_name; }
            set { m_name = value; NotifyOfPropertyChange(() => name); }
        }

        private ReportsWindowViewModel m_parent = null;

        public LoggedVariableViewModel(string name,ReportsWindowViewModel parent)
        {
            m_parent = parent;
            m_name = name;
        }
    }
}