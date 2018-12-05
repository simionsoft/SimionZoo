using System;
using System.Xml;

using Herd.Files;

namespace Badger.ViewModels
{
    public class ChoiceElementConfigViewModel: NestedConfigNode
    {
        private string m_className = "";
        private string m_window = "";
        private bool m_bOptional = false;
        private string m_selectWorld;

        public ChoiceElementConfigViewModel(ExperimentViewModel parentExperiment, ConfigNodeViewModel parent, XmlNode definitionNode
            , string parentXPath, XmlNode configNode= null)
        {
            CommonInitialization(parentExperiment,parent,definitionNode,parentXPath);

            m_className = definitionNode.Attributes[XMLTags.classAttribute].Value;
            if (definitionNode.Attributes.GetNamedItem(XMLTags.windowAttribute) != null)
                m_window = definitionNode.Attributes[XMLTags.windowAttribute].Value;
            else m_window = "";
            if (definitionNode.Attributes.GetNamedItem(XMLTags.optionalAttribute) != null)
                m_bOptional = definitionNode.Attributes[XMLTags.optionalAttribute].Value == "true";
            else m_bOptional = false;
            if (definitionNode.Attributes.GetNamedItem(XMLTags.badgerMetadataAttribute)!=null)
            {
                //BadgerMetadata="World=Wind-turbine"
                string metadata = definitionNode.Attributes[XMLTags.badgerMetadataAttribute].Value;
                if (metadata.StartsWith("World="))
                    m_selectWorld= metadata.Remove(0,"World=".Length);

                m_parentExperiment.SelectWorld(m_selectWorld);
            }


            if (configNode != null)
                configNode = configNode[name];
            //we allow an undefined class ("", most likely) and we just ignore children
            childrenInit(parentExperiment, parentExperiment.GetClassDefinition(m_className,true)
                , m_xPath, configNode);
        }

        //Choice elements cannot be forked, so no need to fork them
        public override ConfigNodeViewModel clone()
        {
            ChoiceElementConfigViewModel newInstance =
                new ChoiceElementConfigViewModel(m_parentExperiment, m_parent, nodeDefinition, m_parent.xPath);

            newInstance.m_className = m_className;
            newInstance.m_bOptional = m_bOptional;
            newInstance.m_window = m_window;
            
            return newInstance;
        }

    }
}
