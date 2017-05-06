using Caliburn.Micro;
using System.Xml;
using Badger.Data;
using Badger.Simion;

namespace Badger.ViewModels
{

    public class LoggedForkValueViewModel: SelectableTreeItem
    {
        private string m_value = "";
        public string value { get { return m_value; } set { m_value = value; } }

        private BindableCollection<LoggedForkViewModel> m_forks = new BindableCollection<LoggedForkViewModel>();
        public BindableCollection<LoggedForkViewModel> forks
        {
            get { return m_forks; }
            set { m_forks = value; NotifyOfPropertyChange(() => forks); }
        }

        //this is used to hide the space given to display children forks in case there is none
        private bool m_bHasForks = false;
        public bool bHasForks
        {
            get { return m_bHasForks; }
            set { m_bHasForks = value; NotifyOfPropertyChange(() => bHasForks); }
        }

        public LoggedForkValueViewModel(XmlNode configNode, Window parent)
        {
            m_parentWindow = parent;

            if (configNode.Attributes.GetNamedItem(XMLConfig.valueAttribute) != null)
                value = configNode.Attributes[XMLConfig.valueAttribute].Value;
            else
                value = configNode.InnerText;

            foreach (XmlNode child in configNode.ChildNodes)
            {
                if (child.Name == XMLConfig.forkTag)
                {
                    LoggedForkViewModel newFork = new LoggedForkViewModel(child, parent);
                    forks.Add(newFork);
                }
            }

            //hide the area used to display children forks?
            bHasForks = forks.Count != 0;
        }

        public override void TraverseAction(bool doActionLocally,System.Action<SelectableTreeItem> action)
        {
            if (doActionLocally) LocalTraverseAction(action);
            foreach (LoggedForkViewModel fork in forks)
                fork.TraverseAction(doActionLocally, action);
        }
    }
}
