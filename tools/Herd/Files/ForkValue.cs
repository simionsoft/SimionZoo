using System;
using System.Collections.Generic;
using System.Xml;

namespace Herd.Files
{
    public class ForkValue
    {
        public string Value;

        public List<Fork> Forks = new List<Fork>();

        public ForkValue(XmlNode configNode)
        {
  
            if (configNode.Attributes.GetNamedItem(XMLTags.valueAttribute) != null)
                Value = configNode.Attributes[XMLTags.valueAttribute].Value;
            else
                Value = configNode.InnerText;

            foreach (XmlNode child in configNode.ChildNodes)
            {
                if (child.Name == XMLTags.forkTag)
                {
                    Fork newFork = new Fork(child);
                    Forks.Add(newFork);
                }
            }
        }
    }
}
