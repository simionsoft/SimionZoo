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

        public XmlDefRefValueConfigViewModel(AppViewModel appDefinition, ConfigNodeViewModel parent, XmlNode definitionNode, string parentXPath, XmlNode configNode = null)
        {
            commonInit(appDefinition, parent, definitionNode, parentXPath);

            m_hangingFrom = definitionNode.Attributes[XMLConfig.hangingFromAttribute].Value;

            if (configNode != null)
            {
                configNode = configNode[name];
                selectedEnumeratedName = configNode.InnerText;
            }

            //the xml definition file may not be yet loaded
            enumeratedNames = m_appViewModel.getAuxDefinition(m_hangingFrom);

            if (enumeratedNames == null)
            {
                //Either we have loaded the config but the list is of values has not yet been loaded
                //or no config file has been loaded. In Either case, we register for a deferred load step
                m_appViewModel.registerDeferredLoadStep(updateXMLDefRef);
            }

            m_appViewModel.registerXMLDefRef(updateXMLDefRef);
        }

        public override ConfigNodeViewModel clone()
        {
            XmlDefRefValueConfigViewModel newInstance=
                new XmlDefRefValueConfigViewModel(m_appViewModel, m_parent, nodeDefinition, m_parent.xPath);
            m_appViewModel.registerXMLDefRef(newInstance.updateXMLDefRef);
            newInstance.m_hangingFrom = m_hangingFrom;
            newInstance.enumeratedNames = enumeratedNames;
            newInstance.selectedEnumeratedName = selectedEnumeratedName;
            return newInstance;
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
