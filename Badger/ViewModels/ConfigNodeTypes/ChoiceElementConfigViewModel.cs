using System;
using System.Xml;
using Simion;

namespace Badger.ViewModels
{
    public class ChoiceElementConfigViewModel: NestedConfigNode
    {
        private string m_className = "";
        private string m_window = "";
        private bool m_bOptional = false;
        private string m_loadXML;

        public ChoiceElementConfigViewModel(AppViewModel appDefinition,XmlNode definitionNode, string parentXPath, XmlNode configNode)
        {
            commonInit(appDefinition,definitionNode,parentXPath);

            m_className = definitionNode.Attributes[XMLConfig.classAttribute].Value;
            if (definitionNode.Attributes.GetNamedItem(XMLConfig.windowAttribute) != null)
                m_window = definitionNode.Attributes[XMLConfig.windowAttribute].Value;
            else m_window = "";
            if (definitionNode.Attributes.GetNamedItem(XMLConfig.optionalAttribute) != null)
                m_bOptional = definitionNode.Attributes[XMLConfig.optionalAttribute].Value == "true";
            else m_bOptional = false;
            if (definitionNode.Attributes.GetNamedItem(XMLConfig.loadXMLFileAttribute) != null)
                m_loadXML = definitionNode.Attributes[XMLConfig.loadXMLFileAttribute].Value;
            else m_loadXML = "";

            childrenInit(appDefinition, appDefinition.getClassDefinition(m_className)
                , m_xPath, configNode);
        }


        public override void setSelected()
        {
            if (m_loadXML != "")
            {
                m_appViewModel.loadAuxDefinitions(m_loadXML);
            }
        }

    }
}
