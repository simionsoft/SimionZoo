using System.Collections.Generic;
using System.Xml;
using Caliburn.Micro;
using Simion;

namespace Badger.ViewModels
{
    abstract public class ConfigNodeViewModel: PropertyChangedBase
    {
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
            name = definitionNode.Attributes[XMLConfig.nameAttribute].Value;
            xPath = parentXPath + "/" + name;
        }


        //FACTORY
        public static ConfigNodeViewModel getInstance(AppViewModel appDefinition,XmlNode definitionNode, string parentXPath, XmlNode configNode= null)
        {
            switch (definitionNode.Name)
            {
                case XMLConfig.integerNodeTag: return new IntegerValueConfigViewModel(definitionNode,parentXPath,configNode);
                    //get parent from caller and spread it to be able to get definitions
                case XMLConfig.doubleNodeTag: return new DoubleValueConfigViewModel(definitionNode,parentXPath,configNode);
                case XMLConfig.branchNodeTag: return new BranchConfigViewModel(appDefinition,definitionNode,parentXPath,configNode);
            }

            return null;
        }
    }
    abstract class NestedConfigNode: ConfigNodeViewModel
    {
        //Children
        protected List<ConfigNodeViewModel> m_children;

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

        protected void childrenInit(AppViewModel appDefinition, XmlNode definitionNode, string parentXPath, XmlNode configNode= null)
        {
            foreach (XmlNode child in definitionNode.ChildNodes)
            {
                ConfigNodeViewModel childNode = ConfigNodeViewModel.getInstance(appDefinition, child, parentXPath, configNode);
                m_children.Add(childNode);
            }
        }
    }
}
