using System;
using System.Xml;
using Badger.Simion;

namespace Badger.ViewModels
{
    public class MultiValuedItemConfigViewModel: NestedConfigNode
    {
        private string m_className = "";

        public MultiValuedItemConfigViewModel(ExperimentViewModel parentExperiment,MultiValuedConfigViewModel parent
            ,XmlNode definitionNode, string parentXPath, XmlNode configNode=null)
        {
            commonInit(parentExperiment,parent,definitionNode,parentXPath);

            m_parent = parent;
            m_className = definitionNode.Attributes[XMLConfig.classAttribute].Value;

            switch (m_className)
            {
                case XMLConfig.integerNodeTag: children.Add(new IntegerValueConfigViewModel(parentExperiment, this,definitionNode, m_xPath, configNode));break;
                case XMLConfig.boolNodeTag: children.Add(new BoolValueConfigViewModel(parentExperiment, this, definitionNode, m_xPath, configNode)); break;
                case XMLConfig.doubleNodeTag: children.Add(new DoubleValueConfigViewModel(parentExperiment, this,definitionNode, m_xPath, configNode));break ;
                case XMLConfig.stringNodeTag: children.Add(new StringValueConfigViewModel(parentExperiment, this, definitionNode, m_xPath, configNode));break ;
                case XMLConfig.filePathNodeTag: children.Add(new FilePathValueConfigViewModel(parentExperiment, this,definitionNode, m_xPath, configNode));break ;
                case XMLConfig.dirPathNodeTag: children.Add(new DirPathValueConfigViewModel(parentExperiment, this,definitionNode, m_xPath, configNode));break ;
                default:
                    XmlNode classDefinition = parentExperiment.getClassDefinition(m_className);
                    if (classDefinition != null)
                        childrenInit(parentExperiment, parentExperiment.getClassDefinition(m_className), m_xPath, configNode);
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
            MultiValuedItemConfigViewModel newItem = new MultiValuedItemConfigViewModel(m_parentExperiment
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
