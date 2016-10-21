using System.IO;
using Simion;

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
        public ForkedNodeViewModel(AppViewModel appViewModel,ConfigNodeViewModel forkedNode)
        {
            m_appViewModel = appViewModel;
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

        public override void outputXML(StreamWriter writer,string leftSpace)
        {
            if (m_appViewModel.saveMode == SaveMode.SaveForks)
            {
                writer.Write(leftSpace + "<" + XMLConfig.forkedNodeTag + " " 
                    +XMLConfig.nameAttribute + "=\"" + name.TrimEnd(' ') + "\">");
                writer.Write(fork.selectedForkValue.name);
                writer.WriteLine(leftSpace + "<" + XMLConfig.forkedNodeTag + ">");
            }
            else
                selectedForkValue.outputXML(writer, leftSpace);
        }
    }
}
