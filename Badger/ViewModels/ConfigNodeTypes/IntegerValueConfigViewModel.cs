using Simion;
using System.Xml;

namespace Badger.ViewModels
{
    class IntegerValueConfigViewModel: ConfigNodeViewModel
    {
        public IntegerValueConfigViewModel(AppViewModel appViewModel, XmlNode definitionNode, string parentXPath, XmlNode configNode = null)
        {
            commonInit(appViewModel, definitionNode, parentXPath);

            if (configNode == null)
            {
                //default init
                content = definitionNode.Attributes[XMLConfig.defaultAttribute].Value;
            }
            else
            {
                //init from config file
                content= configNode[name].InnerText;
            }
        }

        public override bool validate()
        {
            int parsedValue;
            if (int.TryParse(content, out parsedValue))
                return true;
            return false;
        }

        public override string getXML()
        {
            return "<" + name + ">" + content + "</" + name + ">";
        }
    }
}
