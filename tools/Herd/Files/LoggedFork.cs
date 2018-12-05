
using System.Collections.Generic;
using System.Xml;

namespace Herd.Files
{
    public class LoggedFork
    {
        public string Name;

        public List<LoggedFork> Forks = new List<LoggedFork>();
        public List<LoggedForkValue> Values = new List<LoggedForkValue>();

        public LoggedFork(XmlNode configNode)
        {
            if (configNode.Attributes.GetNamedItem(XMLTags.aliasAttribute) != null)
                Name = configNode.Attributes[XMLTags.aliasAttribute].Value;

            foreach (XmlNode child in configNode.ChildNodes)
            {
                if (child.Name == XMLTags.forkTag)
                {
                    LoggedFork newFork = new LoggedFork(child);
                    Forks.Add(newFork);
                }
                else if (child.Name == XMLTags.forkValueTag)
                {
                    LoggedForkValue newValue = new LoggedForkValue(child);
                    Values.Add(newValue);
                }
            }
        }
    }
}
