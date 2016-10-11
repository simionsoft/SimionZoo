using System;
using System.Xml;
using Simion;

namespace Badger.ViewModels
{
    public class BranchConfigViewModel: NestedConfigNode
    {
        private string m_className = "";
        private string m_window = "";
        private bool m_bOptional = false;

        public BranchConfigViewModel(AppViewModel appDefinition,XmlNode definitionNode, string parentXPath, XmlNode configNode)
        {
            commonInit(appDefinition,definitionNode,parentXPath);

            m_className = definitionNode.Attributes[XMLConfig.classAttribute].Value;
            if (definitionNode.Attributes.GetNamedItem(XMLConfig.windowAttribute) != null)
                m_window = definitionNode.Attributes[XMLConfig.windowAttribute].Value;
            else m_window = "";
            if (definitionNode.Attributes.GetNamedItem(XMLConfig.optionalAttribute) != null)
                m_bOptional = definitionNode.Attributes[XMLConfig.optionalAttribute].Value == "true";
            else m_bOptional = false;

            childrenInit(appDefinition, appDefinition.getClassDefinition(m_className)
                , m_xPath, configNode);
        }

        public override bool validate()
        {
            foreach (ConfigNodeViewModel child in children)
            {
                bIsValid &= child.validate();
            }
            return bIsValid;
        }

        public override string getXMLHeader(){ return "<" + name + ">"; }
        public override string getXMLFooter() { return "</" + name + ">"; }

    }
}
