using Simion;
using System.Xml;
using System.Collections.Generic;
using Caliburn.Micro;

namespace Badger.ViewModels
{
    class EnumeratedValueConfigViewModel: ConfigNodeViewModel
    {
        private AppViewModel m_appDefinition;
        private List<string> m_enumeratedType;
        public List<string> enumeratedNames { get { return m_enumeratedType; } }

        public string selectedEnumeratedName
        {
            get { return content; }
            set { content = value; }
        }
        string m_class;
        public EnumeratedValueConfigViewModel(AppViewModel appDefinition, XmlNode definitionNode, string parentXPath, XmlNode configNode = null)
        {
            commonInit(appDefinition, definitionNode, parentXPath);
            comment = definitionNode.Attributes[XMLConfig.commentAttribute].Value;
            m_class = definitionNode.Attributes[XMLConfig.classAttribute].Value;
            m_appDefinition = appDefinition;
            if (configNode != null)
            {
                //init from config file
                m_default= configNode[name].InnerText;
            }

            appDefinition.registerDeferredLoadStep(deferredLoad);
        }

        public void deferredLoad()
        {
            m_enumeratedType = m_appDefinition.getEnumeratedType(m_class);
            content = m_default;
        }

        public override bool validate()
        {
            List<string> enumeration = m_appDefinition.getEnumeratedType(m_class);
            return enumeration.Exists(id => (id==content));
        }

        public override string getXML()
        {
            return "<" + name + ">" + content + "</" + name + ">";
        }
    }
}
