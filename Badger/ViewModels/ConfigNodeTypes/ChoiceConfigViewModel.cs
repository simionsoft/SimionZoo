using Caliburn.Micro;
using System.Xml;
using System.IO;

using Herd.Files;

namespace Badger.ViewModels
{
    public class ChoiceConfigViewModel : NestedConfigNode
    {
        //choices
        private string m_selectedChoiceName = null;
        private ConfigNodeViewModel m_selectedChoice = null;
        public ConfigNodeViewModel selectedChoice
        {
            get { return m_selectedChoice; }
            set
            {
                if (m_selectedChoice != null)
                    m_selectedChoice.onRemoved(); //this is called to unregister forks beneath this node
                m_selectedChoice = value;
                NotifyOfPropertyChange(() => selectedChoice);
            }
        }
        public string selectedChoiceName
        {
            get { return m_selectedChoiceName; }
            set
            {
                m_selectedChoiceName = value;
                content = m_selectedChoiceName;
                NotifyOfPropertyChange(() => selectedChoiceName);

                loadSelectedChoiceElement(selectedChoiceName);
            }
        }

        public void loadSelectedChoiceElement(string selectedChoiceElementName, XmlNode configNode = null)
        {
            foreach (XmlNode choiceElement in nodeDefinition.ChildNodes)
                if (choiceElement.Attributes[XMLTags.nameAttribute].Value == selectedChoiceElementName)
                {
                    //forces validation if it's called with a configNode
                    if (configNode != null)
                        selectedChoiceName = selectedChoiceElementName;

                    selectedChoice = getInstance(m_parentExperiment, this, choiceElement, m_xPath, configNode);
                    m_children.Clear();
                    children.Add(selectedChoice);
                    break;
                }
        }

        private BindableCollection<string> m_choiceNames = new BindableCollection<string>();
        public BindableCollection<string> choiceNames { get { return m_choiceNames; } }


        public ChoiceConfigViewModel(ExperimentViewModel parentExperiment, ConfigNodeViewModel parent,
            XmlNode definitionNode, string parentXPath, XmlNode configNode = null, bool initChildren = true)
        {
            string choiceElementName;
            CommonInitialization(parentExperiment, parent, definitionNode, parentXPath);

            if (configNode != null) configNode = configNode[name];

            if (initChildren)
            {
                foreach (XmlNode choiceElement in definitionNode.ChildNodes)
                {
                    choiceElementName = choiceElement.Attributes[XMLTags.nameAttribute].Value;

                    m_choiceNames.Add(choiceElementName);

                    if (configNode != null && choiceElementName == configNode.ChildNodes[0].Name)
                    {
                        loadSelectedChoiceElement(choiceElementName, configNode);
                    }
                }
            }
        }


        public override bool Validate()
        {
            bool bChoiceNameIsValid = false;
            if (selectedChoiceName == null || selectedChoiceName == "")
                return false;
            foreach (XmlNode choiceElement in nodeDefinition)
                if (selectedChoiceName == choiceElement.Attributes[XMLTags.nameAttribute].Value)
                    bChoiceNameIsValid = true;

            return bChoiceNameIsValid && base.Validate();
        }

        public override void outputXML(StreamWriter writer, SaveMode mode, string leftSpace)
        {
            if (mode == SaveMode.AsExperiment || mode == SaveMode.AsExperimentalUnit)
                writer.Write(leftSpace + getXMLHeader());
            // If we don't do this and just take leftSpace value passed as parameter the behaviour
            // of this variable is a litle bit odd.
            leftSpace += "  ";
            selectedChoice.outputXML(writer, mode,  leftSpace);

            if (mode == SaveMode.AsExperiment || mode == SaveMode.AsExperimentalUnit)
                writer.Write(leftSpace + getXMLFooter());
        }

        public override ConfigNodeViewModel clone()
        {
            ChoiceConfigViewModel newChoice = new ChoiceConfigViewModel(m_parentExperiment, parent
                , nodeDefinition, parent.xPath, null);
            foreach (ConfigNodeViewModel child in children)
            {
                ConfigNodeViewModel clonedChild = child.clone();
                clonedChild.parent = newChoice;
                newChoice.children.Add(clonedChild);
            }
            newChoice.selectedChoiceName = selectedChoiceName;
            return newChoice;
        }

    }
}