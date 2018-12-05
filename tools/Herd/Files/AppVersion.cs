using System;
using System.Collections.Generic;
using System.Xml;
using Herd.Network;

namespace Herd.Files
{
    public class AppVersion
    {
        string m_name = PropValues.None;
        string m_exeFile = PropValues.None;

        private AppVersionRequirements m_requirements;
        public AppVersionRequirements Requirements { get { return m_requirements; } }

        public string ExeFile { get { return m_exeFile; } }

        public override string ToString()
        {
            string xml = "<" + XmlTags.Version + " " + XMLTags.nameAttribute + "=\"" + m_name + "\">\n";
            xml += "<" + XmlTags.Exe + ">" + m_exeFile + "</" + XmlTags.Exe + ">\n";
            xml += m_requirements.ToString();
            xml += "</" + XmlTags.Version + ">";
            return xml;
        }

        public AppVersion(string name, string exeFile, AppVersionRequirements requirements)
        {
            m_name = name;
            m_exeFile = exeFile;
            m_requirements = requirements;
        }

        public AppVersion(XmlNode node)
        {
            if (node.Attributes.GetNamedItem(XMLTags.nameAttribute) != null)
                m_name = node.Attributes[XMLTags.nameAttribute].Value;

            foreach (XmlElement child in node.ChildNodes)
            {
                //Only Exe and AppVersionRequirements (i/o files and architecture)
                if (child.Name == XmlTags.Exe)
                    m_exeFile = child.InnerText;
                else if (child.Name == XmlTags.Requirements)
                {
                    m_requirements = new AppVersionRequirements(child);
                }
            }

        }
        /// <summary>
        /// Returns the best match (assuming versions are ordered by preference) to the local machine's architecture
        /// </summary>
        /// <param name="versions"></param>
        public static AppVersion BestMatch(List<AppVersion> versions)
        {
            foreach (AppVersion version in versions)
            {
                if (HerdAgent.ArchitectureId() == version.Requirements.Architecture)
                    return version;
            }
            return null;
        }
    }
}
