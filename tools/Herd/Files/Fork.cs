
using System.Collections.Generic;
using System.Xml;

namespace Herd.Files
{
    public class Fork
    {
        public string Name;

        public List<Fork> Forks = new List<Fork>();
        public List<ForkValue> Values = new List<ForkValue>();

        public Fork(XmlNode configNode)
        {
            if (configNode.Attributes.GetNamedItem(XMLTags.aliasAttribute) != null)
                Name = configNode.Attributes[XMLTags.aliasAttribute].Value;

            foreach (XmlNode child in configNode.ChildNodes)
            {
                if (child.Name == XMLTags.forkTag)
                {
                    Fork newFork = new Fork(child);
                    Forks.Add(newFork);
                }
                else if (child.Name == XMLTags.forkValueTag)
                {
                    ForkValue newValue = new ForkValue(child);
                    Values.Add(newValue);
                }
            }
        }
    }
}
