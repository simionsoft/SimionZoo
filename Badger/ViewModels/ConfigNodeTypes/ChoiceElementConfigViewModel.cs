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
        //private string m_loadXML;

        public ChoiceElementConfigViewModel(AppViewModel appDefinition, ConfigNodeViewModel parent, XmlNode definitionNode
            , string parentXPath, XmlNode configNode= null)
        {
            commonInit(appDefinition,parent,definitionNode,parentXPath);

            m_className = definitionNode.Attributes[XMLConfig.classAttribute].Value;
            if (definitionNode.Attributes.GetNamedItem(XMLConfig.windowAttribute) != null)
                m_window = definitionNode.Attributes[XMLConfig.windowAttribute].Value;
            else m_window = "";
            if (definitionNode.Attributes.GetNamedItem(XMLConfig.optionalAttribute) != null)
                m_bOptional = definitionNode.Attributes[XMLConfig.optionalAttribute].Value == "true";
            else m_bOptional = false;
            //if (definitionNode.Attributes.GetNamedItem(XMLConfig.loadXMLFileAttribute) != null)
            //    m_loadXML = definitionNode.Attributes[XMLConfig.loadXMLFileAttribute].Value;
            //else m_loadXML = "";

            //if (m_loadXML != "")
            //{
            //    m_appViewModel.loadAuxDefinitions(m_loadXML);
            //}

            if (configNode != null)
                configNode = configNode[name];
            //we allow an undefined class ("", most likely) and we just ignore children
            childrenInit(appDefinition, appDefinition.getClassDefinition(m_className,true)
                , m_xPath, configNode);
        }

        //Choice elements cannot be forked, so no need to fork them
        public override ConfigNodeViewModel clone()
        {
            ChoiceElementConfigViewModel newInstance =
                new ChoiceElementConfigViewModel(m_appViewModel, m_parent, nodeDefinition, m_parent.xPath);

            newInstance.m_className = m_className;
            newInstance.m_bOptional = m_bOptional;
            //newInstance.m_loadXML = m_loadXML;
            newInstance.m_window = m_window;
            
            return newInstance;
        }

    }
}
