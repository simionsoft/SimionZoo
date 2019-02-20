using System.Xml;

using Herd.Files;


namespace Badger.ViewModels
{
    public class MultiValuedItemConfigViewModel: NestedConfigNode
    {
        private string m_className = "";

        public MultiValuedItemConfigViewModel(ExperimentViewModel parentExperiment,MultiValuedConfigViewModel parent
            ,XmlNode definitionNode, string parentXPath, XmlNode configNode=null)
        {
            CommonInitialization(parentExperiment,parent,definitionNode,parentXPath);

            m_parent = parent;
            m_className = definitionNode.Attributes[XMLTags.classAttribute].Value;

            switch (m_className)
            {
                case XMLTags.integerNodeTag: children.Add(new IntegerValueConfigViewModel(parentExperiment, this,definitionNode, m_xPath, configNode));break;
                case XMLTags.boolNodeTag: children.Add(new BoolValueConfigViewModel(parentExperiment, this, definitionNode, m_xPath, configNode)); break;
                case XMLTags.doubleNodeTag: children.Add(new DoubleValueConfigViewModel(parentExperiment, this,definitionNode, m_xPath, configNode));break ;
                case XMLTags.stringNodeTag: children.Add(new StringValueConfigViewModel(parentExperiment, this, definitionNode, m_xPath, configNode));break ;
                case XMLTags.filePathNodeTag: children.Add(new FilePathValueConfigViewModel(parentExperiment, this,definitionNode, m_xPath, configNode));break ;
                case XMLTags.dirPathNodeTag: children.Add(new DirPathValueConfigViewModel(parentExperiment, this,definitionNode, m_xPath, configNode));break ;
                case XMLTags.stateVarRefTag: children.Add(new WorldVarRefValueConfigViewModel(parentExperiment, WorldVarType.StateVar, parent, definitionNode, parentXPath, configNode)); break;
                case XMLTags.actionVarRefTag: children.Add(new WorldVarRefValueConfigViewModel(parentExperiment, WorldVarType.ActionVar, parent, definitionNode, parentXPath, configNode)); break;
                default:
                    XmlNode classDefinition = parentExperiment.GetClassDefinition(m_className);
                    if (classDefinition != null)
                        childrenInit(parentExperiment, parentExperiment.GetClassDefinition(m_className), m_xPath, configNode);
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
