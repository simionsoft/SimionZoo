using System.Xml;
using Simion;

namespace Badger.ViewModels
{
    class BranchConfigViewModel: NestedConfigNode
    {
        private string m_class = "";
        private string m_window = "";
        private bool m_bOptional = false;

        public BranchConfigViewModel(AppViewModel appDefinition,XmlNode definitionNode, string parentXPath, XmlNode configNode)
        {
            commonInit(definitionNode,parentXPath);

            m_class = definitionNode.Attributes[XMLConfig.classAttribute].Value;
            if (definitionNode.Attributes.GetNamedItem(XMLConfig.windowAttribute) != null)
                m_window = definitionNode.Attributes[XMLConfig.windowAttribute].Value;
            else m_window = "";
            if (definitionNode.Attributes.GetNamedItem(XMLConfig.optionalAttribute) != null)
                m_bOptional = definitionNode.Attributes[XMLConfig.optionalAttribute].Value == "true";
            else m_bOptional = false;

            childrenInit(appDefinition,definitionNode, m_xPath, configNode);
        }

        public override string getXMLHeader(){ return "<" + name + ">"; }
        public override string getXMLFooter() { return "</" + name + ">"; }

    }
}
