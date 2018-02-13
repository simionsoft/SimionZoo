using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Xml;

namespace Badger.ViewModels.ConfigNodeTypes
{
    class LinkedNodeViewModel : NestedConfigNode
    {
        private ForkValueViewModel m_selectedForkValue;


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
        public LinkedNodeViewModel(ExperimentViewModel parentExperiment, ConfigNodeViewModel forkedNode)
        {

        }

        /// <summary>
        /// Constructor called when loading an experiment config file
        /// </summary>
        /// <param name="parentExperiment"></param>
        /// <param name="parentNode"></param>
        /// <param name="classDefinition"></param>
        /// <param name="configNode"></param>
        /// <param name="initChildren"></param
        public LinkedNodeViewModel(ExperimentViewModel parentExperiment, ConfigNodeViewModel parentNode,
            XmlNode classDefinition, XmlNode configNode = null, bool initChildren = true)
        {
            //configNode must be non-null since no ForkedNodeVM can be created from the app defintion
            m_parentExperiment = parentExperiment;
            nodeDefinition = classDefinition;
            m_parent = parentNode;

            if (initChildren)
            {
                foreach (XmlNode forkValueConfig in configNode.ChildNodes)
                {
                    children.Add(new ForkValueViewModel(parentExperiment, classDefinition, this, forkValueConfig));
                }
            }
        }

        //constructor used in clone()
        public LinkedNodeViewModel() { }

        public override ConfigNodeViewModel clone()
        {
            ForkedNodeViewModel newForkedNode = new ForkedNodeViewModel();
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


        public override bool validate()
        {
            if (name == "" || !m_parentExperiment.forkRegistry.Validate(alias))
                return false;
            foreach (ForkValueViewModel value in children)
            {
                if (!value.configNode.validate()) return false;
            }
            return true;
        }

     


        public void UnlinkNode()
        {
            //unregister this fork


            NestedConfigNode parent = m_parent as NestedConfigNode;
            if (parent != null)
            {
                int childIndex = parent.children.IndexOf(this);
                parent.children.Remove(this);
                m_parentExperiment.updateNumForkCombinations();
            }
        }
    }
}
