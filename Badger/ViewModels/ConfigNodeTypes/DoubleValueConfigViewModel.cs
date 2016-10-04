using Simion;
using System.Xml;

namespace Badger.ViewModels
{
    class DoubleValueConfigViewModel: ConfigNodeViewModel
    {
        private double m_value = 0.0;
        public double value
        {
            get { return m_value; }
            set { m_value = value; NotifyOfPropertyChange(() => value); }
        }

        public DoubleValueConfigViewModel(XmlNode definitionNode, string parentXPath, XmlNode configNode = null)
        {
            commonInit(definitionNode, parentXPath);
            comment = definitionNode.Attributes[XMLConfig.commentAttribute].Value;
            if (configNode == null)
            {
                //default init
                value = double.Parse(definitionNode.Attributes[XMLConfig.defaultAttribute].Value);
            }
            else
            {
                //init from config file
                value = double.Parse(configNode[name].InnerText);
            }
        }

        public override string getXML()
        {
            return "<" + name + ">" + value.ToString() + "</" + name + ">";
        }
    }
}
