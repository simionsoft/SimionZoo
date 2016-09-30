using System.Xml;
using System.Collections.Generic;
using Simion;

namespace Badger.ViewModels
{
    class AppViewModel
    {
        private List<string> m_preFiles= new List<string>();
        private List<string> m_exeFiles = new List<string>();
        private List<XmlNode> m_classDefinitions = new List<XmlNode>();
        private List<XmlNode> m_enumDefinitions = new List<XmlNode>();
        public XmlNode getClassDefinition(string className)
        {
            return m_classDefinitions.Find(node => node.Attributes["Name"].Value == className);
        }
        public XmlNode getEnumDefinition(string enumName)
        {
            return m_enumDefinitions.Find(node => node.Attributes["Name"].Value == enumName);
        }

        private string m_name;
        private string m_version;

        List<ConfigNode> m_children;

        public AppViewModel(string fileName)
        {
            XmlDocument configDocument = new XmlDocument();
            configDocument.Load(fileName);

            foreach (XmlNode rootChild in configDocument.ChildNodes)
            {
                if (rootChild.Name == XMLConfig.appNodeTag)
                {
                    //APP node
                    foreach (XmlNode configNode in rootChild.ChildNodes)
                    {
                        m_preFiles.Clear();
                        m_exeFiles.Clear();
                        m_name = configNode.Attributes["Name"].Value;
                        m_version = configNode.Attributes["FileVersion"].Value;

                        foreach (XmlNode child in configNode.ChildNodes)
                        {
                            //Only EXE, PRE, INCLUDE and BRANCH children nodes
                            if (child.Name == "EXE") m_exeFiles.Add(child.Value);
                            else if (child.Name == "PRE") m_preFiles.Add(child.Value);
                            else if (child.Name == "INCLUDE")
                                loadIncludedDefinitionFile(child.Value);
                            else m_children.Add(ConfigNode.getInstance(child));
                        }
                    }
                }
            }
        }
        private void loadIncludedDefinitionFile(string appDefinitionFile)
        {
            XmlDocument configDocument = new XmlDocument();
            configDocument.Load(appDefinitionFile);
            foreach (XmlNode child in configDocument.ChildNodes)
            {
                if (child.Name == XMLConfig.definitionNodeTag)
                {
                    foreach (XmlNode definition in child.ChildNodes)
                    {
                        if (definition.Name == XMLConfig.classDefinitionNodeTag)
                            m_classDefinitions.Add(definition);
                        else if (definition.Name == XMLConfig.enumDefinitionNodeTag)
                            m_enumDefinitions.Add(definition);
                    }
                }
            }
        }
    }
}
