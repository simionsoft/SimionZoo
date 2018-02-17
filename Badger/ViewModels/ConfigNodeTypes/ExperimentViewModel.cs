using System.Xml;
using System.Collections.Generic;
using Caliburn.Micro;
using System.IO;
using Badger.Simion;
using Badger.Data;
using System;
using System.Linq;

namespace Badger.ViewModels
{
    /// <summary>
    /// Save modes:
    ///     - AsExperiment: all forks and all values are saved.
    ///     - AsExperimentalUnit: for each combination of fork values, a different experimental unit
    ///         will be saved, meaning no forked nodes will be actually saved, but only the currently
    ///         selected value.
    ///     - AsProject: in a single file, all the forks, values and the rest of nodes are saved.
    ///     - ForkValues: only the selected values for each fork are saved.
    ///     - ForkHierarchy: forkedNodes and forks will be saved as a unique experiment.
    ///     - Prerequisites: list of files' names that are necessary to execute an experiment.
    /// </summary>
    public enum SaveMode { AsExperiment, AsExperimentalUnit, AsProject, ForkValues, ForkHierarchy, Prerequisites };

    public enum WorldVarType { StateVar, ActionVar, Constant };

    public class ExperimentViewModel : PropertyChangedBase
    {
        public SaveMode saveMode = SaveMode.AsExperiment;

        //deferred load step
        public delegate void deferredLoadStep();

        private List<deferredLoadStep> m_deferredLoadSteps = new List<deferredLoadStep>();

        public void registerDeferredLoadStep(deferredLoadStep func) { m_deferredLoadSteps.Add(func); }

        public void doDeferredLoadSteps()
        {
            foreach (deferredLoadStep deferredStep in m_deferredLoadSteps)
                deferredStep();
            m_deferredLoadSteps.Clear();
        }

        //app properties: prerrequisites, exe files, definitions...
        private List<string> m_preFiles = new List<string>();

        public List<string> getPrerrequisites() { return m_preFiles; }

        private string m_exeFile;

        public string getExeFilename() { return m_exeFile; }

        private Dictionary<string, XmlNode> m_classDefinitions = new Dictionary<string, XmlNode>();

        private Dictionary<string, List<string>> m_enumDefinitions = new Dictionary<string, List<string>>();

        /// <summary>
        /// Rename rules: files that must be stored in the remote machine in a different relative location
        /// 
        /// Example: 64 bit runtime C++ libraries have the same name that 32-bit versions have.
        ///         In the local machine, 64 bit libraries are in /bin/64, 32 libraries are in /bin, but both
        ///         must be in the same directory as the .exe using them, so the 64 dll-s must be saved in /bin in
        ///         the remote machine.
        /// </summary>
        private Dictionary<string, string> m_renameRules = new Dictionary<string, string>();

        public Dictionary<string, string> renameRules { get { return m_renameRules; } }

        public XmlNode getClassDefinition(string className, bool bCanBeNull = false)
        {
            if (!m_classDefinitions.ContainsKey(className))
            {
                if (!bCanBeNull)
                    CaliburnUtility.ShowWarningDialog("Undefined class" + className, "ERROR");
                return null;
            }
            return m_classDefinitions[className];
        }

        public void addEnumeratedType(XmlNode definition)
        {
            List<string> enumeratedValues = new List<string>();

            foreach (XmlNode child in definition)
                enumeratedValues.Add(child.InnerText);

            m_enumDefinitions.Add(definition.Attributes[XMLConfig.nameAttribute].Value, enumeratedValues);
        }

        public List<string> getEnumeratedType(string enumName)
        {
            if (!m_enumDefinitions.ContainsKey(enumName))
            {
                CaliburnUtility.ShowWarningDialog("Undefined enumeratedType: " + enumName, "ERROR");
                return null;
            }
            return m_enumDefinitions[enumName];
        }

        //  the app node's name: RLSimion, ...
        private string m_appName;

        public string appName { get { return m_appName; } set { m_appName = value; NotifyOfPropertyChange(() => appName); } }

        //experiment's name
        private string m_name;

        public string name
        {
            get { return m_name; }
            set { m_name = MainWindowViewModel.getValidAppName(value); NotifyOfPropertyChange(() => name); }
        }

        //file name (not null if it has been saved)
        private string m_fileName;

        public string fileName
        {
            get { return m_fileName; }
            set { m_fileName = value; NotifyOfPropertyChange(() => fileName); }
        }

        private string m_version;

        private BindableCollection<ConfigNodeViewModel> m_children = new BindableCollection<ConfigNodeViewModel>();

        public BindableCollection<ConfigNodeViewModel> children
        {
            get { return m_children; }
            set { m_children = value; NotifyOfPropertyChange(() => children); }
        }

        private Dictionary<string, WorldDefinition> m_worldDefinitions = new Dictionary<string, WorldDefinition>();

        private string m_selectedWorld = "";

        public void selectWorld(string worldName)
        {
            if (m_worldDefinitions.ContainsKey(worldName))
            {
                m_selectedWorld = worldName;
                updateWorldDefinition();
            }
            else CaliburnUtility.ShowWarningDialog("The world selected hasn't been defined. Check for missmatched names in the source code of the app", "ERROR");
        }

        private void parseClassDefinitionMetadata(XmlNode definition)
        {
            string worldName;
            WorldDefinition worldDefinition;
            if (definition.Attributes.GetNamedItem(XMLConfig.worldAttribute) != null)
            {
                worldName = definition.Attributes[XMLConfig.worldAttribute].Value;
                worldDefinition = new WorldDefinition();
                foreach (XmlNode child in definition.ChildNodes)
                {
                    if (child.Name == XMLConfig.stateVarTag)
                        worldDefinition.addStateVar(new StateVar(child));
                    else if (child.Name == XMLConfig.actionVarTag)
                        worldDefinition.addActionVar(new ActionVar(child));
                    else if (child.Name == XMLConfig.constantTag)
                        worldDefinition.addConstant(child.Attributes[XMLConfig.nameAttribute].Value);
                }

                m_worldDefinitions.Add(worldName, worldDefinition);
            }
        }

        public void getWorldVarNameList(WorldVarType varType, ref List<string> varNameList)
        {
            if (varNameList != null && m_selectedWorld != "")
            {
                switch (varType)
                {
                    case WorldVarType.StateVar:
                        m_worldDefinitions[m_selectedWorld].getStateVarNameList(ref varNameList);
                        break;
                    case WorldVarType.ActionVar:
                        m_worldDefinitions[m_selectedWorld].getActionVarNameList(ref varNameList);
                        break;
                    case WorldVarType.Constant:
                        m_worldDefinitions[m_selectedWorld].getConstantNameList(ref varNameList);
                        break;
                }
            }
        }

        //WorldVarRefs
        private List<deferredLoadStep> m_WorldVarRefListeners = new List<deferredLoadStep>();

        public void registerWorldVarRef(deferredLoadStep func)
        { m_WorldVarRefListeners.Add(func); }

        private void updateWorldDefinition()
        {
            foreach (deferredLoadStep func in m_WorldVarRefListeners)
                func();
        }


        public void Initialize(string appDefinitionFileName, XmlNode configRootNode, string experimentName)
        {
            XmlDocument appDefinition = new XmlDocument();
            appDefinition.Load(appDefinitionFileName);

            foreach (XmlNode rootChild in appDefinition.ChildNodes)
            {
                if (rootChild.Name == XMLConfig.appNodeTag)
                {
                    //APP node
                    m_preFiles.Clear();
                    m_appName = rootChild.Attributes[XMLConfig.nameAttribute].Value;

                    name = experimentName;

                    if (rootChild.Attributes.GetNamedItem(XMLConfig.versionAttribute) != null)
                        m_version = rootChild.Attributes[XMLConfig.versionAttribute].Value;
                    else
                    {
                        CaliburnUtility.ShowWarningDialog("Error reading version attribute: "
                            + XMLConfig.experimentConfigVersion, "ERROR");
                        m_version = "0.0.0.0";
                    }

                    foreach (XmlNode child in rootChild.ChildNodes)
                    {
                        //Only EXE, PRE, INCLUDE and BRANCH children nodes
                        if (child.Name == XMLConfig.exeNodeTag)
                            m_exeFile = child.InnerText;
                        else if (child.Name == XMLConfig.preNodeTag)
                        {
                            m_preFiles.Add(child.InnerText);
                            if (child.Attributes.GetNamedItem(XMLConfig.renameAttr) != null)
                            {
                                //add the new rename rule
                                renameRules[child.InnerText] = child.Attributes[XMLConfig.renameAttr].Value;
                            }
                        }
                        else if (child.Name == XMLConfig.includeNodeTag)
                            loadIncludedDefinitionFile(child.InnerText);
                        else
                        {
                            // here we assume definitions are before the children
                            children.Add(ConfigNodeViewModel.getInstance(this, null, child, m_appName, configRootNode));
                        }
                    }
                }
            }

            LinkNodes();
            //deferred load step: enumerated types
            doDeferredLoadSteps();
        }

        /// <summary>
        ///     This constructor builds the whole tree of ConfigNodes either
        ///         - with default values ("New") or
        ///         - with a configuration file ("Load")
        /// </summary>
        /// <param name="appDefinitionFileName"></param>
        /// <param name="configFilename"></param>
        public ExperimentViewModel(string appDefinitionFileName, string configFilename)
        {
            // Load the configFile if a configFilename is provided
            XmlNode configRootNode = null;
            if (configFilename != null)
            {
                var configDoc = new XmlDocument();
                configDoc.Load(configFilename);
                configRootNode = configDoc.LastChild;
            }

            Initialize(appDefinitionFileName, configRootNode, Utility.GetFilename(configFilename, true, 2));
            //we remove the two extensions in "simion.exp"
        }

        //This constructor is called when a badger file is loaded. Because all the experiments are embedded within a single
        //XML file, the calling method will be passing XML nodes belonging to the single XML file instead of filenames
        public ExperimentViewModel(string appDefinitionFileName, XmlNode configRootNode, string experimentName)
        {
            Initialize(appDefinitionFileName, configRootNode, experimentName);
        }

        private void loadIncludedDefinitionFile(string appDefinitionFile)
        {
            XmlDocument definitionFile = new XmlDocument();
            definitionFile.Load(appDefinitionFile);
            foreach (XmlNode child in definitionFile.ChildNodes)
            {
                if (child.Name == XMLConfig.definitionNodeTag)
                {
                    foreach (XmlNode definition in child.ChildNodes)
                    {
                        string name = definition.Attributes[XMLConfig.nameAttribute].Value;
                        if (definition.Name == XMLConfig.classDefinitionNodeTag)
                        {
                            parseClassDefinitionMetadata(definition);
                            m_classDefinitions.Add(name, definition);
                        }
                        else if (definition.Name == XMLConfig.enumDefinitionNodeTag)
                            addEnumeratedType(definition);
                    }
                }
            }
        }

        public bool validate()
        {
            foreach (ConfigNodeViewModel node in m_children)
                if (!node.validate()) return false;
            return true;
        }

        //this method saves an experiment. Depending on the mode:
        //  -SaveMode.AsExperimentUnit -> the caller should iterate on i= [0..getNumForkCombinations) and call
        //setCombination(i). This method will then save the i-th combination
        //  -SaveMode.AsExperiment -> this method should be called only once per experiment. All the forks will be saved embedded
        //in the config file
        //  -SaveMode.AsExperimentBatch -> this method is called from SaveExperimentBatchFile and saves only the information related to forks
        //   We need this to know later which value were given to each fork
        public void save(string filename, SaveMode mode, string leftSpace = "")
        {
            using (StreamWriter writer = new StreamWriter(filename))
            {
                saveToStream(writer, mode, leftSpace);
            }
        }

        public void saveToStream(StreamWriter writer, SaveMode mode, string leftSpace)
        {
            saveMode = mode;
            // Header for experiments and experiment units
            if (mode == SaveMode.AsExperiment || mode == SaveMode.AsExperimentalUnit)
            {
                //We are saving the config file as an experiment, experiment unit, or a badger file
                writer.WriteLine(leftSpace + "<" + appName + " " + XMLConfig.versionAttribute
                + "=\"" + XMLConfig.experimentConfigVersion + "\">");
            }

            // Body
            foreach (ConfigNodeViewModel node in m_children)
                node.outputXML(writer, mode, leftSpace);

            // Footer for experiments and experiment units
            if (mode == SaveMode.AsExperiment || mode == SaveMode.AsExperimentalUnit)
                writer.WriteLine(leftSpace + "</" + appName + ">");
        }

        public string numForkCombinations
        {
            get { return "(" + getNumForkCombinations() + ")"; }
        }

        public void updateNumForkCombinations()
        {
            NotifyOfPropertyChange(() => numForkCombinations);
        }

        public int getNumForkCombinations()
        {
            int numForkCombinations = 1;
            foreach (ConfigNodeViewModel child in children)
                numForkCombinations *= child.getNumForkCombinations();

            return numForkCombinations;
        }

        public string setForkCombination(int combination)
        {
            string combinationName = name;
            int combinationId = combination;
            foreach (ConfigNodeViewModel child in children)
            {
                child.setForkCombination(ref combinationId, ref combinationName);
            }

            return combinationName;
        }

        //the list of forks hanging directly from the main experiment
        private ForkRegistry m_forkRegistry = new ForkRegistry();


        public ForkRegistry forkRegistry
        {
            get { return m_forkRegistry; }
            set { m_forkRegistry = value; NotifyOfPropertyChange(() => forkRegistry); }
        }

        private ConfigNodeViewModel m_linkOriginNode;


        public ConfigNodeViewModel LinkOriginNode
        {
            get { return m_linkOriginNode; }
            set
            {
                m_linkOriginNode = value;
                NotifyOfPropertyChange(() => LinkOriginNode);
            }
        }

        /// <summary>
        /// Implementation of depth first search algorithm for experiment tree.
        /// </summary>
        /// <param name="targetNode"></param>
        public ConfigNodeViewModel DepthFirstSearch(string nodeName, string alias = "")
        {
            var nodeStack = new Stack<ConfigNodeViewModel>(new[] { children[0] });

            while (nodeStack.Any())
            {
                ConfigNodeViewModel node = nodeStack.Pop();

                if (node.nodeDefinition != null)
                    if (node.nodeDefinition.Attributes[XMLConfig.nameAttribute].Value.Equals(nodeName))
                    {
                        if (node.nodeDefinition.Attributes[XMLConfig.aliasAttribute] != null)
                            if (node.nodeDefinition.Attributes[XMLConfig.aliasAttribute].Value.Equals(alias))
                                return node;

                        if (node.parent is NestedConfigNode)
                            return node;

                        return null;
                    }

                WalkThroughBranch(ref nodeStack, node);
            }

            return null;
        }

        /// <summary>
        /// Implementation of depth first search algorithm for experiment tree.
        /// </summary>
        /// <param name="targetNode"></param>
        public ConfigNodeViewModel DepthFirstSearch(ConfigNodeViewModel targetNode)
        {
            var nodeStack = new Stack<ConfigNodeViewModel>(new[] { children[0] });

            while (nodeStack.Any())
            {
                ConfigNodeViewModel node = nodeStack.Pop();

                if (node.Equals(targetNode))
                    return node;

                WalkThroughBranch(ref nodeStack, node);
            }

            return null;
        }


        private void WalkThroughBranch(ref Stack<ConfigNodeViewModel> nodeStack, ConfigNodeViewModel branchRoot)
        {
            if (branchRoot is NestedConfigNode)
            {
                NestedConfigNode branch = (NestedConfigNode)branchRoot;

                if (branch.children.Count > 0)
                    foreach (var node in branch.children)
                        nodeStack.Push(node);
            }
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="originNode"></param>
        /// <param name="link"></param>
        public void CheckLinkableNodes(ConfigNodeViewModel originNode, bool link = true)
        {
            var nodeStack = new Stack<ConfigNodeViewModel>(new[] { children[0] });

            while (nodeStack.Any())
            {
                ConfigNodeViewModel node = nodeStack.Pop();

                if (link && node.nodeDefinition != null)
                {
                    if (node.nodeDefinition.Name.Equals(originNode.nodeDefinition.Name)
                        && !node.IsLinkOrigin)
                    {
                        node.CanBeLinked = true;
                        node.IsLinkable = false;
                    }
                }
                else
                {
                    node.CanBeLinked = false;
                    node.IsLinkable = true;
                }

                WalkThroughBranch(ref nodeStack, node);
            }
        }

        /// <summary>
        /// Link nodes when experiment is loaded from a file. This has to be done once all nodes
        /// are loaded.
        /// </summary>
        private void LinkNodes()
        {
            var nodeStack = new Stack<ConfigNodeViewModel>(new[] { children[0] });

            while (nodeStack.Any())
            {
                ConfigNodeViewModel node = nodeStack.Pop();
                
                if (node is LinkedNodeViewModel)
                {
                    LinkedNodeViewModel linkedNode = (LinkedNodeViewModel)node;
                    linkedNode.Origin = DepthFirstSearch(linkedNode.OriginName, linkedNode.OriginAlias);

                    linkedNode.createLinkedNode(node);
                    // Add the node to origin linked nodes give the functionality to reflect content
                    // changes of in all linked nodes
                    linkedNode.Origin.LinkedNodes.Add(linkedNode.LinkedNode);

                    if (linkedNode.Origin is ForkedNodeViewModel)
                    {
                        ForkedNodeViewModel forkedOrigin = (ForkedNodeViewModel)linkedNode.Origin;
                        int len = forkedOrigin.children.Count;

                        for (int i = 0; i < len; i++)
                        {
                            ForkedNodeViewModel linkedFork = (ForkedNodeViewModel)linkedNode.LinkedNode;
                            ForkValueViewModel linkedForkValue = (ForkValueViewModel)linkedFork.children[i];
                            ((ForkValueViewModel)forkedOrigin.children[i]).configNode.LinkedNodes
                                .Add(linkedForkValue.configNode);
                            linkedForkValue.configNode.name = linkedNode.name;
                            linkedForkValue.configNode.comment = linkedNode.comment;
                            linkedForkValue.configNode.nodeDefinition = linkedNode.nodeDefinition;
                        }
                    }
                }

                WalkThroughBranch(ref nodeStack, node);
            }
        }
    }
}
