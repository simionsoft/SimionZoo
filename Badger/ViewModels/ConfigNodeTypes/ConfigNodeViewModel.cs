using System.IO;
using System.Xml;
using Caliburn.Micro;
using Badger.Simion;
using System;

namespace Badger.ViewModels
{
    abstract public class ConfigNodeViewModel : PropertyChangedBase
    {
        //access to the root node
        protected ExperimentViewModel m_parentExperiment;
        public XmlNode nodeDefinition;

        protected string m_default = "";
        private string m_content = "";

        public string content
        {
            get { return m_content; }
            set
            {
                m_content = value;
                bIsValid = validate();
                NotifyOfPropertyChange(() => content);
            }
        }


        private string m_textColor = XMLConfig.colorDefaultValue;


        public string textColor
        {
            get { return m_textColor; }
            set { m_textColor = value; NotifyOfPropertyChange(() => textColor); }
        }


        abstract public bool validate();

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

        public void forkThisNode()
        {
            bCanBeForked = false;

            if (m_parent != null)
            {
                m_parent.forkChild(this);
            }
            else Console.WriteLine("Can't fork this node because it has no parent: " + name);
        }

        virtual public void forkChild(ConfigNodeViewModel forkedChild)
        {
            Console.WriteLine("Error: non-nested config node asked to fork a child");
        }

        virtual public void unforkThisNode() { }


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
        protected void commonInit(ExperimentViewModel parentExperiment, ConfigNodeViewModel parent,
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
