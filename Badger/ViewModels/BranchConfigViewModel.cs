using System.Xml;
using Simion;

namespace Badger.ViewModels
{
    class BranchConfigViewModel: NestedConfigNode
    {
        private string m_class = "";
        private string m_window = "";
        public BranchConfigViewModel(XmlNode definition)
        {
            commonInit(definition);

            m_class = definition.Attributes[XMLConfig.classAttribute].Value;
            m_window = definition.Attributes[XMLConfig.windowAttribute].Value;

            initChildren(definition);
        }

        public string getXMLHeader()
        {
            return "<" + XMLConfig.branchNodeTag + " " + XMLConfig.nameAttribute + "=\"" + name
                + "\" " + XMLConfig.;
        }
    }
}
