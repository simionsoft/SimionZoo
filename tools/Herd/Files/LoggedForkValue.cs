using System;
using System.Collections.Generic;
using System.Xml;

namespace Herd.Files
{
    public class LoggedForkValue
    {
        public string Value;

        public List<LoggedFork> Forks = new List<LoggedFork>();

        public LoggedForkValue(XmlNode configNode)
        {
  
            if (configNode.Attributes.GetNamedItem(XMLTags.valueAttribute) != null)
                Value = configNode.Attributes[XMLTags.valueAttribute].Value;
            else
                Value = configNode.InnerText;

            foreach (XmlNode child in configNode.ChildNodes)
            {
                if (child.Name == XMLTags.forkTag)
                {
                    LoggedFork newFork = new LoggedFork(child);
                    Forks.Add(newFork);
                }
            }
        }
    }
}
