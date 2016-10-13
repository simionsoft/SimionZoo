using System.Collections.Generic;
using System.Xml;
using Caliburn.Micro;
using Simion;

namespace Badger.ViewModels
{
    abstract public class ConfigNodeViewModel: PropertyChangedBase
    {
        //access to the root node
        protected AppViewModel m_appViewModel;
        protected XmlNode m_definitionNode;

        protected string m_default = "";
        private string m_content = "";
        public string content
        {
            get { return m_content; }
            set {
                m_content = value;
                bIsValid= validate();
                NotifyOfPropertyChange(() => content);
            }
        }

        private string m_textColor = XMLConfig.colorDefaultValue;
        public string textColor { get { return m_textColor; }
            set { m_textColor = value; NotifyOfPropertyChange(() => textColor); } }

        abstract public bool validate();

        //Comment
        private string m_comment= "";
        public string comment { get { return m_comment; } set { m_comment = value; } }

        //Validation
        private bool m_bIsValid = false;
        public bool bIsValid
        {
            get { return m_bIsValid; }
            set
            {
                m_bIsValid = value;
                if (m_bIsValid) textColor = XMLConfig.colorValidValue;
                else textColor = XMLConfig.colorInvalidValue;
                NotifyOfPropertyChange(() => bIsValid);
            }
        }

        //XML output methods
        public virtual string getXML(string leftSpace)
        {
            return leftSpace + "<" + name + ">" + content + "</" + name + ">\n";
        }

        //XPath methods
        protected string m_xPath;
        public string xPath { get{ return m_xPath; }  set { m_xPath = value; } }

        //Name
        private string m_name;
        public string name { get { return m_name; } set { m_name = value; } }
        
        //Initialization stuff common to all types of configuration nodes
        protected void commonInit(AppViewModel appViewModel,XmlNode definitionNode, string parentXPath)
        {
            m_appViewModel = appViewModel;
            m_definitionNode = definitionNode;
            name = definitionNode.Attributes[XMLConfig.nameAttribute].Value;
            xPath = parentXPath + "/" + name;
            if (definitionNode.Attributes.GetNamedItem(XMLConfig.defaultAttribute)!=null)
            {
                m_default = definitionNode.Attributes[XMLConfig.defaultAttribute].Value;
            }
            if (definitionNode.Attributes.GetNamedItem(XMLConfig.commentAttribute) != null)
            {
                comment = definitionNode.Attributes[XMLConfig.commentAttribute].Value;
            }   
        }


        //FACTORY
        public static ConfigNodeViewModel getInstance(AppViewModel appDefinition,XmlNode definitionNode, string parentXPath, XmlNode configNode= null)
        {
            switch (definitionNode.Name)
            {
                case XMLConfig.integerNodeTag: return new IntegerValueConfigViewModel(appDefinition, definitionNode,parentXPath,configNode);
                case XMLConfig.doubleNodeTag: return new DoubleValueConfigViewModel(appDefinition, definitionNode, parentXPath,configNode);
                case XMLConfig.stringNodeTag: return new StringValueConfigViewModel(appDefinition, definitionNode, parentXPath, configNode);
                case XMLConfig.filePathNodeTag: return new FilePathValueConfigViewModel(appDefinition, definitionNode, parentXPath, configNode);
                case XMLConfig.dirPathNodeTag: return new DirPathValueConfigViewModel(appDefinition, definitionNode, parentXPath, configNode);
                case XMLConfig.xmlRefNodeTag: return new XmlDefRefValueConfigViewModel(appDefinition, definitionNode, parentXPath, configNode);

                case XMLConfig.branchNodeTag: return new BranchConfigViewModel(appDefinition,definitionNode,parentXPath,configNode);
                case XMLConfig.choiceNodeTag: return new ChoiceConfigViewModel(appDefinition, definitionNode, parentXPath, configNode);
                case XMLConfig.choiceElementNodeTag: return new ChoiceElementConfigViewModel(appDefinition, definitionNode, parentXPath, configNode);
                case XMLConfig.enumNodeTag: return new EnumeratedValueConfigViewModel(appDefinition, definitionNode, parentXPath, configNode);
                case XMLConfig.multiValuedNodeTag: return new MultiValuedConfigViewModel(appDefinition, definitionNode, parentXPath, configNode);
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

        public override string getXML(string leftSpace)
        { return leftSpace + getXMLHeader() + getChildrenXML(leftSpace + "  ") + leftSpace + getXMLFooter(); }

        public string getChildrenXML(string leftSpace)
        {
            string xml = "";
            foreach (ConfigNodeViewModel child in m_children) xml += child.getXML(leftSpace);
            return xml;
        }
        public virtual string getXMLHeader() { return "<" + name + ">\n"; }
        public virtual string getXMLFooter() { return "</" + name + ">\n"; }

        protected void childrenInit(AppViewModel appViewModel, XmlNode classDefinition
            , string parentXPath, XmlNode configNode= null)
        {
            if (classDefinition != null)
            {
                foreach (XmlNode child in classDefinition.ChildNodes)
                {
                    ConfigNodeViewModel childNode = ConfigNodeViewModel.getInstance(appViewModel, child, parentXPath, configNode);
                    if (childNode!=null)
                        children.Add(childNode);
                }
            }
        }
        public override bool validate()
        {
            bIsValid = true;
            foreach (ConfigNodeViewModel child in children)
            {
                if (!child.validate())
                {
                    bIsValid = false;
                }
            }
            return bIsValid;
        }
    }
}
