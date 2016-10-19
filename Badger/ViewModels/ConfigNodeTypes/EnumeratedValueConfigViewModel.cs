using Simion;
using System.Xml;
using System.Collections.Generic;
using Caliburn.Micro;

namespace Badger.ViewModels
{
    class EnumeratedValueConfigViewModel: ConfigNodeViewModel
    {
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
        public EnumeratedValueConfigViewModel(AppViewModel appDefinition, ConfigNodeViewModel parent, XmlNode definitionNode, string parentXPath, XmlNode configNode = null)
        {
            commonInit(appDefinition, parent, definitionNode, parentXPath);

            m_class = definitionNode.Attributes[XMLConfig.classAttribute].Value;
            enumeratedNames = m_appViewModel.getEnumeratedType(m_class);

            if (configNode == null || configNode[name] == null)
            {
                content = m_default;
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
            List<string> enumeration = m_appViewModel.getEnumeratedType(m_class);
            return enumeration.Exists(id => (id==content));
        }
    }
}
