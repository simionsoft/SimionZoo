using System;
using System.Xml;
using Simion;

namespace Badger.ViewModels
{
    public class MultiValuedConfigViewModel: NestedConfigNode
    {
        private string m_className = "";
        private bool m_bOptional;

        public MultiValuedConfigViewModel(AppViewModel appDefinition,XmlNode definitionNode, string parentXPath, XmlNode configNode)
        {
            commonInit(appDefinition, definitionNode,parentXPath);

            m_className = definitionNode.Attributes[XMLConfig.classAttribute].Value;
            if (definitionNode.Attributes.GetNamedItem(XMLConfig.optionalAttribute) != null)
                m_bOptional = definitionNode.Attributes[XMLConfig.optionalAttribute].Value == "true";
            else m_bOptional = false;
            
            if (configNode!=null)
            {
                foreach(XmlNode child in configNode.ChildNodes)
                {
                    children.Add(new MultiValuedItemConfigViewModel(appDefinition, this,definitionNode, m_xPath, child));
                }
            }
            else //default initialization
            {
                if (!m_bOptional)
                    children.Add(new MultiValuedItemConfigViewModel(appDefinition, this,definitionNode, m_xPath));
            }

        }

        public void addChild()
        {
            children.Add(new MultiValuedItemConfigViewModel(m_appViewModel,this, m_definitionNode, m_xPath));
            m_appViewModel.doDeferredLoadSteps();
        }
        public void removeChild(MultiValuedItemConfigViewModel child)
        {
            children.Remove(child);
        }

        public override bool validate()
        {
            if (!m_bOptional && children.Count == 0) return false;
            return base.validate();
        }
    }
}
