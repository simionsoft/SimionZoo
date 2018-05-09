using System.Collections.Generic;
using System.Xml;
using Herd;

namespace Badger.Data
{
    public class Requirements
    {
        protected List<string> m_inputFiles = new List<string>();
        protected List<string> m_outputFiles = new List<string>();
        protected Dictionary<string, string> m_renameRules = new Dictionary<string, string>();

        //input files
        public bool AddInputFile(string file)
        {
            if (!m_inputFiles.Contains(file))
            {
                m_inputFiles.Add(file);
                return true;
            }
            else return false;
        }
        public List<string> InputFiles { get { return m_inputFiles; } }

        //output files
        public bool AddOutputFile(string file)
        {
            if (!m_outputFiles.Contains(file))
            {
                m_outputFiles.Add(file);
                return true;
            }
            else return false;
        }
        public List<string> OutputFiles { get { return m_outputFiles; } }

        //rename rules
        /// <summary>
        /// Rename rules: files that must be stored in the remote machine in a different relative location
        /// 
        /// Example: 64 bit runtime C++ libraries have the same name that 32-bit versions have.
        ///         In the local machine, 64 bit libraries are in /bin/64, 32 libraries are in /bin, but both
        ///         must be in the same directory as the .exe using them, so the 64 dll-s must be saved in /bin in
        ///         the remote machine.
        /// </summary>
        public void AddRenameRule(string original, string rename)
        {
            m_renameRules[original] = rename;
        }
        public Dictionary<string, string> RenameRules { get { return m_renameRules; } }


        /// <summary>
        /// Common initialization used by sub-classes of Requirements
        /// </summary>
        /// <param name="node"></param>
        public void CommonInit(XmlNode node)
        {
            //Read from the output of the app itself (run with "-requirements" flag)
            foreach (XmlElement child in node.ChildNodes)
            {
                //parse input files
                if (child.Name == XmlTags.Input)
                {
                    AddInputFile(child.Value);
                    //must the input file be renamed in remote machines??
                    if (child.GetAttribute(XmlTags.RenameAttr) != null)
                        AddRenameRule(child.Value, child.Attributes[XmlTags.RenameAttr].Value);
                }
                else if (child.Name == XmlTags.Output)
                {
                    AddOutputFile(child.Value);
                    //must the input file be renamed in remote machines??
                    if (child.GetAttribute(XmlTags.RenameAttr) != null)
                        AddRenameRule(child.Value, child.Attributes[XmlTags.RenameAttr].Value);
                }
            }
        }

        public override string ToString()
        {
            string xml = "";
            foreach (string file in m_inputFiles)
            {
                if (m_renameRules.ContainsKey(file))
                    xml += "<" + XmlTags.Input + " " + XmlTags.RenameAttr + "=\"" + m_renameRules[file] + "\">"
                        + file + "</" + XmlTags.Input + ">\n";
                else xml += "<" + XmlTags.Input + ">" + file + "</" + XmlTags.Input + ">\n";
            }

            return xml;
        }
    }

    public class AppVersionRequirements : Requirements
    {
        private string m_architecture;
        public string Architecture { get { return m_architecture; } set { m_architecture = value; } }

        public AppVersionRequirements(string architecture)
        {
            m_architecture = architecture;
        }
        public AppVersionRequirements(XmlNode node)
        {
            //Read from the config file in /config (/config/RLSimion.xml)
            CommonInit(node);

            foreach (XmlNode child in node.ChildNodes)
            {
                if (child.Name == PropNames.Architecture)
                {
                    //parse "NumCPUCores" property
                    Architecture = child[PropNames.Architecture].InnerText;
                }
            }
        }

        public override string ToString()
        {
            string commonProperties= base.ToString();
            string xml = "<" + XmlTags.Requirements + ">\n";
            xml += commonProperties + "\n";
            xml += "<" + XmlTags.Architecture + ">" + m_architecture + "</" + XmlTags.Architecture + ">\n";
            xml += "</" + XmlTags.Requirements + ">\n";
            return xml;
        }
    }

    public class RunTimeRequirements : Requirements
    {
        private int m_numCPUCores;
        public int NumCPUCores { get { return m_numCPUCores; } set { m_numCPUCores = value; } }

        public RunTimeRequirements(int numCPUCores)
        {
            m_numCPUCores = numCPUCores;
        }

        public RunTimeRequirements(XmlNode node)
        {
            //Read from the output of the app itself (run with "-requirements" flag)
            CommonInit(node);

            foreach (XmlNode child in node.ChildNodes)
            {
                if (child.Name == PropNames.NumCPUCores)
                {
                    //parse "NumCPUCores" property
                    NumCPUCores = int.Parse(child[PropNames.NumCPUCores].InnerText);
                }
            }
        }

        public override string ToString()
        {
            string commonProperties = base.ToString();
            string xml = "<" + XmlTags.Requirements + ">\n";
            xml += commonProperties + "\n";
            xml += "<" + XmlTags.NumCPUCores + ">" + m_numCPUCores + "</" + XmlTags.NumCPUCores + ">\n";
            xml += "</" + XmlTags.Requirements + ">\n";
            return xml;
        }
    }
}
