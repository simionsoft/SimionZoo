using Simion;
using System.Xml;


namespace Badger.ViewModels
{
    class IntegerValueConfigViewModel: ConfigNodeViewModel
    {
        private int m_value= 0;
        public int value
        {
            get { return m_value; }
            set { m_value = value; NotifyOfPropertyChange(()=>value); }
        }

        public IntegerValueConfigViewModel(XmlNode definitionNode, string parentXPath, XmlNode configNode = null)
        {
            commonInit(definitionNode, parentXPath);
            comment = definitionNode.Attributes[XMLConfig.commentAttribute].Value;
            if (configNode == null)
            {
                //default init
                value = int.Parse(definitionNode.Attributes[XMLConfig.defaultAttribute].Value);
            }
            else
            {
                //init from config file
                value= int.Parse(configNode[name].InnerText);
            }
        }

        public override string getXML()
        {
            return "<" + name + ">" + value.ToString() + "</" + name + ">";
        }
    }
}
