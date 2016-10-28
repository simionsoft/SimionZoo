using System.IO;
using Simion;
using System.Xml;
using System;
using Caliburn.Micro;

namespace Badger.ViewModels
{
    public class ForkedNodeViewModel : ConfigNodeViewModel
    {
        private BindableCollection<ForkValueViewModel> m_values = new BindableCollection<ForkValueViewModel>();
        public BindableCollection<ForkValueViewModel> values
        {
            get { return m_values; }
            set { m_values = value; NotifyOfPropertyChange(() => values); }
        }

        private ForkValueViewModel m_selectedForkValue = null;
        public ForkValueViewModel selectedForkValue
        {
            get { return m_selectedForkValue; }
            set
            {
                m_selectedForkValue = value;
                NotifyOfPropertyChange(() => selectedForkValue);
                selectedValueConfigNode = m_selectedForkValue.configNode;
            }
        }

        public ConfigNodeViewModel selectedValueConfigNode
        {
            get { return selectedForkValue.configNode; }
            set { NotifyOfPropertyChange(() => selectedValueConfigNode); }
        }
        const double disabledOpacity = 0.5;
        const double enabledOpacity = 1.0;
        public string currentValueIndex { get { return (values.IndexOf(selectedForkValue)+1) + "/" + values.Count; } }
        public double bIsTherePreviousValue { get { if( values.IndexOf(selectedForkValue) > 0)
                    return enabledOpacity; return disabledOpacity; } }
        public double bIsThereNextValue { get { if (values.IndexOf(selectedForkValue) < values.Count - 1)
                    return enabledOpacity; return disabledOpacity; } }
        public double bIsThereMoreValues { get { if (values.Count > 1)
                    return enabledOpacity; return disabledOpacity; } }

        private void updateBoolFlags()
        {
            NotifyOfPropertyChange(() => bIsThereMoreValues);
            NotifyOfPropertyChange(() => bIsThereNextValue);
            NotifyOfPropertyChange(() => bIsTherePreviousValue);
            NotifyOfPropertyChange(() => currentValueIndex);
        }

        private void renameValues()
        {
            int i = 0;
            foreach (ForkValueViewModel fValue in values)
            {
                fValue.name = "Value-" + i;
                i++;
            }
        }

        //Constructor used from the experiment editor
        public ForkedNodeViewModel(AppViewModel appViewModel,ConfigNodeViewModel forkedNode)
        {
            //the config node forked now hangs from this ForkedNode's child
            m_parent = forkedNode.parent;
            forkedNode.parent = this;

            ForkValueViewModel newForkValue= new ForkValueViewModel("Value-0", this, forkedNode);
            values.Add(newForkValue);
            selectedForkValue = newForkValue;

            m_appViewModel = appViewModel;
            nodeDefinition = forkedNode.nodeDefinition;
            name = forkedNode.name;
            NotifyOfPropertyChange(() => selectedForkValue);
        }
        //Constructor called when loading an experiment config file
        public ForkedNodeViewModel(AppViewModel appViewModel,ConfigNodeViewModel parentNode
            ,XmlNode classDefinition,XmlNode configNode)
        {
            m_appViewModel = appViewModel;
            nodeDefinition = classDefinition;
            m_parent = parentNode;
            name = configNode.Attributes[XMLConfig.nameAttribute].Value;
            //fork = new ForkViewModel(appViewModel,name,this);
            foreach (XmlNode forkValueConfig in configNode.ChildNodes)
            {
                values.Add(new ForkValueViewModel(appViewModel, classDefinition,this, forkValueConfig));
            }
            //notify changes
            if (values.Count > 0)
                selectedForkValue = values[0];
            NotifyOfPropertyChange(() => values);
        }

        public override ConfigNodeViewModel clone()
        {
            throw new NotImplementedException();
        }

        public override bool validate()
        {
            foreach (ForkValueViewModel value in values)
            {
                if (!value.configNode.validate()) return false;
            }
            return true;
        }

        public override void outputXML(StreamWriter writer,string leftSpace)
        {
            if (m_appViewModel.saveMode == SaveMode.SaveForks)
            {
                writer.WriteLine(leftSpace + "<" + XMLConfig.forkedNodeTag + " " 
                    +XMLConfig.nameAttribute + "=\"" + name.TrimEnd(' ') + "\">");
                outputXML(writer, leftSpace + "  ");
                writer.WriteLine(leftSpace + "</" + XMLConfig.forkedNodeTag + ">");
            }
            else
                selectedForkValue.configNode.outputXML(writer, leftSpace);
        }

        public void addValue()
        {
            string newValueName= "Value-" + values.Count;
            ForkValueViewModel newForkValue = new ForkValueViewModel(newValueName,this,selectedValueConfigNode.clone());
            values.Add(newForkValue);
            updateBoolFlags();
        }

        public void removeFork()
        {
            NestedConfigNode parent = m_parent as NestedConfigNode;
            int childIndex = parent.children.IndexOf(this);
            parent.children.Remove(this);
            parent.children.Insert(childIndex, selectedForkValue.configNode);
        }

        public void removeSelectedValue()
        {
            //we don't remove the value if there is no other value
            if (values.Count == 1) return;

            ForkValueViewModel removedValue = selectedForkValue;
            int index = values.IndexOf(selectedForkValue);
            if (index == values.Count - 1)
                selectedForkValue = values[index - 1];
            else
                selectedForkValue = values[index + 1];

            values.Remove(removedValue);

            renameValues();
            updateBoolFlags();
        }

        public void nextValue()
        {
            int index = values.IndexOf(selectedForkValue);
            if (index < values.Count - 1)
                selectedForkValue = values[index + 1];
            updateBoolFlags();
        }
        public void previousValue()
        {
            int index = values.IndexOf(selectedForkValue);
            if (index >0)
                selectedForkValue = values[index - 1];
            updateBoolFlags();
        }
    }
}
