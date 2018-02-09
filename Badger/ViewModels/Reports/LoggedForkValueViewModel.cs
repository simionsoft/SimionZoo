using Caliburn.Micro;
using System.Xml;
using Badger.Data;
using Badger.Simion;
using System.Collections.Generic;

namespace Badger.ViewModels
{

    public class LoggedForkValueViewModel: SelectableTreeItem
    {
        private string m_value = "";
        public string Value { get { return m_value; } set { m_value = value; } }

        private BindableCollection<LoggedForkViewModel> m_forks = new BindableCollection<LoggedForkViewModel>();
        public BindableCollection<LoggedForkViewModel> Forks
        {
            get { return m_forks; }
            set { m_forks = value; NotifyOfPropertyChange(() => Forks); }
        }

        //this is used to hide the space given to display children forks in case there is none
        private bool m_bHasForks = false;
        public bool HasChildrenForks
        {
            get { return m_bHasForks; }
            set { m_bHasForks = value; NotifyOfPropertyChange(() => HasChildrenForks); }
        }

        public LoggedForkViewModel parent { get; set; }
        public List<LoggedExperimentalUnitViewModel> expUnits { get; }
            = new List<LoggedExperimentalUnitViewModel>();

        public LoggedForkValueViewModel(XmlNode configNode, LoggedForkViewModel _parent)
        {
            parent = _parent;

            if (configNode.Attributes.GetNamedItem(XMLConfig.valueAttribute) != null)
                Value = configNode.Attributes[XMLConfig.valueAttribute].Value;
            else
                Value = configNode.InnerText;

            foreach (XmlNode child in configNode.ChildNodes)
            {
                if (child.Name == XMLConfig.forkTag)
                {
                    LoggedForkViewModel newFork = new LoggedForkViewModel(child);
                    Forks.Add(newFork);
                }
            }

            //hide the area used to display children forks?
            HasChildrenForks = Forks.Count != 0;
        }

        public override void TraverseAction(bool doActionLocally,System.Action<SelectableTreeItem> action)
        {
            if (doActionLocally) LocalTraverseAction(action);
            foreach (LoggedForkViewModel fork in Forks)
                fork.TraverseAction(doActionLocally, action);
            foreach (LoggedExperimentalUnitViewModel expUnit in expUnits)
                expUnit.LocalTraverseAction(action);
        }
    }
}
