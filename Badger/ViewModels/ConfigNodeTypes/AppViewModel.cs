using System.Xml;
using System.Collections.Generic;
using Caliburn.Micro;

using Simion;

namespace Badger.ViewModels
{
    public class AppViewModel: PropertyChangedBase
    {
        private List<string> m_preFiles= new List<string>();
        private List<string> m_exeFiles = new List<string>();
        private Dictionary<string,XmlNode> m_classDefinitions = new Dictionary<string, XmlNode>();
        private Dictionary<string,XmlNode> m_enumDefinitions = new Dictionary<string, XmlNode>();
        private XmlDocument m_configDocument = new XmlDocument();

        public XmlNode getClassDefinition(string className)
        {
            return m_classDefinitions[className];
        }
        public XmlNode getEnumDefinition(string enumName)
        {
            return m_enumDefinitions[enumName];
        }

        private string m_name;
        public string name { get { return m_name; }set { m_name = name; } }
        private string m_version;

        private BindableCollection<ConfigNodeViewModel> m_children= new BindableCollection<ConfigNodeViewModel>();
        public BindableCollection<ConfigNodeViewModel> children { get { return m_children; }
            set { m_children = value; NotifyOfPropertyChange(() => children); } }

        //This constructor does not build the whole tree of ConfigNodes. It just initializes the root node
        //To build the whole tree, use load(configNode) or defaultInit()
        public AppViewModel(string fileName)
        {
            m_configDocument.Load(fileName);

            foreach (XmlNode rootChild in m_configDocument.ChildNodes)
            {
                if (rootChild.Name == XMLConfig.appNodeTag)
                {
                    //APP node
                    m_preFiles.Clear();
                    m_exeFiles.Clear();
                    m_name = rootChild.Attributes["Name"].Value;
                    m_version = rootChild.Attributes["FileVersion"].Value;

                    foreach (XmlNode child in rootChild.ChildNodes)
                    {
                        //Only EXE, PRE, INCLUDE and BRANCH children nodes
                        if (child.Name == XMLConfig.exeNodeTag) m_exeFiles.Add(child.InnerText);
                        else if (child.Name == XMLConfig.preNodeTag) m_preFiles.Add(child.InnerText);
                        else if (child.Name == XMLConfig.includeNodeTag)
                            loadIncludedDefinitionFile(child.InnerText);
                        else 
                            m_children.Add(ConfigNodeViewModel.getInstance(this,child, m_name));
                            //here we assume definitions are before the children
                    }
                }
            }
        }
        private void loadIncludedDefinitionFile(string appDefinitionFile)
        {
            XmlDocument definitionFile = new XmlDocument();
            definitionFile.Load(appDefinitionFile);
            foreach (XmlNode child in definitionFile.ChildNodes)
            {
                if (child.Name == XMLConfig.definitionNodeTag)
                {
                    foreach (XmlNode definition in child.ChildNodes)
                    {
                        string name = definition.Attributes[XMLConfig.nameAttribute].Value;
                        if (definition.Name == XMLConfig.classDefinitionNodeTag)
                            m_classDefinitions.Add(name, definition);
                        else if (definition.Name == XMLConfig.enumDefinitionNodeTag)
                            m_enumDefinitions.Add(name, definition);
                    }
                }
            }
        }

        //This method can be called to initialize the configuration tree, either providing a configuration file
        //or the default values
        public void init(string configFile= null)
        {
            //Load the config XML file
            XmlDocument configDocument= null;
            if (configFile!=null && configFile!="")
            {
                configDocument = new XmlDocument();
                configDocument.Load(configFile);
            }

            //initialize the config tree
            ConfigNodeViewModel configNode;
            m_children.Clear();
            foreach (XmlNode rootChild in m_configDocument.ChildNodes)
            {
                if (rootChild.Name == XMLConfig.appNodeTag)
                {
                    foreach (XmlNode child in rootChild.ChildNodes)
                    {
                        if (child.Name != XMLConfig.exeNodeTag && child.Name != XMLConfig.includeNodeTag
                            && child.Name != XMLConfig.preNodeTag)
                        {
                            //the base xPath is the app's name
                            configNode = ConfigNodeViewModel.getInstance(this,child, name, configDocument);
                            if (configNode!=null)
                                m_children.Add(configNode);
                        }
                    }
                }
            }
        }

    }
}
