using System.IO;
using Simion;
using System.Xml;
using System;
using Caliburn.Micro;

namespace Badger.ViewModels
{
    public class ForkedNodeViewModel : NestedConfigNode
    {
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
        public string currentValueIndex { get { return (children.IndexOf(selectedForkValue)+1) + "/" + children.Count; } }
        public double bIsTherePreviousValue { get { if( children.IndexOf(selectedForkValue) > 0)
                    return enabledOpacity; return disabledOpacity; } }
        public double bIsThereNextValue { get { if (children.IndexOf(selectedForkValue) < children.Count - 1)
                    return enabledOpacity; return disabledOpacity; } }
        public double bIsThereMoreValues { get { if (children.Count > 1)
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
            foreach (ForkValueViewModel fValue in children)
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
            children.Add(newForkValue);
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
                children.Add(new ForkValueViewModel(appViewModel, classDefinition,this, forkValueConfig));
            }
            //notify changes
            if (children.Count > 0)
                selectedForkValue = children[0] as ForkValueViewModel;
            NotifyOfPropertyChange(() => children);
        }

        public override ConfigNodeViewModel clone()
        {
            throw new NotImplementedException();
        }

        public override bool validate()
        {
            foreach (ForkValueViewModel value in children)
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
            string newValueName= "Value-" + children.Count;
            ForkValueViewModel newForkValue = new ForkValueViewModel(newValueName,this,selectedValueConfigNode.clone());
            children.Add(newForkValue);
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
            if (children.Count == 1) return;

            ForkValueViewModel removedValue = selectedForkValue;
            int index = children.IndexOf(selectedForkValue);
            if (index == children.Count - 1)
                selectedForkValue = children[index - 1] as ForkValueViewModel;
            else
                selectedForkValue = children[index + 1] as ForkValueViewModel;

            children.Remove(removedValue);

            renameValues();
            updateBoolFlags();
        }

        public void nextValue()
        {
            int index = children.IndexOf(selectedForkValue);
            if (index < children.Count - 1)
                selectedForkValue = children[index + 1] as ForkValueViewModel;
            updateBoolFlags();
        }
        public void previousValue()
        {
            int index = children.IndexOf(selectedForkValue);
            if (index >0)
                selectedForkValue = children[index - 1] as ForkValueViewModel;
            updateBoolFlags();
        }

        public override int getNumForkCombinations()
        {
            int numForkCombinations= 0;
            foreach (ConfigNodeViewModel child in children)
                numForkCombinations += child.getNumForkCombinations();
            return numForkCombinations;
        }

        public override void setForkCombination(ref int id)
        {
            int valueId;
            //set the correct value for the fork
            valueId = id % children.Count;
            selectedForkValue = children[valueId] as ForkValueViewModel;
            id = id / children.Count;

            foreach (ConfigNodeViewModel child in children)
            {
                child.setForkCombination(ref id);
            }
        }
    }
}
