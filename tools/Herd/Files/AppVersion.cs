/*
	SimionZoo: A framework for online model-free Reinforcement Learning on continuous
	control problems

	Copyright (c) 2016 SimionSoft. https://github.com/simionsoft

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

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

            foreach (XmlNode childNode in node.ChildNodes)
            {
                XmlElement child = childNode as XmlElement;
                if (child == null)
                    continue;

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
        /// <param name="versions">Input list of app versions</param>
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
