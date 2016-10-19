using System;
using System.Collections.Generic;
using Caliburn.Micro;

namespace Badger.ViewModels
{
    public class ForkViewModel : PropertyChangedBase
    {
        private string m_name;
        public string name { get { return m_name; } set { m_name = value; } }
        public ForkedNodeViewModel forkNode;

        private BindableCollection<ForkValueViewModel> m_values = new BindableCollection<ForkValueViewModel>();
        public BindableCollection<ForkValueViewModel> values
        {
            get { return m_values; }
            set { m_values = value; NotifyOfPropertyChange(() => values); }
        }

        private ConfigNodeViewModel m_selectedForkValue= null;
        public ConfigNodeViewModel selectedForkValue
        {
            get { return m_selectedForkValue; }
            set { m_selectedForkValue = value;  }
        }

        //the forked node is the old node, which has already been replaced by the forkNode
        //the forkNode will reference to this Fork
        //the forkedNode will be added as the first value of this Fork
        public ForkViewModel(ConfigNodeViewModel forkedNode, ForkedNodeViewModel forkNode)
        {
            name = forkedNode.name;
            values.Add(new ForkValueViewModel(forkedNode,m_values.Count));
            selectedForkValue = forkedNode;
        }
    }
}
