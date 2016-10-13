using Simion;
using System.Xml;
using System.Collections.Generic;
using Caliburn.Micro;

namespace Badger.ViewModels
{
    class XmlDefRefValueConfigViewModel: ConfigNodeViewModel
    {
        private AppViewModel m_appDefinition;
        private List<string> m_enumeratedNames= null;
        public List<string> enumeratedNames
        {
            get { return m_enumeratedNames; }
            set { m_enumeratedNames = value; NotifyOfPropertyChange(() => enumeratedNames); }
        }

        public string selectedEnumeratedName
        {
            get { return content; }
            set {
                content = value;
                NotifyOfPropertyChange(() => selectedEnumeratedName);
            }
        }
        string m_hangingFrom;
        public XmlDefRefValueConfigViewModel(AppViewModel appDefinition, XmlNode definitionNode, string parentXPath, XmlNode configNode = null)
        {
            commonInit(appDefinition, definitionNode, parentXPath);
            System.Console.WriteLine("loading " + name);

            m_hangingFrom = definitionNode.Attributes[XMLConfig.hangingFromAttribute].Value;
            m_appDefinition = appDefinition;

            if (configNode!=null)
            {
                configNode = configNode[name];
                content = configNode.InnerText;
            }

            appDefinition.registerXMLDefRef(updateXMLDefRef);
        }

        public void updateXMLDefRef()
        {
            enumeratedNames = m_appDefinition.getAuxDefinition(m_hangingFrom);
            textColor = XMLConfig.colorInvalidValue;

        }

        public override bool validate()
        {
            List<string> enumeration = m_appDefinition.getAuxDefinition(m_hangingFrom);
            return enumeration.Exists(id => (id==content));
        }
    }
}
