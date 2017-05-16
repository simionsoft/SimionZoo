using System.Xml;
using System.Collections.Generic;
using Badger.Data;
using Badger.Simion;
using System;
using Caliburn.Micro;

namespace Badger.ViewModels
{
    public class LoggedForkViewModel : SelectableTreeItem
    {
        private string m_name = "unnamed";
        public string name
        {
            get { return m_name; }
            set { m_name = value; }
        }
        private List<LoggedForkValueViewModel> m_values = new List<LoggedForkValueViewModel>();
        public List<LoggedForkValueViewModel> values
        {
            get { return m_values; }
            set { m_values = value; }
        }

        private List<LoggedForkViewModel> m_forks = new List<LoggedForkViewModel>();
        public List<LoggedForkViewModel> forks
        {
            get { return m_forks; }
            set { m_forks = value; }
        }

        //this is used to hide the space given to display children forks in case there is none
        private bool m_bHasForks = false;
        public bool bHasForks
        {
            get { return m_bHasForks; }
            set { m_bHasForks = value; NotifyOfPropertyChange(() => bHasForks); }
        }


        public LoggedForkViewModel(XmlNode configNode)
        {
            if (configNode.Attributes.GetNamedItem(XMLConfig.aliasAttribute) != null)
                name = configNode.Attributes[XMLConfig.aliasAttribute].Value;

            foreach (XmlNode child in configNode.ChildNodes)
            {
                if (child.Name == XMLConfig.forkTag)
                {
                    LoggedForkViewModel newFork = new LoggedForkViewModel(child);
                    forks.Add(newFork);
                }
                else if (child.Name == XMLConfig.forkValueTag)
                {
                    LoggedForkValueViewModel newValue = new LoggedForkValueViewModel(child);
                    values.Add(newValue);
                }
            }

            //hide the area used to display children forks?
            bHasForks = forks.Count != 0;
        }
        

        public override void TraverseAction(bool doActionLocally, Action<SelectableTreeItem> action)
        {
            if (doActionLocally) LocalTraverseAction(action);
            foreach (LoggedForkValueViewModel value in m_values) value.TraverseAction(true, action);
            foreach (LoggedForkViewModel fork in m_forks) fork.TraverseAction(true, action);
        }
    }
}
