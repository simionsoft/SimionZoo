using Simion;
using System.Xml;

namespace Badger.ViewModels
{
    class StringValueConfigViewModel: ConfigNodeViewModel
    {
        public StringValueConfigViewModel(XmlNode definitionNode, string parentXPath, XmlNode configNode = null)
        {
            commonInit(definitionNode, parentXPath);
            comment = definitionNode.Attributes[XMLConfig.commentAttribute].Value;
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
            return content!="";
        }

        public override string getXML()
        {
            return "<" + name + ">" + content + "</" + name + ">";
        }
    }
}
