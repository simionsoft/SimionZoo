using System.IO;
using Caliburn.Micro;
using Simion;
using System.Xml;

namespace Badger.ViewModels
{
    public class ForkViewModel : PropertyChangedBase
    {
        private AppViewModel m_appViewModel;
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

        
        
        public ForkViewModel(AppViewModel appViewModel,string forkName, ForkedNodeViewModel forkConfigNode)
        {
            m_appViewModel = appViewModel;
            name = forkName;
            m_forkNode = forkConfigNode;
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
            int i = 0;
            foreach(ForkValueViewModel fValue in values)
            {
                fValue.name= "Value-" + i;
                i++;
            }
        }
        public void addValue()
        {
            int valueId = values.Count;
            ForkValueViewModel newForkValue = new ForkValueViewModel(m_appViewModel,this);
            newForkValue.forkValue = selectedForkValue.forkValue.clone();
            newForkValue.name = "Value-" + values.Count;
            values.Add(newForkValue);
        }

        public void outputXML(StreamWriter writer,string leftSpace)
        {
            foreach (ForkValueViewModel forkValue in m_values)
            {
                forkValue.outputXML(writer, leftSpace + "  "
                    ,m_appViewModel.saveMode==SaveMode.SaveForks);
            }
        }
    }
}
