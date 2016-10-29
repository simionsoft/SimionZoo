using System;
using System.Xml;
using Simion;

namespace Badger.ViewModels
{
    public class MultiValuedItemConfigViewModel: NestedConfigNode
    {
        private string m_className = "";

        public MultiValuedItemConfigViewModel(AppViewModel appDefinition,MultiValuedConfigViewModel parent
            ,XmlNode definitionNode, string parentXPath, XmlNode configNode=null)
        {
            commonInit(appDefinition,parent,definitionNode,parentXPath);

            m_parent = parent;
            m_className = definitionNode.Attributes[XMLConfig.classAttribute].Value;

            switch (m_className)
            {
                case XMLConfig.integerNodeTag: children.Add(new IntegerValueConfigViewModel(appDefinition, this,definitionNode, m_xPath, configNode));break;
                case XMLConfig.doubleNodeTag: children.Add(new DoubleValueConfigViewModel(appDefinition, this,definitionNode, m_xPath, configNode));break ;
                case XMLConfig.stringNodeTag: children.Add(new StringValueConfigViewModel(appDefinition, this, definitionNode, m_xPath, configNode));break ;
                case XMLConfig.filePathNodeTag: children.Add(new FilePathValueConfigViewModel(appDefinition, this,definitionNode, m_xPath, configNode));break ;
                case XMLConfig.dirPathNodeTag: children.Add(new DirPathValueConfigViewModel(appDefinition, this,definitionNode, m_xPath, configNode));break ;
                default:
                    XmlNode classDefinition = appDefinition.getClassDefinition(m_className);
                    if (classDefinition != null)
                        childrenInit(appDefinition, appDefinition.getClassDefinition(m_className), m_xPath, configNode);
                    break;
            }
            
        }
        public void remove()
        {
            MultiValuedConfigViewModel parent = m_parent as MultiValuedConfigViewModel;
            if (parent != null)
                parent.removeChild(this);
        }

        public override ConfigNodeViewModel clone()
        {
            MultiValuedItemConfigViewModel newItem = new MultiValuedItemConfigViewModel(m_appViewModel
                , parent as MultiValuedConfigViewModel
                , nodeDefinition, parent.xPath);
            foreach (ConfigNodeViewModel child in children)
            {
                ConfigNodeViewModel clonedChild = child.clone();
                clonedChild.parent = newItem;
                newItem.children.Add(clonedChild);
            }
            return newItem;
        }

    }
}
