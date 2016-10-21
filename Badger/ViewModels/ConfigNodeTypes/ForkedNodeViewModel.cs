using System.IO;
using Simion;
using System.Xml;

namespace Badger.ViewModels
{
    public class ForkedNodeViewModel : ConfigNodeViewModel
    {
        public ConfigNodeViewModel selectedForkValue
        {
            get { return fork.selectedForkValue.configNode; }
            set
            {
                //we don't need to set it (the actual value will be set within the ForkViewModel class)
                //we just notify that the selection has changed
                NotifyOfPropertyChange(() => selectedForkValue);
            }
        }

        private ForkViewModel m_fork;
        public ForkViewModel fork { get { return m_fork; } set { m_fork = value; } }
        //Constructor used from the experiment editor
        public ForkedNodeViewModel(AppViewModel appViewModel,ConfigNodeViewModel forkedNode)
        {
            m_appViewModel = appViewModel;
            nodeDefinition = forkedNode.nodeDefinition;
            name = forkedNode.name;
            NotifyOfPropertyChange(() => selectedForkValue);
        }
        //Constructor called when loading an experiment config file
        public ForkedNodeViewModel(AppViewModel appViewModel,XmlNode classDefinition,XmlNode configNode)
        {
            name = configNode.Attributes[XMLConfig.nameAttribute].Value;
            fork = new ForkViewModel(appViewModel,name,this);
            foreach (XmlNode forkValueConfig in configNode)
            {
                fork.values.Add(new ForkValueViewModel(appViewModel, fork, forkValueConfig));
            }
        }
        public override bool validate()
        {
            foreach (ForkValueViewModel value in fork.values)
            {
                if (!value.configNode.validate()) return false;
            }
            return true;
        }

        public override void outputXML(StreamWriter writer,string leftSpace)
        {
            if (m_appViewModel.saveMode == SaveMode.SaveForks)
            {
                writer.Write(leftSpace + "<" + XMLConfig.forkedNodeTag + " " 
                    +XMLConfig.nameAttribute + "=\"" + name.TrimEnd(' ') + "\">");
                fork.outputXML(writer, leftSpace + "  ");
                writer.WriteLine(leftSpace + "<" + XMLConfig.forkedNodeTag + ">");
            }
            else
                selectedForkValue.outputXML(writer, leftSpace);
        }
    }
}
