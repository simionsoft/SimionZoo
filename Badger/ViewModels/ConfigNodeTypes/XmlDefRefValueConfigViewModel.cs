using Simion;
using System.Xml;
using System.Collections.Generic;
using Caliburn.Micro;

namespace Badger.ViewModels
{
    class XmlDefRefValueConfigViewModel: ConfigNodeViewModel
    {
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
        private string m_hangingFrom;

        public XmlDefRefValueConfigViewModel(AppViewModel appDefinition, XmlNode definitionNode, string parentXPath, XmlNode configNode = null)
        {
            commonInit(appDefinition, definitionNode, parentXPath);

            m_hangingFrom = definitionNode.Attributes[XMLConfig.hangingFromAttribute].Value;

            if (configNode!=null)
            {
                configNode = configNode[name];
                selectedEnumeratedName = configNode.InnerText;
                //the xml definition file may not be yet loaded. We defer this step
                m_appViewModel.registerDeferredLoadStep(updateXMLDefRef);
            }

            m_appViewModel.registerXMLDefRef(updateXMLDefRef);
        }

        public void updateXMLDefRef()
        {
            enumeratedNames = m_appViewModel.getAuxDefinition(m_hangingFrom);
        }

        public override bool validate()
        {
            List<string> enumeration = m_appViewModel.getAuxDefinition(m_hangingFrom);
            return enumeration.Exists(id => (id==content));
        }
    }
}
