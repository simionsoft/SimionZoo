using System.Collections.Generic;
using System.Xml;
using Caliburn.Micro;
using Simion;

namespace Badger.ViewModels
{
    abstract public class ConfigNodeViewModel: PropertyChangedBase
    {
        protected string m_default = "";
        private string m_content = "";
        public string content
        {
            get { return m_content; }
            set { m_content = value; m_bIsValid= validate();
                NotifyOfPropertyChange(() => bIsValid);
                NotifyOfPropertyChange(() => content); }
        }

        private string m_textColor = "Black";
        public string textColor { get { return m_textColor; }set { m_textColor = value; NotifyOfPropertyChange(() => textColor); } }

        abstract public bool validate();

        static public XmlNode nodeDefinition { get; set; }

        //Comment
        private string m_comment= "";
        public string comment { get { return m_comment; } set { m_comment = value; } }

        //Validation
        private bool m_bIsValid = false;
        public bool bIsValid { get { return m_bIsValid; } set { m_bIsValid = value; } }

        //XML output methods
        public abstract string getXML();

        //XPath methods
        protected string m_xPath;
        public string xPath { get{ return m_xPath; }  set { m_xPath = value; } }

        //Name
        private string m_name;
        public string name { get { return m_name; } set { m_name = value; } }
        
        //Initialization stuff common to all types of configuration nodes
        protected void commonInit(XmlNode definitionNode, string parentXPath)
        {
            nodeDefinition = definitionNode;
            name = definitionNode.Attributes[XMLConfig.nameAttribute].Value;
            xPath = parentXPath + "/" + name;
            if (definitionNode.Attributes.GetNamedItem(XMLConfig.defaultAttribute)!=null)
            {
                m_default = definitionNode.Attributes[XMLConfig.defaultAttribute].Value;
            }
        }


        //FACTORY
        public static ConfigNodeViewModel getInstance(AppViewModel appDefinition,XmlNode definitionNode, string parentXPath, XmlNode configNode= null)
        {
            switch (definitionNode.Name)
            {
                case XMLConfig.integerNodeTag: return new IntegerValueConfigViewModel(definitionNode,parentXPath,configNode);
                case XMLConfig.doubleNodeTag: return new DoubleValueConfigViewModel(definitionNode,parentXPath,configNode);
                case XMLConfig.stringNodeTag: return new StringValueConfigViewModel(definitionNode, parentXPath, configNode);
                case XMLConfig.filePathNodeTag: return new FilePathValueConfigViewModel(definitionNode, parentXPath, configNode);
                case XMLConfig.dirPathNodeTag: return new DirPathValueConfigViewModel(definitionNode, parentXPath, configNode);

                case XMLConfig.branchNodeTag: return new BranchConfigViewModel(appDefinition,definitionNode,parentXPath,configNode);
                case XMLConfig.choiceNodeTag: return new ChoiceConfigViewModel(appDefinition, definitionNode, parentXPath, configNode);
                case XMLConfig.choiceElementNodeTag: return new ChoiceElementConfigViewModel(appDefinition, definitionNode, parentXPath, configNode);
            }

            return null;
        }
    }
    abstract public class NestedConfigNode: ConfigNodeViewModel
    {
        //Children
        protected BindableCollection<ConfigNodeViewModel> m_children= new BindableCollection<ConfigNodeViewModel>();
        public BindableCollection<ConfigNodeViewModel> children { get { return m_children; }
            set { m_children = value; NotifyOfPropertyChange(() => children); } }

        public override string getXML()
        { return getXMLHeader() + getChildrenXML() + getXMLFooter(); }

        public string getChildrenXML()
        {
            string xml = "";
            foreach (ConfigNodeViewModel child in m_children) xml += child.getXML();
            return xml;
        }
        public abstract string getXMLHeader();
        public abstract string getXMLFooter();

        protected void childrenInit(AppViewModel appDefinition, XmlNode classDefinition, string parentXPath, XmlNode configNode= null)
        {
            if (classDefinition != null)
            {
                foreach (XmlNode child in classDefinition.ChildNodes)
                {
                    ConfigNodeViewModel childNode = ConfigNodeViewModel.getInstance(appDefinition, child, parentXPath, configNode);
                    if (childNode!=null)
                        m_children.Add(childNode);
                }
            }
        }
    }
}
