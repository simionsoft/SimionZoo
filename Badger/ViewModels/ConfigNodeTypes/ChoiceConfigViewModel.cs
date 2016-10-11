using Caliburn.Micro;
using System.Xml;
using Simion;

namespace Badger.ViewModels
{
    public class ChoiceConfigViewModel : NestedConfigNode
    {
        //aux xml definitions stuff
        private string m_xmlDefinitionId="";

        //choices
        private string m_selectedChoiceName = "";
        private ConfigNodeViewModel m_selectedChoice = null;
        public ConfigNodeViewModel selectedChoice
        {
            get { return m_selectedChoice; }
            set { m_selectedChoice = value; NotifyOfPropertyChange(() => selectedChoice); }
        }
        public string selectedChoiceName
        {
            get { return m_selectedChoiceName; }
            set
            {
                m_selectedChoiceName = value;
                foreach (ConfigNodeViewModel child in m_children)
                    if (child.name == value)
                    {
                        child.setSelected();
                        selectedChoice = child;
                        NotifyOfPropertyChange(() => selectedChoiceName);
                        NotifyOfPropertyChange(() => selectedChoice);
                        break;
                    }
            }
        }

        private BindableCollection<string> m_choiceNames = new BindableCollection<string>();
        public BindableCollection<string> choiceNames { get { return m_choiceNames; } }


        public ChoiceConfigViewModel(AppViewModel appDefinition, XmlNode definitionNode, string parentXPath, XmlNode configNode = null)
        {
            commonInit(appDefinition,definitionNode, parentXPath);

            // currently we ignore the xml definition Id and assume we will only use one
            //if (definitionNode.Attributes.GetNamedItem(XMLConfig.xmlDefinitionId) != null)
            //    m_xmlDefinitionId = definitionNode.Attributes[XMLConfig.xmlDefinitionId].Value;

            childrenInit(appDefinition, definitionNode, parentXPath, configNode);

            //init the list of choices
            foreach (ConfigNodeViewModel child in m_children)
                m_choiceNames.Add(child.name);
            NotifyOfPropertyChange(() => choiceNames);

            if (configNode == null && m_children.Count > 0)
            {
                selectedChoiceName = m_children[0].name;
                selectedChoice = m_children[0];
            }
            else
            {
                //init from config file
                selectedChoiceName = configNode.ChildNodes[0].Name;
                foreach (ConfigNodeViewModel child in m_children)
                    if (child.name == selectedChoiceName)
                        selectedChoice = child;
            }
        }

        public override bool validate()
        {
            return (selectedChoice == null) || selectedChoice.validate();
        }

        public override string getXMLHeader() { return "<" + name + ">"; }
        public override string getXMLFooter() { return "</" + name + ">"; }
        public override string getXML()
        {

            return selectedChoice.getXML();
        }
    }
}