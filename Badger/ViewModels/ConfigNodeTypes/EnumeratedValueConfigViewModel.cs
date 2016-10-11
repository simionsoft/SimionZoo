using Simion;
using System.Xml;
using System.Collections.Generic;
using Caliburn.Micro;

namespace Badger.ViewModels
{
    class EnumeratedValueConfigViewModel: ConfigNodeViewModel
    {
        private AppViewModel m_appDefinition;
        private List<string> m_enumeratedNames;
        public List<string> enumeratedNames
        {
            get { return m_enumeratedNames; }
            set { m_enumeratedNames = value; NotifyOfPropertyChange(() => enumeratedNames); }
        }

        public string selectedEnumeratedName
        {
            get { return content; }
            set { content = value; }
        }
        string m_class;
        public EnumeratedValueConfigViewModel(AppViewModel appDefinition, XmlNode definitionNode, string parentXPath, XmlNode configNode = null)
        {
            commonInit(appDefinition, definitionNode, parentXPath);

            m_class = definitionNode.Attributes[XMLConfig.classAttribute].Value;
            m_appDefinition = appDefinition;
            if (configNode != null)
            {
                //init from config file
                content = configNode[name].InnerText;
            }
            else
            {
                content = m_default;
                textColor = XMLConfig.colorDefaultValue;
            }

            appDefinition.registerDeferredLoadStep(deferredLoad);
        }

        public void deferredLoad()
        {
            enumeratedNames = m_appDefinition.getEnumeratedType(m_class);
            //content = m_default;
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
