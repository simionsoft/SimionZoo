/*
	SimionZoo: A framework for online model-free Reinforcement Learning on continuous
	control problems

	Copyright (c) 2016 SimionSoft. https://github.com/simionsoft

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

using System.Xml;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System;
using System.Diagnostics;

using Caliburn.Micro;

using Badger.Data;

using Herd.Files;

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

        public void RegisterDeferredLoadStep(deferredLoadStep func) { m_deferredLoadSteps.Add(func); }

        public void DoDeferredLoadSteps()
        {
            foreach (deferredLoadStep deferredStep in m_deferredLoadSteps)
                deferredStep();
            m_deferredLoadSteps.Clear();
        }

        private Dictionary<string, XmlNode> m_classDefinitions = new Dictionary<string, XmlNode>();

        private Dictionary<string, List<string>> m_enumDefinitions = new Dictionary<string, List<string>>();

        public XmlNode GetClassDefinition(string className, bool bCanBeNull = false)
        {
            if (!m_classDefinitions.ContainsKey(className))
            {
                if (!bCanBeNull)
                    CaliburnUtility.ShowWarningDialog("Undefined class" + className, "ERROR");
                return null;
            }
            return m_classDefinitions[className];
        }

        public void AddEnumeratedType(XmlNode definition)
        {
            List<string> enumeratedValues = new List<string>();

            foreach (XmlNode child in definition)
                enumeratedValues.Add(child.InnerText);

            m_enumDefinitions.Add(definition.Attributes[XMLTags.nameAttribute].Value, enumeratedValues);
        }

        public List<string> GetEnumeratedType(string enumName)
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

        public string AppName { get { return m_appName; } set { m_appName = value; NotifyOfPropertyChange(() => AppName); } }

        //experiment's name
        private string m_name;

        public string Name
        {
            get { return m_name; }
            set { m_name = MainWindowViewModel.Instance.EditorWindowVM.getValidAppName(value);
                NotifyOfPropertyChange(() => Name); }
        }

        //file name (not null if it has been saved)
        private string m_fileName;

        public string FileName
        {
            get { return m_fileName; }
            set { m_fileName = value; NotifyOfPropertyChange(() => FileName); }
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

        public void SelectWorld(string worldName)
        {
            if (m_worldDefinitions.ContainsKey(worldName))
            {
                m_selectedWorld = worldName;
                UpdateWorldReferences();
            }
            else CaliburnUtility.ShowWarningDialog("The world selected hasn't been defined. Check for missmatched names in the source code of the app", "ERROR");
        }

        private void ParseClassDefinitionMetadata(XmlNode definition)
        {
            string worldName;
            WorldDefinition worldDefinition;
            if (definition.Attributes.GetNamedItem(XMLTags.worldAttribute) != null)
            {
                worldName = definition.Attributes[XMLTags.worldAttribute].Value;
                worldDefinition = new WorldDefinition();
                foreach (XmlNode child in definition.ChildNodes)
                {
                    if (child.Name == XMLTags.stateVarTag)
                        worldDefinition.AddStateVar(new StateVar(child));
                    else if (child.Name == XMLTags.actionVarTag)
                        worldDefinition.AddActionVar(new ActionVar(child));
                    else if (child.Name == XMLTags.constantTag)
                        worldDefinition.AddConstant(child.Attributes[XMLTags.nameAttribute].Value);
                }

                m_worldDefinitions.Add(worldName, worldDefinition);
            }
        }

        public List<string> GetWorldVarNameList(WorldVarType varType)
        {
            List<string> outList = new List<string>();
            switch (varType)
            {
                case WorldVarType.StateVar:
                    if (m_selectedWorld != "")
                        outList.AddRange(m_worldDefinitions[m_selectedWorld].GetStateVarNameList());
                    outList.AddRange(m_wiresViewModel.GetWireNames());
                    break;
                case WorldVarType.ActionVar:
                    if (m_selectedWorld != "")
                        outList.AddRange(m_worldDefinitions[m_selectedWorld].GetActionVarNameList());
                    outList.AddRange(m_wiresViewModel.GetWireNames());
                    break;
                case WorldVarType.Constant:
                    if (m_selectedWorld != "")
                        outList.AddRange(m_worldDefinitions[m_selectedWorld].GetConstantNameList());
                    break;
            }
            return outList;
        }

        //WorldVarRefs
        private List<deferredLoadStep> m_WorldVarRefListeners = new List<deferredLoadStep>();

        public void RegisterWorldVarRef(deferredLoadStep func) { m_WorldVarRefListeners.Add(func); }

        public void UpdateWorldReferences()
        {
            foreach (deferredLoadStep func in m_WorldVarRefListeners)
                func();
        }

        //App versions
        List<AppVersion> m_appVersions = new List<AppVersion>();

        public List<AppVersion> AppVersions { get { return m_appVersions; } }

        public void Initialize(string appDefinitionFileName, XmlNode configRootNode, string experimentName)
        {
            //Create wires
            m_wiresViewModel = new WiresViewModel(this);

            XmlDocument appDefinition = new XmlDocument();
            appDefinition.Load(appDefinitionFileName);

            foreach (XmlNode rootChild in appDefinition.ChildNodes)
            {
                if (rootChild.Name == XMLTags.AppNodeTag)
                {
                    //APP node
                    m_appName = rootChild.Attributes[XMLTags.nameAttribute].Value;

                    Name = experimentName;

                    if (rootChild.Attributes.GetNamedItem(XMLTags.versionAttribute) != null)
                        m_version = rootChild.Attributes[XMLTags.versionAttribute].Value;
                    else
                    {
                        CaliburnUtility.ShowWarningDialog("Error reading version attribute: "
                            + XMLTags.ExperimentConfigVersion, "ERROR");
                        m_version = "0.0.0.0";
                    }

                    foreach (XmlNode child in rootChild.ChildNodes)
                    {
                        if (child.Name == Herd.Network.XmlTags.Version)
                            m_appVersions.Add(new AppVersion(child));
                        else if (child.Name == Herd.Network.XmlTags.Include)
                            LoadIncludedDefinitionFile(child.InnerText);
                        else
                        {
                            // here we expect definitions before any children that uses them
                            children.Add(ConfigNodeViewModel.getInstance(this, null, child, m_appName, configRootNode));
                        }
                    }
                }
            }

            LinkNodes();
            //deferred load step: enumerated types
            DoDeferredLoadSteps();
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

            //Remove the extensions of the experiment file to give name to the experiment
            int numExtensions = Herd.Utils.NumParts(Extensions.Project, '.');
            Initialize(appDefinitionFileName, configRootNode, Herd.Utils.GetFilename(configFilename, true, numExtensions));
        }

        //This constructor is called when a badger file is loaded. Because all the experiments are embedded within a single
        //XML file, the calling method will be passing XML nodes belonging to the single XML file instead of filenames
        public ExperimentViewModel(string appDefinitionFileName, XmlNode configRootNode, string experimentName)
        {
            Initialize(appDefinitionFileName, configRootNode, experimentName);
        }

        private void LoadIncludedDefinitionFile(string appDefinitionFile)
        {
            XmlDocument definitionFile = new XmlDocument();
            definitionFile.Load(appDefinitionFile);
            foreach (XmlNode child in definitionFile.ChildNodes)
            {
                if (child.Name == XMLTags.DefinitionNodeTag)
                {
                    foreach (XmlNode definition in child.ChildNodes)
                    {
                        string name = definition.Attributes[XMLTags.nameAttribute].Value;
                        if (definition.Name == XMLTags.ClassDefinitionNodeTag)
                        {
                            ParseClassDefinitionMetadata(definition);
                            m_classDefinitions.Add(name, definition);
                        }
                        else if (definition.Name == XMLTags.EnumDefinitionNodeTag)
                            AddEnumeratedType(definition);
                    }
                }
            }
        }

        WiresViewModel m_wiresViewModel;
        
        /// <summary>
        /// Shows a new window with the wires used in the experiment
        /// </summary>
        public void ShowWires()
        {
            CaliburnUtility.ShowPopupWindow(m_wiresViewModel, "Wires");
            UpdateWireConnections();
            UpdateWorldReferences(); //state and action variables may reference a wire too
        }
        public WireViewModel GetWire(string name)
        {
            foreach (WireViewModel wire in m_wiresViewModel.Wires)
                if (name == wire.Name)
                    return wire;
            return null;
        }
        public void GetWireNames(ref List<string> wireNames)
        {
            foreach (WireViewModel wire in m_wiresViewModel.Wires)
                wireNames.Add(wire.Name);
        }

        public void AddWire(string name, double minimum, double maximum)
        {
            m_wiresViewModel.AddWire(name, minimum, maximum);
        }
        public void AddWire(string name)
        {
            m_wiresViewModel.AddWire(name);
        }

        private List<deferredLoadStep> m_WireConnections = new List<deferredLoadStep>();

        public void RegisterWireConnection(deferredLoadStep func) { m_WireConnections.Add(func); }

        public void UpdateWireConnections()
        {
            foreach (deferredLoadStep func in m_WireConnections)
                func();
        }


        //Validation
        public bool Validate()
        {
            foreach (ConfigNodeViewModel node in m_children)
                if (!node.Validate()) return false;
            return true;
        }

        //Runs the experiment with the currently selected fork values locally.
        //The experiment is saved in a temporary folder
        public void RunLocallyCurrentConfiguration()
        {
            //we save the experiment with the currently selected fork values
            int fileId = new Random().Next(1, 1000);
            string filename = Folders.tempRelativeDir + fileId.ToString() 
                + Extensions.Experiment;

            if (!Directory.Exists(Folders.tempRelativeDir))
                Directory.CreateDirectory(Folders.tempRelativeDir);

            Save(filename, SaveMode.AsExperimentalUnit);
            AppVersion bestMatch = AppVersion.BestMatch(m_appVersions);

            if (bestMatch != null)
            {
                Process proc = new Process()
                {
                    StartInfo = new ProcessStartInfo
                    {
                        FileName = bestMatch.ExeFile,
                        Arguments = filename + " -local",
                        UseShellExecute = false,
                        RedirectStandardOutput = false,
                        CreateNoWindow = true
                    }
                };
                proc.Start();
            }
            else CaliburnUtility.ShowWarningDialog("The local machine doesn't mach the requirements of any of the app versions", "ERROR");
        }

        //this method saves an experiment. Depending on the mode:
        //  -SaveMode.AsExperimentUnit -> the caller should iterate on i= [0..getNumForkCombinations) and call
        //setCombination(i). This method will then save the i-th combination
        //  -SaveMode.AsExperiment -> this method should be called only once per experiment. All the forks will be saved embedded
        //in the config file
        //  -SaveMode.AsExperimentBatch -> this method is called from SaveExperimentBatchFile and saves only the information related to forks
        //   We need this to know later which value were given to each fork
        public void Save(string filename, SaveMode mode, string leftSpace = "")
        {
            using (StreamWriter writer = new StreamWriter(filename))
            {
                SaveToStream(writer, mode, leftSpace);
            }
        }

        public void SaveToStream(StreamWriter writer, SaveMode mode, string leftSpace)
        {
            saveMode = mode;
            // Header for experiments and experiment units
            if (mode == SaveMode.AsExperiment || mode == SaveMode.AsExperimentalUnit)
            {
                //We are saving the config file as an experiment, experiment unit, or a badger file
                writer.WriteLine(leftSpace + "<" + AppName + " " + XMLTags.versionAttribute
                + "=\"" + XMLTags.ExperimentConfigVersion + "\">");
            }

            // Body
            foreach (ConfigNodeViewModel node in m_children)
                node.outputXML(writer, mode, leftSpace);

            // Footer for experiments and experiment units
            if (mode == SaveMode.AsExperiment || mode == SaveMode.AsExperimentalUnit)
                writer.WriteLine(leftSpace + "</" + AppName + ">");
        }

        public string NumForkCombinations
        {
            get { return "(" + getNumForkCombinations() + ")"; }
        }

        public void updateNumForkCombinations()
        {
            NotifyOfPropertyChange(() => NumForkCombinations);
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
            string combinationName = Name;
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



        //Links
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
                    if (node.nodeDefinition.Attributes[XMLTags.nameAttribute].Value.Equals(nodeName))
                    {
                        if (node.nodeDefinition.Attributes[XMLTags.aliasAttribute] != null)
                            if (node.nodeDefinition.Attributes[XMLTags.aliasAttribute].Value.Equals(alias))
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
                        if (node.nodeDefinition.Name.Equals("BRANCH"))
                        {
                            if (((BranchConfigViewModel)node).ClassName.Equals(
                                ((BranchConfigViewModel)originNode).ClassName))
                            {
                                node.CanBeLinked = true;
                                node.IsLinkable = false;
                            }
                        }
                        else
                        {
                            node.CanBeLinked = true;
                            node.IsLinkable = false;
                        }
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

                    linkedNode.CreateLinkedNode();
                    linkedNode.LinkedNode.IsLinkable = false;
                    linkedNode.LinkedNode.IsLinked = true;
                    linkedNode.LinkedNode.IsNotLinked = false;
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
