using Simion;
using System.Xml;

namespace Badger.ViewModels
{
    class DoubleValueConfigViewModel: ConfigNodeViewModel
    {

        public DoubleValueConfigViewModel(XmlNode definitionNode, string parentXPath, XmlNode configNode = null)
        {
            commonInit(definitionNode, parentXPath);
            comment = definitionNode.Attributes[XMLConfig.commentAttribute].Value;
            if (configNode == null)
            {
                //default init
                value = definitionNode.Attributes[XMLConfig.defaultAttribute].Value;
            }
            else
            {
                //init from config file
                value = configNode[name].InnerText;
            }
        }

        public override bool validate()
        {
            double parsedValue;
            if (double.TryParse(value, out parsedValue))
                return true;
            return false;
        }

        public override string getXML()
        {
            return "<" + name + ">" + value.ToString() + "</" + name + ">";
        }
    }
}
