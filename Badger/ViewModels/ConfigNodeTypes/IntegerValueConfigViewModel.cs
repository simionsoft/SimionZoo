using Simion;
using System.Xml;
using System;

namespace Badger.ViewModels
{
    class IntegerValueConfigViewModel: ConfigNodeViewModel
    {
        public IntegerValueConfigViewModel(XmlNode definitionNode, string parentXPath, XmlNode configNode = null)
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
                value= configNode[name].InnerText;
            }
        }

        public override bool validate()
        {
            int parsedValue;
            if (int.TryParse(value, out parsedValue))
                return true;
            return false;
        }

        public override string getXML()
        {
            return "<" + name + ">" + value + "</" + name + ">";
        }
    }
}
