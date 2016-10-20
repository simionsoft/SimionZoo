using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Badger.ViewModels
{
    public class ForkedNodeViewModel : ConfigNodeViewModel
    {
        public ConfigNodeViewModel selectedForkValue
        {
            get { return fork.selectedForkValue.forkValue; }
            set
            {
                //we don't need to set it (the actual value will be set within the ForkViewModel class)
                //we just notify that the selection has changed
                NotifyOfPropertyChange(() => selectedForkValue);
            }
        }

        private ForkViewModel m_fork;
        public ForkViewModel fork { get { return m_fork; } set { m_fork = value; } }
        public ForkedNodeViewModel(ConfigNodeViewModel forkedNode)
        {
            name = forkedNode.name;
            NotifyOfPropertyChange(() => selectedForkValue);
        }

        public override bool validate()
        {
            foreach (ForkValueViewModel value in fork.values)
            {
                if (!value.forkValue.validate()) return false;
            }
            return true;
        }
    }
}
