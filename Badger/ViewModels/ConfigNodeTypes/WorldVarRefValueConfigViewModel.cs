using System.Xml;
using System.Collections.ObjectModel;

namespace Badger.ViewModels
{
    class WorldVarRefValueConfigViewModel: ConfigNodeViewModel
    {
        private ObservableCollection<string> m_variables= new ObservableCollection<string>();
        public ObservableCollection<string> Variables
        {
            get { return m_variables; }
            set { m_variables = value; NotifyOfPropertyChange(() => Variables); }
        }

        public string SelectedVariable
        {
            get { return content; }
            set {
                content = value;
                NotifyOfPropertyChange(() => SelectedVariable);
            }
        }
        
        private WorldVarType m_varType;

        public WorldVarRefValueConfigViewModel(ExperimentViewModel parentExperiment, WorldVarType varType, ConfigNodeViewModel parent, XmlNode definitionNode, string parentXPath, XmlNode configNode = null)
        {
            CommonInitialization(parentExperiment, parent, definitionNode, parentXPath);

            m_varType = varType;//definitionNode.Attributes[XMLConfig.hangingFromAttribute].Value;

            //the possible values taken by this world variable
            m_parentExperiment.GetWorldVarNameList(m_varType, ref m_variables);
            NotifyOfPropertyChange(() => Variables);

            if (configNode != null)
            {
                configNode = configNode[name];
                SelectedVariable = configNode.InnerText;
            }

            if (m_variables.Count==0)
            {
                //Either we have loaded the config but the list is of values has not yet been loaded
                //or no config file has been loaded. In Either case, we register for a deferred load step
                m_parentExperiment.RegisterDeferredLoadStep(Update);
            }

            m_parentExperiment.RegisterWorldVarRef(Update);
        }

        public override ConfigNodeViewModel clone()
        {
            WorldVarRefValueConfigViewModel newInstance=
                new WorldVarRefValueConfigViewModel(m_parentExperiment, m_varType,m_parent, nodeDefinition, m_parent.xPath);
            m_parentExperiment.RegisterWorldVarRef(newInstance.Update);
            newInstance.m_varType = m_varType;
            newInstance.Variables = Variables;
            newInstance.SelectedVariable = SelectedVariable;
            return newInstance;
        }

        public void Update()
        {
            m_parentExperiment.GetWorldVarNameList(m_varType, ref m_variables);
            NotifyOfPropertyChange(() => Variables);

            //to force re-validation if the list of variables wasn't available at node creation time
            if (!Variables.Contains(content))
            {
                if (m_variables.Count > 0)
                    SelectedVariable = Variables[0];
                else SelectedVariable = "";
            }
        }

        public override bool Validate()
        {
            return Variables.Contains(content);
        }
    }
}
