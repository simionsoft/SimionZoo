using Badger.Simion;
using System.Xml;
using System.Collections.Generic;
using Caliburn.Micro;

namespace Badger.ViewModels
{
    class WorldVarRefValueConfigViewModel: ConfigNodeViewModel
    {
        private List<string> m_varNames= new List<string>();
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

        public WorldVarRefValueConfigViewModel(ExperimentViewModel parentExperiment, WorldVarType varType, ConfigNodeViewModel parent, XmlNode definitionNode, string parentXPath, XmlNode configNode = null)
        {
            commonInit(parentExperiment, parent, definitionNode, parentXPath);

            m_varType = varType;//definitionNode.Attributes[XMLConfig.hangingFromAttribute].Value;

            //the possible values taken by this world variable
            m_parentExperiment.getWorldVarNameList(m_varType, ref m_varNames);
            NotifyOfPropertyChange(() => varNames);

            if (configNode != null)
            {
                configNode = configNode[name];
                selectedEnumeratedName = configNode.InnerText;
            }

            if (m_varNames.Count==0)
            {
                //Either we have loaded the config but the list is of values has not yet been loaded
                //or no config file has been loaded. In Either case, we register for a deferred load step
                m_parentExperiment.RegisterDeferredLoadStep(updateValues);
            }

            m_parentExperiment.registerWorldVarRef(updateValues);
        }

        public override ConfigNodeViewModel clone()
        {
            WorldVarRefValueConfigViewModel newInstance=
                new WorldVarRefValueConfigViewModel(m_parentExperiment, m_varType,m_parent, nodeDefinition, m_parent.xPath);
            m_parentExperiment.registerWorldVarRef(newInstance.updateValues);
            newInstance.m_varType = m_varType;
            newInstance.varNames = varNames;
            newInstance.selectedEnumeratedName = selectedEnumeratedName;
            return newInstance;
        }

        public void updateValues()
        {
            m_parentExperiment.getWorldVarNameList(m_varType, ref m_varNames);
            NotifyOfPropertyChange(() => varNames);

            //to force re-validation if the list of variables wasn't available at node creation time
            selectedEnumeratedName = selectedEnumeratedName;
        }

        public override bool validate()
        {
            return varNames.Exists(id => (id==content));
        }
    }
}
