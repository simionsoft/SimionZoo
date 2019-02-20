using System.Xml;

using Herd.Files;

namespace Badger.ViewModels
{
    class DoubleValueConfigViewModel : ConfigNodeViewModel
    {
        public DoubleValueConfigViewModel(ExperimentViewModel parentExperiment, ConfigNodeViewModel parent,
            XmlNode definitionNode, string parentXPath, XmlNode configNode = null)
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
            DoubleValueConfigViewModel newInstance =
                new DoubleValueConfigViewModel(m_parentExperiment, m_parent, nodeDefinition, m_parent.xPath);

            newInstance.content = content;
            newInstance.textColor = textColor;
            return newInstance;
        }


        public override bool Validate()
        {
            double parsedValue;
            if (double.TryParse(content, out parsedValue))
                return true;
            return false;
        }
    }
}
