using System.IO;
using Simion;
using Caliburn.Micro;
using System.Xml;
using System;

namespace Badger.ViewModels
{
    public class ForkValueViewModel:ConfigNodeViewModel
    {

        private ConfigNodeViewModel m_configNode;
        public ConfigNodeViewModel configNode
        {
            get { return m_configNode; }
            set { m_configNode = value; NotifyOfPropertyChange(() => configNode); }
        }

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

        public override bool validate()
        {
            return configNode.validate();
        }

        public override ConfigNodeViewModel clone()
        {
            //can't clone this type of node
            throw new NotImplementedException();
        }
    }
}
