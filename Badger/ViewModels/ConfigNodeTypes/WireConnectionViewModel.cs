using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;

namespace Badger.ViewModels
{
    class WireConnectionViewModel: ConfigNodeViewModel
    {
        private List<string> m_wireNames = new List<string>();
        public List<string> WireNames
        {
            get { return m_wireNames; }
            set { m_wireNames = value; NotifyOfPropertyChange(() => WireNames); }
        }

        public string SelectedWire
        {
            get { return content; }
            set
            {
                content = value;
                NotifyOfPropertyChange(() => SelectedWire);
            }
        }

        public WireConnectionViewModel(ExperimentViewModel parentExperiment, ConfigNodeViewModel parent, XmlNode definitionNode, string parentXPath, XmlNode configNode = null)
        {
            CommonInitialization(parentExperiment, parent, definitionNode, parentXPath);

            //the possible values taken by this world variable
            m_parentExperiment.GetWireNames(ref m_wireNames);
            NotifyOfPropertyChange(() => WireNames);

            if (configNode != null)
            {
                configNode = configNode[name];
                SelectedWire = configNode.InnerText;
            }

            if (m_wireNames.Count == 0)
            {
                //Either we have loaded the config but the list is of values has not yet been loaded
                //or no config file has been loaded. In Either case, we register for a deferred load step
                m_parentExperiment.RegisterDeferredLoadStep(Update);
            }

            m_parentExperiment.RegisterWireConnection(Update);
        }

        public override ConfigNodeViewModel clone()
        {
            WireConnectionViewModel newInstance =
                new WireConnectionViewModel(m_parentExperiment, m_parent, nodeDefinition, m_parent.xPath);
            m_parentExperiment.RegisterWireConnection(newInstance.Update);
            newInstance.WireNames = WireNames;
            newInstance.SelectedWire = SelectedWire;
            return newInstance;
        }

        public void Update()
        {
            m_parentExperiment.GetWireNames(ref m_wireNames);
            NotifyOfPropertyChange(() => WireNames);

            //to force re-validation if the list of variables wasn't available at node creation time
            if (!WireNames.Exists(id => (id == content)))
            {
                if (m_wireNames.Count > 0)
                    SelectedWire = WireNames[0];
                else SelectedWire = "";
            }
        }

        public override bool Validate()
        {
            return WireNames.Exists(id => (id == content));
        }
    }

}
