using Caliburn.Micro;
using System.Xml;
using Simion;
using System.IO;

namespace Badger.ViewModels
{
    public class ChoiceConfigViewModel : NestedConfigNode
    {
        //choices
        private string m_selectedChoiceName= null;
        private ConfigNodeViewModel m_selectedChoice = null;
        public ConfigNodeViewModel selectedChoice
        {
            get { return m_selectedChoice; }
            set { m_selectedChoice = value;
                NotifyOfPropertyChange(() => selectedChoice); }
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

        public void loadSelectedChoiceElement(string selectedChoiceElementName, XmlNode configNode=null)
        {
            foreach (XmlNode choiceElement in nodeDefinition.ChildNodes)
                if (choiceElement.Attributes[XMLConfig.nameAttribute].Value == selectedChoiceElementName)
                {
                    //forces validation if it's called with a configNode
                    if (configNode != null)
                        selectedChoiceName = selectedChoiceElementName;

                    selectedChoice = getInstance(m_appViewModel,this
                        ,choiceElement, m_xPath, configNode);
                    m_children.Clear();
                    children.Add(selectedChoice);
                    break;
                }
        }

        private BindableCollection<string> m_choiceNames = new BindableCollection<string>();
        public BindableCollection<string> choiceNames { get { return m_choiceNames; } }

        
        public ChoiceConfigViewModel(AppViewModel appDefinition, ConfigNodeViewModel parent
            , XmlNode definitionNode
            , string parentXPath, XmlNode configNode = null, bool initChildren= true)
        {
            string choiceElementName;
            commonInit(appDefinition, parent, definitionNode, parentXPath);

            if (configNode != null) configNode = configNode[name];

            if (initChildren)
            {
                foreach (XmlNode choiceElement in definitionNode.ChildNodes)
                {
                    choiceElementName = choiceElement.Attributes[XMLConfig.nameAttribute].Value;

                    m_choiceNames.Add(choiceElementName);

                    if (configNode != null && choiceElementName == configNode.ChildNodes[0].Name)
                    {
                        loadSelectedChoiceElement(choiceElementName, configNode);
                    }
                }
            }
        }


        public override bool validate()
        {
            bool bChoiceNameIsValid = false;
            if (selectedChoiceName == null || selectedChoiceName == "") return false;
            foreach (XmlNode choiceElement in nodeDefinition)
                if (selectedChoiceName == choiceElement.Attributes[XMLConfig.nameAttribute].Value)
                    bChoiceNameIsValid= true;

            return bChoiceNameIsValid && base.validate();
        }

        public override void outputXML(StreamWriter writer,SaveMode mode, string leftSpace)
        {
            if (mode!=SaveMode.OnlyForks)
                writer.Write(leftSpace + getXMLHeader());
            selectedChoice.outputXML(writer,mode, leftSpace + "  ");
            if (mode!=SaveMode.OnlyForks)
                writer.Write(leftSpace + getXMLFooter());
        }

        public override ConfigNodeViewModel clone()
        {
            ChoiceConfigViewModel newChoice= new ChoiceConfigViewModel(m_appViewModel, parent
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