using System.IO;
using System.Xml;
using Caliburn.Micro;
using Badger.Simion;
using System;
using Badger.ViewModels.ConfigNodeTypes;
using System.Collections.Generic;
using System.Linq;

namespace Badger.ViewModels
{
    abstract public class ConfigNodeViewModel : PropertyChangedBase
    {
        //access to the root node
        protected ExperimentViewModel m_parentExperiment;

        private XmlNode m_nodeDefinition;

        public XmlNode nodeDefinition
        {
            get { return m_nodeDefinition; }
            set
            {
                m_nodeDefinition = value;
                NotifyOfPropertyChange(() => nodeDefinition);
            }
        }

        protected string m_default = "";

        private string m_content = "";


        public string content
        {
            get { return m_content; }
            set
            {
                m_content = value;
                bIsValid = Validate();
                NotifyOfPropertyChange(() => content);
                // Reflect content change in linked nodes
                foreach (var node in m_linkedNodes)
                    node.content = value;
            }
        }


        private string m_textColor = XMLConfig.colorDefaultValue;


        public string textColor
        {
            get { return m_textColor; }
            set { m_textColor = value; NotifyOfPropertyChange(() => textColor); }
        }


        abstract public bool Validate();

        //Comment
        private string m_comment = "";

        public string comment { get { return m_comment; } set { m_comment = value; } }

        //Validation
        private bool m_bIsValid;

        public bool bIsValid
        {
            get { return m_bIsValid; }
            set
            {
                m_bIsValid = value;
                if (m_bIsValid) textColor = XMLConfig.colorValidValue;
                else textColor = XMLConfig.colorInvalidValue;
                NotifyOfPropertyChange(() => bIsValid);
            }
        }


        private bool m_bCanBeForked = true;

        public bool bCanBeForked { get { return m_bCanBeForked; } set { m_bCanBeForked = value; NotifyOfPropertyChange(() => bCanBeForked); } }

        public void ForkThisNode(ConfigNodeViewModel originNode)
        {
            bCanBeForked = false;

            if (originNode.m_parent != null)
            {
                originNode.m_parent.ForkChild(originNode);
            }
            else Console.WriteLine("Can't fork this node because it has no parent: " + name);
        }

        virtual public void ForkChild(ConfigNodeViewModel forkedChild)
        {
            Console.WriteLine("Error: non-nested config node asked to fork a child");
        }

        virtual public void UnforkThisNode() { }

        private bool m_bCanBeLinked;

        public bool CanBeLinked
        {
            get { return m_bCanBeLinked; }
            set
            {
                m_bCanBeLinked = value;
                NotifyOfPropertyChange(() => CanBeLinked);
            }
        }

        private bool m_bIsLinkable = true;

        /// <summary>
        /// All nodes are linkable by default. The moment a node of a type is chosen as a
        /// link origin this property is set to false for that specific node and for all other
        /// nodes of that type.
        /// </summary>
        public bool IsLinkable
        {
            get { return m_bIsLinkable; }
            set
            {
                m_bIsLinkable = value;
                NotifyOfPropertyChange(() => IsLinkable);
            }
        }

        private bool m_bLinking;

        public bool Linking
        {
            get { return m_bLinking; }
            set
            {
                m_bLinking = value;
                m_bCanBeLinked = !m_bLinking;
                NotifyOfPropertyChange(() => Linking);
            }
        }

        private bool m_bIsLinked;

        public bool IsLinked
        {
            get { return m_bIsLinked; }
            set
            {
                m_bIsLinked = value;
                NotifyOfPropertyChange(() => IsLinked);
            }
        }

        private bool m_bIsNotLinked = true;

        public bool IsNotLinked
        {
            get { return m_bIsNotLinked; }
            set
            {
                m_bIsNotLinked = value;
                NotifyOfPropertyChange(() => IsNotLinked);
            }
        }

        private bool m_bIsLinkOrigin;

        public bool IsLinkOrigin
        {
            get { return m_bIsLinkOrigin; }
            set
            {
                m_bIsLinkOrigin = value;
                NotifyOfPropertyChange(() => IsLinkOrigin);
            }
        }

        /// <summary>
        /// The list with the linked nodes
        /// </summary>
        private BindableCollection<ConfigNodeViewModel> m_linkedNodes = new BindableCollection<ConfigNodeViewModel>();

        /// <summary>
        /// Linked nodes list property to get and/or set values
        /// </summary>
        public BindableCollection<ConfigNodeViewModel> LinkedNodes
        {
            get { return m_linkedNodes; }
            set { m_linkedNodes = value; NotifyOfPropertyChange(() => LinkedNodes); }
        }

        /// <summary>
        ///  Take the right-clicked node as the origin node to link with all the posible linkable
        ///  nodes (i.e. nodes of the same class). Linkable nodes CanBeLinked property value are set 
        ///  to true.
        /// </summary>
        /// <param name="originNode">The origin node of the linking process</param>
        public void LinkThisNode(ConfigNodeViewModel originNode)
        {
            Linking = IsLinkOrigin = true;
            IsLinkable = false;
            m_parentExperiment.LinkOriginNode = originNode;
            m_parentExperiment.CheckLinkableNodes(originNode);
        }

        /// <summary>
        /// Cancel a linking process between two nodes.
        /// </summary>
        public void CancelLinking(ConfigNodeViewModel originNode)
        {
            Linking = IsLinkOrigin = false;
            IsLinkable = true;
            m_parentExperiment.LinkOriginNode = null;
            m_parentExperiment.CheckLinkableNodes(originNode, false);
        }

        /// <summary>
        /// Actually perform the linking with the node.
        /// </summary>
        /// <param name="targetNode"></param>
        public void Link(ConfigNodeViewModel targetNode)
        {
            var linkedNode = new LinkedNodeViewModel(m_parentExperiment,
                        m_parentExperiment.LinkOriginNode, targetNode);

            var node = m_parentExperiment.DepthFirstSearch(targetNode);

            NestedConfigNode parent = (NestedConfigNode)node.m_parent;
            // For node substitution we just need the index in the parent children list
            int index = parent.children.IndexOf(node);
            parent.children.Remove(node);
            parent.children.Insert(index, linkedNode);

            m_parentExperiment.LinkOriginNode.LinkedNodes.Add(linkedNode.LinkedNode);
            // If the origin is a ForkedNode we also need to link the fork values inside
            // this is for content changes reflection.
            if (m_parentExperiment.LinkOriginNode is ForkedNodeViewModel)
            {
                ForkedNodeViewModel forkedOrigin = (ForkedNodeViewModel)m_parentExperiment.LinkOriginNode;
                int len = forkedOrigin.children.Count;

                for (int i = 0; i < len; i++)
                {
                    ForkedNodeViewModel linkedFork = (ForkedNodeViewModel)linkedNode.LinkedNode;
                    ForkValueViewModel linkedForkValue = (ForkValueViewModel)linkedFork.children[i];
                    ((ForkValueViewModel)forkedOrigin.children[i]).configNode.LinkedNodes
                        .Add(linkedForkValue.configNode);
                    linkedForkValue.configNode.name = targetNode.name;
                    linkedForkValue.configNode.comment = targetNode.comment;
                    linkedForkValue.configNode.nodeDefinition = targetNode.nodeDefinition;
                }
            }

            linkedNode.LinkedNode.IsLinkable = false;
            linkedNode.LinkedNode.IsLinked = true;
            linkedNode.LinkedNode.IsNotLinked = false;
        }

        /// <summary>
        /// Unlink the node removing it from its origin linked nodes list and restore it to its 
        /// original node class.
        /// </summary>
        public void UnlinkNode()
        {
            LinkedNodeViewModel linkedNode = (LinkedNodeViewModel)this;
            linkedNode.Origin.LinkedNodes.Remove(linkedNode);

            NestedConfigNode parent = (NestedConfigNode)linkedNode.m_parent;

            ConfigNodeViewModel unlinkedNode = getInstance(m_parentExperiment, parent,
                linkedNode.nodeDefinition, m_parentExperiment.AppName);
            // Keep the content of the former ogirin node
            if (!(linkedNode.Origin is ForkedNodeViewModel))
            {
                unlinkedNode.content = linkedNode.Origin.content;
            }
            else
            {
                ForkedNodeViewModel forkedNode = (ForkedNodeViewModel)linkedNode.LinkedNode;
                int valueIndex = int.Parse(forkedNode.currentValueIndex.Substring(0, 1)) - 1;
                ForkValueViewModel value = (ForkValueViewModel)forkedNode.children[valueIndex];
                unlinkedNode.content = value.configNode.content;
            }
            // For node substitution We don't need the index in the whole tree just
            // the index in the parent children list
            int index = parent.children.IndexOf(linkedNode);
            parent.children.Remove(linkedNode);
            parent.children.Insert(index, unlinkedNode);

            unlinkedNode.CanBeLinked = true;
            unlinkedNode.IsLinkable = false;
        }

        //clone
        public abstract ConfigNodeViewModel clone();


        //XML output methods
        public virtual void outputXML(StreamWriter writer, SaveMode mode, string leftSpace)
        {
            if (mode == SaveMode.AsExperiment || mode == SaveMode.AsExperimentalUnit)
                writer.Write(leftSpace + "<" + name + ">" + content + "</" + name + ">\n");
        }

        //XPath methods
        protected string m_xPath;

        public string xPath { get { return m_xPath; } set { m_xPath = value; } }

        //Name
        private string m_name;

        public string name { get { return m_name; } set { m_name = value; } }

        //Parent
        protected ConfigNodeViewModel m_parent;

        public ConfigNodeViewModel parent
        {
            get { return m_parent; }
            set { m_parent = value; }
        }

        //Initialization stuff common to all types of configuration nodes
        protected void CommonInitialization(ExperimentViewModel parentExperiment, ConfigNodeViewModel parent,
            XmlNode definitionNode, string parentXPath)
        {
            name = definitionNode.Attributes[XMLConfig.nameAttribute].Value;

            m_parent = parent;
            m_parentExperiment = parentExperiment;
            nodeDefinition = definitionNode;

            xPath = parentXPath + "\\" + name;
            if (definitionNode.Attributes.GetNamedItem(XMLConfig.defaultAttribute) != null)
            {
                m_default = definitionNode.Attributes[XMLConfig.defaultAttribute].Value;
            }
            if (definitionNode.Attributes.GetNamedItem(XMLConfig.commentAttribute) != null)
            {
                comment = definitionNode.Attributes[XMLConfig.commentAttribute].Value;
            }
        }


        //FACTORY
        public static ConfigNodeViewModel getInstance(ExperimentViewModel parentExperiment, ConfigNodeViewModel parent,
            XmlNode definitionNode, string parentXPath, XmlNode configNode = null)
        {
            switch (definitionNode.Name)
            {
                case XMLConfig.neuralNetworkNodeTag:
                    return new NeuralNetworkProblemDescriptionConfigViewModel(parentExperiment, parent, definitionNode,
                        parentXPath, configNode);

                case XMLConfig.integerNodeTag:
                    return new IntegerValueConfigViewModel(parentExperiment, parent, definitionNode,
                        parentXPath, configNode);

                case XMLConfig.boolNodeTag:
                    return new BoolValueConfigViewModel(parentExperiment, parent, definitionNode,
                        parentXPath, configNode);

                case XMLConfig.doubleNodeTag:
                    return new DoubleValueConfigViewModel(parentExperiment, parent, definitionNode,
                        parentXPath, configNode);

                case XMLConfig.stringNodeTag:
                    return new StringValueConfigViewModel(parentExperiment, parent, definitionNode,
                        parentXPath, configNode);

                case XMLConfig.filePathNodeTag:
                    return new FilePathValueConfigViewModel(parentExperiment, parent, definitionNode,
                        parentXPath, configNode);

                case XMLConfig.dirPathNodeTag:
                    return new DirPathValueConfigViewModel(parentExperiment, parent, definitionNode,
                        parentXPath, configNode);

                case XMLConfig.stateVarRefTag:
                    return new WorldVarRefValueConfigViewModel(parentExperiment, WorldVarType.StateVar,
                        parent, definitionNode, parentXPath, configNode);

                case XMLConfig.actionVarRefTag:
                    return new WorldVarRefValueConfigViewModel(parentExperiment, WorldVarType.ActionVar,
                        parent, definitionNode, parentXPath, configNode);

                case XMLConfig.branchNodeTag:
                    return new BranchConfigViewModel(parentExperiment, parent, definitionNode, parentXPath,
                        configNode);

                case XMLConfig.choiceNodeTag:
                    return new ChoiceConfigViewModel(parentExperiment, parent, definitionNode, parentXPath,
                        configNode);

                case XMLConfig.choiceElementNodeTag:
                    return new ChoiceElementConfigViewModel(parentExperiment, parent, definitionNode,
                        parentXPath, configNode);

                case XMLConfig.enumNodeTag:
                    return new EnumeratedValueConfigViewModel(parentExperiment, parent, definitionNode,
                        parentXPath, configNode);

                case XMLConfig.multiValuedNodeTag:
                    return new MultiValuedConfigViewModel(parentExperiment, parent, definitionNode,
                        parentXPath, configNode);

                default:
                    return null;
            }
        }

        public virtual int getNumForkCombinations()
        {
            return 1;
        }

        //we do nothing by default
        public virtual void setForkCombination(ref int id, ref string combinationName) { }


        //Lambda Traverse functions
        public virtual int traverseRetInt(Func<ConfigNodeViewModel, int> simpleNodeFunc,
            Func<ConfigNodeViewModel, int> nestedNodeFunc)
        {
            return simpleNodeFunc(this);
        }

        public virtual void traverseDoAction(Action<ConfigNodeViewModel> simpleNodeAction,
            Action<NestedConfigNode> nestedNodeAction)
        {
            simpleNodeAction(this);
        }

        public virtual void onRemoved() { }
    }
}
