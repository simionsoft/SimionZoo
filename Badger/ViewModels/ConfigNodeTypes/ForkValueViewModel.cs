using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Caliburn.Micro;

namespace Badger.ViewModels
{
    public class ForkValueViewModel:PropertyChangedBase
    {
        private string m_name;
        public string name { get { return m_name; }set { m_name = value;NotifyOfPropertyChange(() => name); } }

        private int m_id;
        public int id
        {
            get { return m_id; }
            set
            {
                m_id = value; NotifyOfPropertyChange(() => id);
                name = "Value(" + m_id + ")";
            }
        }

        private ConfigNodeViewModel m_forkValue;
        public ConfigNodeViewModel forkValue
        {
            get { return m_forkValue; }
            set { m_forkValue = value; NotifyOfPropertyChange(() => forkValue); }
        }

        public ForkValueViewModel(ConfigNodeViewModel forkedValue, int valueId)
        {
            forkValue = forkedValue;
            id = valueId;
        }
    }
}
