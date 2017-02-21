using Badger.Simion;
using System.Xml;

namespace Badger.ViewModels
{
    class BoolValueConfigViewModel: ConfigNodeViewModel
    {
        public bool bContentAsBool
        {
            get
            {
                if (content.ToLower() == "true") return true;
                return false;
            }
            set
            {
                if (value) content = "true";
                else content = "false";
            }
        }
        public BoolValueConfigViewModel(ExperimentViewModel parentExperiment, ConfigNodeViewModel parent, XmlNode definitionNode, string parentXPath, XmlNode configNode = null)
        {
            commonInit(parentExperiment, parent, definitionNode, parentXPath);

            if (configNode == null || configNode[name] == null)
            {
                //default init
                content = definitionNode.Attributes[XMLConfig.defaultAttribute].Value;
                textColor = XMLConfig.colorDefaultValue;
            }
            else
            {
                //init from config file
                content = configNode[name].InnerText;
            }
        }

        public override ConfigNodeViewModel clone()
        {
            BoolValueConfigViewModel newInstance =
                new BoolValueConfigViewModel(m_parentExperiment, m_parent, nodeDefinition, m_parent.xPath);

            newInstance.content = content;
            newInstance.textColor = textColor;
            return newInstance;
        }

        public override bool validate()
        {
            bool parsedValue;
            if (bool.TryParse(content, out parsedValue))
                return true;
            return false;
        }

    }
}
