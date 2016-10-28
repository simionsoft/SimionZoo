using System.IO;
using Simion;
using Caliburn.Micro;
using System.Xml;

namespace Badger.ViewModels
{
    public class ForkValueViewModel:PropertyChangedBase
    {
        private string m_name;
        public string name { get { return m_name; }set { m_name = value;NotifyOfPropertyChange(() => name); } }

        private ConfigNodeViewModel m_configNode;
        public ConfigNodeViewModel configNode
        {
            get { return m_configNode; }
            set { m_configNode = value; NotifyOfPropertyChange(() => configNode); }
        }

        private ForkedNodeViewModel m_parent;

        public ForkValueViewModel(string valueName, ForkedNodeViewModel parent, ConfigNodeViewModel forkedNode)
        {
            name = valueName;
            m_parent = parent;
            configNode = forkedNode;
        }

        public ForkValueViewModel(AppViewModel appViewModel,XmlNode classDefinition
            , ConfigNodeViewModel parentNode, XmlNode configNode)
        {
            name = configNode.Attributes[XMLConfig.nameAttribute].Value;
            //not sure how to do this in a more elegant way
            this.configNode = ConfigNodeViewModel.getInstance(appViewModel, parentNode
                , classDefinition, parentNode.xPath, configNode);
            this.configNode.bCanBeForked = false; //already belongs to a fork
        }


        //addHeader should be true if we are in SaveMode.SaveForks mode
        //else it should be false: the fork is completely hidden
        public void outputXML(StreamWriter writer, string leftSpace, bool addHeader)
        {
            if (addHeader)
                writer.WriteLine(leftSpace + "<" + XMLConfig.forkValueTag + " " + XMLConfig.nameAttribute + "=\""
                    + name + "\">");
            configNode.outputXML(writer, leftSpace + "  ");
            if (addHeader)
                writer.WriteLine(leftSpace + "</" + XMLConfig.forkValueTag + ">");
        }
    }
}
