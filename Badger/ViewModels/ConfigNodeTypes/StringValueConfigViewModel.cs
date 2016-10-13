using Simion;
using System.Xml;

namespace Badger.ViewModels
{
    class StringValueConfigViewModel: ConfigNodeViewModel
    {
        public StringValueConfigViewModel(AppViewModel appDefinition, XmlNode definitionNode, string parentXPath, XmlNode configNode = null)
        {
            commonInit(appDefinition, definitionNode, parentXPath);
            System.Console.WriteLine("loading " + name);

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

        public override bool validate()
        {
            return content!="";
        }
    }
}
