using Simion;
using System.Xml;

namespace Badger.ViewModels
{
    class DoubleValueConfigViewModel: ConfigNodeViewModel
    {

        public DoubleValueConfigViewModel(AppViewModel appViewModel, XmlNode definitionNode, string parentXPath, XmlNode configNode = null)
        {
            commonInit(appViewModel,definitionNode, parentXPath);

            if (configNode == null || configNode[name]==null)
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

        public override bool validate()
        {
            double parsedValue;
            if (double.TryParse(content, out parsedValue))
                return true;
            return false;
        }
    }
}
