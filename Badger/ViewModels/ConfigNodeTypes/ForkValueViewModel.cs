using System.IO;
using Simion;
using Caliburn.Micro;
using System.Xml;

namespace Badger.ViewModels
{
    public class ForkValueViewModel:PropertyChangedBase
    {
        private ForkViewModel m_parentFork;
        public ForkViewModel parentFork { get { return m_parentFork; }
        set { m_parentFork = value;  }
        }
        private string m_name;
        public string name { get { return m_name; }set { m_name = value;NotifyOfPropertyChange(() => name); } }

        private ConfigNodeViewModel m_forkValue;
        public ConfigNodeViewModel forkValue
        {
            get { return m_forkValue; }
            set { m_forkValue = value; NotifyOfPropertyChange(() => forkValue); }
        }
        public ForkValueViewModel(AppViewModel appVieModel,ForkViewModel parentFork)
        {
            m_parentFork = parentFork;
        }
        public ForkValueViewModel(AppViewModel appViewModel,XmlNode classDefinition
            , ConfigNodeViewModel parentNode
            , ForkViewModel parentFork, XmlNode configNode)
        {
            name = configNode.Attributes[XMLConfig.nameAttribute].Value;
            m_parentFork = parentFork;
            //not sure how to do this in a more elegant way
            forkValue = ConfigNodeViewModel.getInstance(appViewModel, parentNode
                , classDefinition, parentNode.xPath, configNode);
            forkValue.bCanBeForked = false; //already belongs to a fork
        }

        public void removeThisValue()
        {
            m_parentFork.removeValue(this);
        }

        //addHeader should be true if we are in SaveMode.SaveForks mode
        //else it should be false: the fork is completely hidden
        public void outputXML(StreamWriter writer, string leftSpace, bool addHeader)
        {
            if (addHeader)
                writer.WriteLine(leftSpace + "<" + XMLConfig.forkValueTag + " " + XMLConfig.nameAttribute + "=\""
                    + name + "\">");
            forkValue.outputXML(writer, leftSpace + "  ");
            if (addHeader)
                writer.WriteLine(leftSpace + "</" + XMLConfig.forkValueTag + ">");
        }
    }
}
