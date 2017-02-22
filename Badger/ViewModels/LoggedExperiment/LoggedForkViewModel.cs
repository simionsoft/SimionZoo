using System.Xml;
using System.Collections.Generic;
using Badger.Simion;

namespace Badger.ViewModels
{
    public class LoggedForkViewModel: SelectableTreeItem
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

        public override void OnSelectedChange(bool bSelected)
        {
            foreach(LoggedForkValueViewModel value in m_values) value.bIsSelected= bSelected;
            foreach (LoggedForkViewModel fork in m_forks) fork.bIsSelected = bSelected;
        }

        public LoggedForkViewModel(XmlNode configNode)
        {
            if (configNode.Attributes.GetNamedItem(XMLConfig.aliasAttribute) != null)
                name = configNode.Attributes[XMLConfig.aliasAttribute].Value;

            foreach(XmlNode child in configNode.ChildNodes)
            {
                if (child.Name==XMLConfig.forkTag)
                {
                    LoggedForkViewModel newFork = new LoggedForkViewModel(child);
                    forks.Add(newFork);
                }
                else if (child.Name==XMLConfig.forkValueTag)
                {
                    LoggedForkValueViewModel newValue = new LoggedForkValueViewModel(child);
                    values.Add(newValue);
                }
            }

            if (forks.Count == 0) bVisible = false;
        }
    }
}
