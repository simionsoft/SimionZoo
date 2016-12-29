using Simion;
using System.Xml;
using System.Collections.Generic;
using Caliburn.Micro;

namespace Badger.ViewModels
{
    class WorldVarRefValueConfigViewModel: ConfigNodeViewModel
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
        
        private WorldVarType m_varType;

        public WorldVarRefValueConfigViewModel(AppViewModel appDefinition, WorldVarType varType, ConfigNodeViewModel parent, XmlNode definitionNode, string parentXPath, XmlNode configNode = null)
        {
            commonInit(appDefinition, parent, definitionNode, parentXPath);

            m_varType = varType;//definitionNode.Attributes[XMLConfig.hangingFromAttribute].Value;

            if (configNode != null)
            {
                configNode = configNode[name];
                selectedEnumeratedName = configNode.InnerText;
            }

            //the xml definition file may not be yet loaded
            enumeratedNames = m_appViewModel.getAuxDefinition(m_varType);

            if (enumeratedNames == null)
            {
                //Either we have loaded the config but the list is of values has not yet been loaded
                //or no config file has been loaded. In Either case, we register for a deferred load step
                m_appViewModel.registerDeferredLoadStep(updateValues);
            }

            m_appViewModel.registerXMLDefRef(updateValues);
        }

        public override ConfigNodeViewModel clone()
        {
            WorldVarRefValueConfigViewModel newInstance=
                new WorldVarRefValueConfigViewModel(m_appViewModel, m_varType,m_parent, nodeDefinition, m_parent.xPath);
            m_appViewModel.registerXMLDefRef(newInstance.updateValues);
            newInstance.m_varType = m_varType;
            newInstance.enumeratedNames = enumeratedNames;
            newInstance.selectedEnumeratedName = selectedEnumeratedName;
            return newInstance;
        }

        public void updateValues()
        {
            enumeratedNames = m_appViewModel.getAuxDefinition(m_varType);
        }

        public override bool validate()
        {
            List<string> enumeration = m_appViewModel.getAuxDefinition(m_varType);
            return enumeration.Exists(id => (id==content));
        }
    }
}
