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
                parentExperiment.AddWire(SelectedWire);
            }

            m_parentExperiment.RegisterDeferredLoadStep(Update);
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
            string oldSelectedWire = SelectedWire; //need to save it before it gets invalidated on updating the list

            m_parentExperiment.GetWireNames(ref m_wireNames);
            NotifyOfPropertyChange(() => WireNames);

            SelectedWire = oldSelectedWire;
        }

        public override bool Validate()
        {
            return WireNames.Exists(id => (id == content));
        }
    }

}
