using Badger.Simion;
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

        public EnumeratedValueConfigViewModel(ExperimentViewModel parentExperiment, ConfigNodeViewModel parent,
            XmlNode definitionNode, string parentXPath, XmlNode configNode = null)
        {
            CommonInitialization(parentExperiment, parent, definitionNode, parentXPath);

            m_class = definitionNode.Attributes[XMLConfig.classAttribute].Value;
            enumeratedNames = m_parentExperiment.GetEnumeratedType(m_class);

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

        public override ConfigNodeViewModel clone()
        {
            EnumeratedValueConfigViewModel newInstance =
                new EnumeratedValueConfigViewModel(m_parentExperiment, m_parent, nodeDefinition, m_parent.xPath);
            
            newInstance.m_class = m_class;
            newInstance.enumeratedNames = enumeratedNames;
            newInstance.content = content;
            newInstance.textColor = textColor;
            return newInstance;
        }


        public override bool Validate()
        {
            List<string> enumeration = m_parentExperiment.GetEnumeratedType(m_class);
            return enumeration.Exists(id => (id==content));
        }
    }
}
