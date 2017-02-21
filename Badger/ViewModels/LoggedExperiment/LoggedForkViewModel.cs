using System.Xml;
using System.Collections.Generic;
using Badger.Simion;

namespace Badger.ViewModels
{
    public class LoggedForkViewModel
    {
        private List<LoggedForkValueViewModel> m_values = new List<LoggedForkValueViewModel>();
        public List<LoggedForkValueViewModel> values
        {
            get { return m_values; }
            set { m_values = value; }
        }

        private List<LoggedForkViewModel> m_forks = new List<LoggedForkViewModel>();
        public List<LoggedForkViewModel> forks
        {
            get { return forks; }
            set { m_forks = value; }
        }

        public LoggedForkViewModel(XmlNode configNode)
        {
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
        }
    }
}
