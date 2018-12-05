using System.Xml;

using Herd.Files;

namespace Badger.ViewModels
{
    class IntegerValueConfigViewModel: ConfigNodeViewModel
    {
        public IntegerValueConfigViewModel(ExperimentViewModel parentExperiment, ConfigNodeViewModel parent, XmlNode definitionNode, string parentXPath, XmlNode configNode = null)
        {
            CommonInitialization(parentExperiment, parent, definitionNode, parentXPath);

            if (configNode == null || configNode[name] == null)
            {
                //default init
                content = definitionNode.Attributes[XMLTags.defaultAttribute].Value;
                textColor = XMLTags.colorDefaultValue;
            }
            else
            {
                //init from config file
                content = configNode[name].InnerText;
            }
        }

        public override ConfigNodeViewModel clone()
        {
            IntegerValueConfigViewModel newInstance =
                new IntegerValueConfigViewModel(m_parentExperiment, m_parent, nodeDefinition, m_parent.xPath);

            newInstance.content = content;
            newInstance.textColor = textColor;
            return newInstance;
        }

        public override bool Validate()
        {
            int parsedValue;
            if (int.TryParse(content, out parsedValue))
                return true;
            return false;
        }

    }
}
