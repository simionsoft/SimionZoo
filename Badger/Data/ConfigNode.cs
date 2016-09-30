using System;
using System.Collections.Generic;
using System.Xml;
using Badger.ViewModels;

namespace Simion
{
    abstract class ConfigNode
    {
        static public XmlNode nodeDefinition { get; set; }

        //Comment
        private string m_comment= "";
        public string comment { get { return m_comment; } set { m_comment = value; } }

        //Validation
        private bool m_bIsValid = false;
        public bool bIsValid { get { return m_bIsValid; } set { m_bIsValid = value; } }

        //XML output methods
        abstract public string getXML();

        //XPath methods
        private string m_xPath;
        public string xPath { get{ return m_xPath; }  set { m_xPath = value; } }

        //Name
        private string m_name;
        public string name { get { return m_name; } set { m_name = value; } }
        
        //Constructors
        abstract public void loadClass(XmlNode configNode);


        protected void commonInit(XmlNode definitionNode)
        {
            comment = definition.Attributes[XMLConfig.commentAttribute].Value;
            name = definition.Attributes[XMLConfig.nameAttribute].Value;
        }


        //FACTORY
        public static ConfigNode getInstance(XmlNode definitionNode)
        {
            foreach (XmlNode child in definitionNode.ChildNodes)
            {
                switch (child.Name)
                {
                    case XMLConfig.integerNodeTag: return new IntegerValueConfigViewModel(child);
                }
            }
            return null;
        }
    }
    abstract class NestedConfigNode: ConfigNode
    {
        //Children
        protected List<ConfigNode> m_children;

        public override string getXML()
        { return getXMLHeader() + getChildrenXML() + getXMLFooter(); }

        public string getChildrenXML()
        {
            string xml = "";
            foreach (ConfigNode child in m_children) xml += child.getXML();
            return xml;
        }
        public abstract string getXMLHeader();
        public abstract string getXMLFooter();

        protected void initChildren(XmlNode definition)
        {
            foreach(XmlNode child in definition.ChildNodes)
            {
                m_children.Add(ConfigNode.getInstance(child));
            }
        }
    }
}
