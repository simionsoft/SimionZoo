using System;
using System.Xml;
using Simion;

namespace Badger.ViewModels
{
    public class BranchConfigViewModel: NestedConfigNode
    {
        private string m_className = "";
        private string m_window = "";

        private bool m_bIsOptional = false;
        public bool bIsOptional
        {
            get { return m_bIsOptional; }
            set { m_bIsOptional = value; NotifyOfPropertyChange(() => bIsOptional); }
        }

        private bool m_bIsUsed = false;
        public bool bIsUsed
        {
            get { return !bIsOptional || m_bIsUsed; }
            set { m_bIsUsed = value; NotifyOfPropertyChange(() => bIsUsed); }
        }

        public BranchConfigViewModel(AppViewModel appDefinition,XmlNode definitionNode, string parentXPath, XmlNode configNode)
        {
            commonInit(appDefinition,definitionNode,parentXPath);

            m_className = definitionNode.Attributes[XMLConfig.classAttribute].Value;
            if (definitionNode.Attributes.GetNamedItem(XMLConfig.windowAttribute) != null)
                m_window = definitionNode.Attributes[XMLConfig.windowAttribute].Value;
            else m_window = "";
            if (definitionNode.Attributes.GetNamedItem(XMLConfig.optionalAttribute) != null)
                bIsOptional = definitionNode.Attributes[XMLConfig.optionalAttribute].Value == "true";
            else bIsOptional = false;

            if (configNode != null)
            {
                configNode = configNode[name];
                if (configNode != null && bIsOptional)
                    bIsUsed = true;
            }
            else if (bIsOptional)
                bIsUsed = true;

            childrenInit(appDefinition, appDefinition.getClassDefinition(m_className)
                , m_xPath, configNode);
        }

        public override bool validate()
        {
            if (bIsOptional && !bIsUsed) return true; // don't need to validate children if its optional and not used

            foreach (ConfigNodeViewModel child in children)
                if (!child.validate())
                {
                    //Console.WriteLine(child.name);
                    return false;
                }
            return true;
        }

        public override string getXML(string leftSpace)
        {
            if (bIsUsed)
                return base.getXML(leftSpace);
            return "";
        }
    }
}
