using Simion;
using System.Xml;
using System.Collections.Generic;
using Caliburn.Micro;

namespace Badger.ViewModels
{
    class WorldVarRefValueConfigViewModel: ConfigNodeViewModel
    {
        private List<string> m_varNames= null;
        public List<string> varNames
        {
            get { return m_varNames; }
            set { m_varNames = value; NotifyOfPropertyChange(() => varNames); }
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
            m_appViewModel.getWorldVarNameList(m_varType, ref m_varNames);
            NotifyOfPropertyChange(() => varNames);

            if (varNames == null)
            {
                //Either we have loaded the config but the list is of values has not yet been loaded
                //or no config file has been loaded. In Either case, we register for a deferred load step
                m_appViewModel.registerDeferredLoadStep(updateValues);
            }

            m_appViewModel.registerWorldVarRef(updateValues);
        }

        public override ConfigNodeViewModel clone()
        {
            WorldVarRefValueConfigViewModel newInstance=
                new WorldVarRefValueConfigViewModel(m_appViewModel, m_varType,m_parent, nodeDefinition, m_parent.xPath);
            m_appViewModel.registerWorldVarRef(newInstance.updateValues);
            newInstance.m_varType = m_varType;
            newInstance.varNames = varNames;
            newInstance.selectedEnumeratedName = selectedEnumeratedName;
            return newInstance;
        }

        public void updateValues()
        {
            m_appViewModel.getWorldVarNameList(m_varType, ref m_varNames);
            NotifyOfPropertyChange(() => varNames);
        }

        public override bool validate()
        {
            return varNames.Exists(id => (id==content));
        }
    }
}
