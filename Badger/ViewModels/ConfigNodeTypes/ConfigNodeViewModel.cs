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
        private bool m_bIsValid = false;
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
            else System.Console.WriteLine("Can't fork this node because it has no parent: " + name);
        }
        virtual public void forkChild(ConfigNodeViewModel forkedChild)
        {
            System.Console.WriteLine("Error: non-nested config node asked to fork a child");
        }
        virtual public void unforkThisNode()
        {
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
        protected void commonInit(ExperimentViewModel parentExperiment, ConfigNodeViewModel parent, XmlNode definitionNode, string parentXPath)
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
        public static ConfigNodeViewModel getInstance(ExperimentViewModel parentExperiment, ConfigNodeViewModel parent
            , XmlNode definitionNode, string parentXPath, XmlNode configNode = null)
        {
            switch (definitionNode.Name)
            {
                case XMLConfig.integerNodeTag: return new IntegerValueConfigViewModel(parentExperiment, parent, definitionNode, parentXPath, configNode);
                case XMLConfig.boolNodeTag: return new BoolValueConfigViewModel(parentExperiment, parent, definitionNode, parentXPath, configNode);
                case XMLConfig.doubleNodeTag: return new DoubleValueConfigViewModel(parentExperiment, parent, definitionNode, parentXPath, configNode);
                case XMLConfig.stringNodeTag: return new StringValueConfigViewModel(parentExperiment, parent, definitionNode, parentXPath, configNode);
                case XMLConfig.filePathNodeTag: return new FilePathValueConfigViewModel(parentExperiment, parent, definitionNode, parentXPath, configNode);
                case XMLConfig.dirPathNodeTag: return new DirPathValueConfigViewModel(parentExperiment, parent, definitionNode, parentXPath, configNode);

                case XMLConfig.stateVarRefTag: return new WorldVarRefValueConfigViewModel(parentExperiment, WorldVarType.StateVar, parent, definitionNode, parentXPath, configNode);
                case XMLConfig.actionVarRefTag: return new WorldVarRefValueConfigViewModel(parentExperiment, WorldVarType.ActionVar, parent, definitionNode, parentXPath, configNode);

                case XMLConfig.branchNodeTag: return new BranchConfigViewModel(parentExperiment, parent, definitionNode, parentXPath, configNode);
                case XMLConfig.choiceNodeTag: return new ChoiceConfigViewModel(parentExperiment, parent, definitionNode, parentXPath, configNode);
                case XMLConfig.choiceElementNodeTag: return new ChoiceElementConfigViewModel(parentExperiment, parent, definitionNode, parentXPath, configNode);
                case XMLConfig.enumNodeTag: return new EnumeratedValueConfigViewModel(parentExperiment, parent, definitionNode, parentXPath, configNode);
                case XMLConfig.multiValuedNodeTag: return new MultiValuedConfigViewModel(parentExperiment, parent, definitionNode, parentXPath, configNode);
            }

            return null;
        }

        public virtual int getNumForkCombinations()
        {
            return 1;
        }
        //we do nothing by default
        public virtual void setForkCombination(ref int id, ref string combinationName) { }


        //Lambda Traverse functions
        public virtual int traverseRetInt(Func<ConfigNodeViewModel, int> simpleNodeFunc
            , Func<ConfigNodeViewModel, int> nestedNodeFunc)
        {
            return simpleNodeFunc(this);
        }
        public virtual void traverseDoAction(Action<ConfigNodeViewModel> simpleNodeAction
            , Action<NestedConfigNode> nestedNodeAction)
        {
            simpleNodeAction(this);
        }

        public virtual void onRemoved()
        { }
    }
    abstract public class NestedConfigNode : ConfigNodeViewModel
    {
        //Children
        protected BindableCollection<ConfigNodeViewModel> m_children = new BindableCollection<ConfigNodeViewModel>();
        public BindableCollection<ConfigNodeViewModel> children
        {
            get { return m_children; }
            set { m_children = value; NotifyOfPropertyChange(() => children); }
        }

        public override void outputXML(StreamWriter writer, SaveMode mode, string leftSpace)
        {
            if (mode == SaveMode.AsExperiment || mode == SaveMode.AsExperimentalUnit)
                writer.Write(leftSpace + getXMLHeader());
            //output children. If we are only exporting forks, we don't want to add indentations
            if (mode == SaveMode.ForkHierarchy || mode == SaveMode.ForkValues)
                outputChildrenXML(writer, mode, leftSpace);
            else
                outputChildrenXML(writer, mode, leftSpace + "\t");

            if (mode == SaveMode.AsExperiment || mode == SaveMode.AsExperimentalUnit)
                writer.Write(leftSpace + getXMLFooter());
        }

        public void outputChildrenXML(StreamWriter writer, SaveMode mode, string leftSpace)
        {
            foreach (ConfigNodeViewModel child in m_children)
                child.outputXML(writer, mode, leftSpace);
        }
        public virtual string getXMLHeader() { return "<" + name + ">\n"; }
        public virtual string getXMLFooter() { return "</" + name + ">\n"; }

        protected void childrenInit(ExperimentViewModel parentExperiment, XmlNode classDefinition
            , string parentXPath, XmlNode configNode = null)
        {
            ConfigNodeViewModel childNode;
            XmlNode forkNode;
            if (classDefinition != null)
            {
                foreach (XmlNode child in classDefinition.ChildNodes)
                {
                    forkNode = getForkChild(child.Attributes[XMLConfig.nameAttribute].Value, configNode);
                    if (forkNode != null)
                    {
                        children.Add(new ForkedNodeViewModel(parentExperiment, this, child, forkNode));
                    }
                    else
                    {
                        childNode = ConfigNodeViewModel.getInstance(parentExperiment, this, child
                            , parentXPath, configNode);
                        if (childNode != null)
                            children.Add(childNode);
                    }
                }
            }
        }

        //this method is used to search for fork nodes
        private XmlNode getForkChild(string childName, XmlNode configNode)
        {
            if (configNode == null) return null;

            foreach (XmlNode configChildNode in configNode)
            {
                if (configChildNode.Name == XMLConfig.forkedNodeTag
                    && configChildNode.Attributes[XMLConfig.nameAttribute].Value == childName)
                {
                    return configChildNode;
                }
            }
            return null;
        }
        public override void forkChild(ConfigNodeViewModel forkedChild)
        {
            ForkedNodeViewModel newForkNode;
            if (m_parentExperiment != null)
            {
                //cross-reference
                newForkNode = new ForkedNodeViewModel(m_parentExperiment, forkedChild);

                int oldIndex = children.IndexOf(forkedChild);
                if (oldIndex >= 0)
                {
                    children.Remove(forkedChild);
                    children.Insert(oldIndex, newForkNode);
                }
            }
        }

        public override bool validate()
        {
            bIsValid = true;
            foreach (ConfigNodeViewModel child in children)
            {
                if (!child.validate())
                {
                    bIsValid = false;
                }
            }
            return bIsValid;
        }

        //Lambda Traverse functions
        public override int traverseRetInt(Func<ConfigNodeViewModel, int> simpleNodeFunc
            , Func<ConfigNodeViewModel, int> nestedNodeFunc)
        {
            return nestedNodeFunc(this);
        }
        public override void traverseDoAction(Action<ConfigNodeViewModel> simpleNodeFunc
            , Action<NestedConfigNode> nestedNodeAction)
        {
            nestedNodeAction(this);
        }

        public override int getNumForkCombinations()
        {
            int numForkCombinations = 1;
            foreach (ConfigNodeViewModel child in children)
                numForkCombinations *= child.getNumForkCombinations();
            return numForkCombinations;
        }
        public override void setForkCombination(ref int id, ref string combinationName)
        {
            foreach (ConfigNodeViewModel child in children)
                child.setForkCombination(ref id, ref combinationName);
        }

        public override void onRemoved()
        {
            foreach (ConfigNodeViewModel child in children) child.onRemoved();
        }
    }

}
