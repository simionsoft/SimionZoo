using System.IO;
using System.Xml;

using Herd.Files;

namespace Badger.ViewModels
{
    public class MultiValuedConfigViewModel: NestedConfigNode
    {
        private string m_className = "";
        private bool m_bOptional;

        public MultiValuedConfigViewModel(ExperimentViewModel parentExperiment, ConfigNodeViewModel parent
            , XmlNode definitionNode, string parentXPath, XmlNode configNode= null, bool initChildren= true)
        {
            CommonInitialization(parentExperiment,parent, definitionNode,parentXPath);

            m_className = definitionNode.Attributes[XMLTags.classAttribute].Value;
            if (definitionNode.Attributes.GetNamedItem(XMLTags.optionalAttribute) != null)
                m_bOptional = definitionNode.Attributes[XMLTags.optionalAttribute].Value == "true";
            else m_bOptional = false;
            
            if (configNode!=null)
            {
                foreach(XmlNode configChild in configNode.ChildNodes)
                {
                    if (configChild.Name==name)
                        children.Add(new MultiValuedItemConfigViewModel(parentExperiment, this,definitionNode, m_xPath, configChild));
                }
            }
            else //default initialization
            {
                if (initChildren && !m_bOptional)
                    children.Add(new MultiValuedItemConfigViewModel(parentExperiment, this,definitionNode, m_xPath));
            }
        }

        public override ConfigNodeViewModel clone()
        {
            MultiValuedConfigViewModel newNestedCopy = getInstance(m_parentExperiment, m_parent
                , nodeDefinition, m_parent.xPath) as MultiValuedConfigViewModel;
            foreach (ConfigNodeViewModel child in children)
            {
                newNestedCopy.children.Add(child.clone());
            }
            return newNestedCopy;
        }

        public void addChild()
        {
            children.Add(new MultiValuedItemConfigViewModel(m_parentExperiment,this, nodeDefinition, m_xPath));
            m_parentExperiment.DoDeferredLoadSteps();
        }
        public void removeChild(MultiValuedItemConfigViewModel child)
        {
            children.Remove(child);
        }

        public override bool Validate()
        {
            if (!m_bOptional && children.Count == 0) return false;
            return base.Validate();
        }

        //the base method is overriden because we don't want the MultiValuedConfigViewModel to add
        //any header or footer... those will be added by the MultiValuedItemConfigViewModel children,
        //only once per child
        public override void outputXML(StreamWriter writer,SaveMode mode,string leftSpace)
        {
            outputChildrenXML(writer,mode,leftSpace);
        }
    }
}
