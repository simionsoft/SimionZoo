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
        private ForkViewModel m_parent;
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
        //main constructor method
        public ForkValueViewModel(ForkViewModel parent, ConfigNodeViewModel forkedValue, int valueId)
        {
            m_parent = parent;
            forkValue = forkedValue;
            id = valueId;
        }
        //constructor used to clone ConfigNodes
        public ForkValueViewModel(ForkViewModel parent, ForkValueViewModel originalValue, int valueId)
        {
            m_parent = parent;
            forkValue = originalValue.forkValue.clone();
            id = valueId;
        }

        public void removeThisValue()
        {
            m_parent.removeValue(this);
        }
    }
}
