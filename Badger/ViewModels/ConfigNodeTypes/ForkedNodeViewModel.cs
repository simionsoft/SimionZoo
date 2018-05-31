using System.IO;
using Badger.Simion;
using System.Xml;
using System;
using Caliburn.Micro;

namespace Badger.ViewModels
{
    public class ForkedNodeViewModel : NestedConfigNode
    {
        private ForkValueViewModel m_selectedForkValue;

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
        public string currentValueIndex { get { return (children.IndexOf(selectedForkValue) + 1) + "/" + children.Count; } }
        public double bIsTherePreviousValue
        {
            get
            {
                if (children.IndexOf(selectedForkValue) > 0)
                    return enabledOpacity; return disabledOpacity;
            }
        }
        public double bIsThereNextValue
        {
            get
            {
                if (children.IndexOf(selectedForkValue) < children.Count - 1)
                    return enabledOpacity; return disabledOpacity;
            }
        }
        public double bIsThereMoreValues
        {
            get
            {
                if (children.Count > 1)
                    return enabledOpacity; return disabledOpacity;
            }
        }

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

        private string m_alias = "Name";
        public string alias
        {
            get { return m_alias; }
            set
            {
                m_alias = value;
                bIsValid = m_parentExperiment.forkRegistry.Validate(value);
                NotifyOfPropertyChange(() => alias);
            }
        }

        //Constructor used from the experiment editor
        public ForkedNodeViewModel(ExperimentViewModel parentExperiment, ConfigNodeViewModel forkedNode)
        {
            m_parent = forkedNode.parent;

            ForkValueViewModel newForkValue = new ForkValueViewModel("Value-0", this, forkedNode);
            children.Add(newForkValue);

            selectedForkValue = newForkValue;

            m_parentExperiment = parentExperiment;
            nodeDefinition = forkedNode.nodeDefinition;
            name = forkedNode.name;
            NotifyOfPropertyChange(() => selectedForkValue);

            //register this fork
            m_parentExperiment.forkRegistry.Add(this);

            //we set the alias AFTER adding the fork to the registry to make sure that validation is properly done
            alias = forkedNode.name;
        }

        //Constructor called when loading an experiment config file
        public ForkedNodeViewModel(ExperimentViewModel parentExperiment, ConfigNodeViewModel parentNode,
            XmlNode classDefinition, XmlNode configNode = null, bool initChildren = true)
        {
            //configNode must be non-null since no ForkedNodeVM can be created from the app defintion
            m_parentExperiment = parentExperiment;
            nodeDefinition = classDefinition;
            m_parent = parentNode;
            name = configNode.Attributes[XMLConfig.nameAttribute].Value;

            if (initChildren)
            {
                foreach (XmlNode forkValueConfig in configNode.ChildNodes)
                {
                    children.Add(new ForkValueViewModel(parentExperiment, classDefinition, this, forkValueConfig));
                }
            }
            //notify changes
            if (children.Count > 0)
                selectedForkValue = children[0] as ForkValueViewModel;
            NotifyOfPropertyChange(() => children);

            //register this fork
            m_parentExperiment.forkRegistry.Add(this);

            //we set the alias AFTER adding the fork to the registry to make sure that validation is properly done
            if (configNode.Attributes.GetNamedItem(XMLConfig.aliasAttribute) != null)
                alias = configNode.Attributes.GetNamedItem(XMLConfig.aliasAttribute).Value;
            else alias = name;
        }
        //constructor used in clone()
        public ForkedNodeViewModel() { }

        public override ConfigNodeViewModel clone()
        {
            ForkedNodeViewModel newForkedNode = new ForkedNodeViewModel();
            newForkedNode.m_parentExperiment = m_parentExperiment;
            newForkedNode.m_parent = m_parent;
            newForkedNode.name = name;
            foreach (ConfigNodeViewModel child in children)
            {
                ConfigNodeViewModel clonedChild = child.clone();
                clonedChild.parent = newForkedNode;
                newForkedNode.children.Add(clonedChild);
            }

            //register this fork
            m_parentExperiment.forkRegistry.Add(newForkedNode);

            if (newForkedNode.children.Count > 0)
                newForkedNode.selectedForkValue = newForkedNode.children[0] as ForkValueViewModel;
            return newForkedNode;
        }

        public override bool Validate()
        {
            if (name == "" || !m_parentExperiment.forkRegistry.Validate(alias))
                return false;
            foreach (ForkValueViewModel value in children)
            {
                if (!value.configNode.Validate()) return false;
            }
            return true;
        }

        public override void outputXML(StreamWriter writer, SaveMode mode, string leftSpace)
        {
            if (mode == SaveMode.AsProject || mode == SaveMode.AsExperiment)
            {
                writer.WriteLine(leftSpace + "<" + XMLConfig.forkedNodeTag + " "
                    + XMLConfig.nameAttribute + "=\"" + name.TrimEnd(' ') + "\" " + XMLConfig.aliasAttribute
                    + "=\"" + alias + "\">");
                foreach (ForkValueViewModel child in children)
                    child.outputXML(writer, mode, leftSpace + "  ");
                writer.WriteLine(leftSpace + "</" + XMLConfig.forkedNodeTag + ">");
            }
            else if (mode == SaveMode.AsExperimentalUnit)
            {
                selectedForkValue.configNode.outputXML(writer, mode, leftSpace);
            }
            else if (mode == SaveMode.ForkHierarchy)
            {
                // Open 'FORK' tag with its corresponding attributes
                writer.WriteLine(leftSpace + "<" + XMLConfig.forkTag + " "
                    + XMLConfig.nameAttribute + "=\"" + name.TrimEnd(' ') + "\" " + XMLConfig.aliasAttribute
                    + "=\"" + alias + "\">");
                // Fork values inside. 
                foreach (ForkValueViewModel child in children)
                    child.outputXML(writer, mode, leftSpace + "\t");
                // Close 'FORK' tag
                writer.WriteLine(leftSpace + "</" + XMLConfig.forkTag + ">");
            }
            else if (mode == SaveMode.ForkValues)
            {
                // Open 'FORK' tag inside experimental unit
                writer.WriteLine(leftSpace + "\t<" + XMLConfig.forkTag + " " + XMLConfig.nameAttribute
                    + "=\"" + name.TrimEnd(' ') + "\" " + XMLConfig.aliasAttribute + "=\"" + alias + "\">");
                // We take this nasty shortcut to allow children to be exported, while keeping
                // each fork value as an element in a list instead of nested values
                writer.WriteLine(leftSpace + "\t\t<" + XMLConfig.forkValueTag + " "
                    + XMLConfig.valueAttribute + "=\"" + selectedForkValue.configNode.content + "\"/>");
                // Close 'FORK' tag inside experimental unit
                writer.WriteLine(leftSpace + "\t</" + XMLConfig.forkTag + ">");

                selectedForkValue.outputXML(writer, mode, leftSpace);
            }
        }


        public void AddValue()
        {
            string newValueName = "Value-" + children.Count;
            ForkValueViewModel newForkValue = new ForkValueViewModel(newValueName, this, selectedValueConfigNode.clone());
            children.Add(newForkValue);
            updateBoolFlags();
            m_parentExperiment.updateNumForkCombinations();
        }


        public override void UnforkThisNode()
        {
            //unregister this fork
            m_parentExperiment.forkRegistry.Remove(this);

            NestedConfigNode parent = m_parent as NestedConfigNode;
            if (parent != null)
            {
                int childIndex = parent.children.IndexOf(this);
                parent.children.Remove(this);
                parent.children.Insert(childIndex, selectedForkValue.configNode);
                //the node can again be forked
                selectedForkValue.configNode.parent = parent;
                selectedForkValue.configNode.bCanBeForked = true;
                m_parentExperiment.updateNumForkCombinations();
            }
        }

        public void RemoveSelectedValue()
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
            m_parentExperiment.updateNumForkCombinations();
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
            if (index > 0)
                selectedForkValue = children[index - 1] as ForkValueViewModel;
            updateBoolFlags();
        }

        public override int getNumForkCombinations()
        {
            int numForkCombinations = 0;
            foreach (ConfigNodeViewModel child in children)
                numForkCombinations += child.getNumForkCombinations();
            return numForkCombinations;
        }

        public override void setForkCombination(ref int id, ref string combinationName)
        {
            int valueId = 0;
            ForkValueViewModel currentValue = children[0] as ForkValueViewModel;
            //set the correct value for this fork

            if (getNumForkCombinations() != children.Count)
            {
                //at least there's one fork beneath this one
                while (valueId < children.Count - 1 && currentValue != null && id >= currentValue.getNumForkCombinations())
                {
                    id -= currentValue.getNumForkCombinations();
                    ++valueId;
                    if (valueId < children.Count)
                        currentValue = children[valueId] as ForkValueViewModel;
                }
            }
            else
            {
                //leaf
                valueId = id % children.Count;
                id = id / children.Count;
                currentValue = children[valueId] as ForkValueViewModel;
            }

            combinationName += "-" + valueId;
            selectedForkValue = currentValue;

            //set correct values for child forked nodes
            if (currentValue.getNumForkCombinations() > 1)
                currentValue.setForkCombination(ref id, ref combinationName);
        }

        public override void onRemoved()
        {
            m_parentExperiment.forkRegistry.Remove(this);
        }
    }
}
