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
        public ConfigNodeViewModel configNode
        {
            get { return m_forkValue; }
            set { m_forkValue = value; NotifyOfPropertyChange(() => configNode); }
        }
        //constructor used when a node is forked or a node is cloned
        public ForkValueViewModel(ForkViewModel parent)
        {
            m_parentFork = parent;
        }
        //constructor used to clone ConfigNodes (values are added to a fork node)
        //public ForkValueViewModel(ForkViewModel parent, ForkValueViewModel originalValue)
        //{
        //    m_parent = parent;
        //    forkValue = originalValue.forkValue.clone();
        //}
        //constructor used when forks are read from a config file
        public ForkValueViewModel(AppViewModel appViewModel,ForkViewModel parent, XmlNode configNode)
        {
            m_parentFork = parent;
            //not sure how to do this in a more elegant way
            this.configNode = ConfigNodeViewModel.getInstance(appViewModel, null, configNode, "");
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
            configNode.outputXML(writer, leftSpace + "  ");
            if (addHeader)
                writer.WriteLine(leftSpace + "<" + XMLConfig.forkTag + "/>");
        }
    }
}
