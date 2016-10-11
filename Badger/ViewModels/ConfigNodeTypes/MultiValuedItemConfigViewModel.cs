using System;
using System.Xml;
using Simion;

namespace Badger.ViewModels
{
    public class MultiValuedItemConfigViewModel: NestedConfigNode
    {
        private string m_className = "";
        private MultiValuedConfigViewModel m_parent;

        public MultiValuedItemConfigViewModel(AppViewModel appDefinition,MultiValuedConfigViewModel parent
            ,XmlNode definitionNode, string parentXPath, XmlNode configNode=null)
        {
            commonInit(appDefinition,definitionNode,parentXPath);
            m_parent = parent;
            m_className = definitionNode.Attributes[XMLConfig.classAttribute].Value;

            switch (m_className)
            {
                case XMLConfig.integerNodeTag: children.Add(new IntegerValueConfigViewModel(appDefinition, definitionNode, m_xPath, configNode));break;
                case XMLConfig.doubleNodeTag: children.Add(new DoubleValueConfigViewModel(appDefinition, definitionNode, m_xPath, configNode));break ;
                case XMLConfig.stringNodeTag: children.Add(new StringValueConfigViewModel(appDefinition, definitionNode, m_xPath, configNode));break ;
                case XMLConfig.filePathNodeTag: children.Add(new FilePathValueConfigViewModel(appDefinition, definitionNode, m_xPath, configNode));break ;
                case XMLConfig.dirPathNodeTag: children.Add(new DirPathValueConfigViewModel(appDefinition, definitionNode, m_xPath, configNode));break ;
                default:
                    XmlNode classDefinition = appDefinition.getClassDefinition(m_className);
                    if (classDefinition != null)
                        childrenInit(appDefinition, appDefinition.getClassDefinition(m_className), m_xPath, configNode);
                    break;
            }
            
        }
        public void remove()
        {
            if (m_parent != null)
                m_parent.removeChild(this);
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
