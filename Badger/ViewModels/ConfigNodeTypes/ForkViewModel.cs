using System;
using System.Collections.Generic;
using Caliburn.Micro;

namespace Badger.ViewModels
{
    public class ForkViewModel : PropertyChangedBase
    {
        private AppViewModel m_parent;
        private string m_name;
        public string name { get { return m_name; } set { m_name = value; } }
        private ForkedNodeViewModel m_forkNode;

        private BindableCollection<ForkValueViewModel> m_values = new BindableCollection<ForkValueViewModel>();
        public BindableCollection<ForkValueViewModel> values
        {
            get { return m_values; }
            set { m_values = value; NotifyOfPropertyChange(() => values); }
        }

        private ForkValueViewModel m_selectedForkValue= null;
        public ForkValueViewModel selectedForkValue
        {
            get { return m_selectedForkValue; }
            set {
                m_selectedForkValue = value;
                NotifyOfPropertyChange(() => selectedForkValue);
                m_forkNode.selectedForkValue= selectedForkValue.forkValue;
            }
        }

        //the forked node is the old node, which has already been replaced by the forkNode
        //the forkNode will reference to this Fork
        //the forkedNode will be added as the first value of this Fork
        public ForkViewModel(AppViewModel parent, ConfigNodeViewModel forkedNode, ForkedNodeViewModel forkNode)
        {
            m_parent = parent;
            name = forkedNode.name;
            m_forkNode = forkNode;
            ForkValueViewModel newForkValue = new ForkValueViewModel(this, forkedNode,m_values.Count);
            values.Add(newForkValue);
            selectedForkValue = newForkValue;
        }

        public void removeValue(ForkValueViewModel forkValue)
        {
            //we don't remove the value if there is no other value
            if (values.Count == 1) return;

            if (selectedForkValue == forkValue)
            {
                int index = values.IndexOf(forkValue);
                if (index == values.Count - 1)
                    selectedForkValue = values[index - 1];
                else
                    selectedForkValue = values[index + 1];
            }
            values.Remove(forkValue);
        }
        public void addValue()
        {
            int valueId = values.Count;
            ForkValueViewModel newForkValue = new ForkValueViewModel(this, values[values.Count - 1], valueId);
            values.Add(newForkValue);
        }
    }
}
