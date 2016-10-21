using System.Xml;
using System.Collections.Generic;
using Caliburn.Micro;
using System.IO;
using Simion;
using Badger.Data;
using System;

namespace Badger.ViewModels
{
    //two modes:
    //-CombineForks: for each combination of fork values, a different experiment will be saved
    //-SaveForks: forkedNodes and forks will be saved as a unique experiment
    public enum SaveMode { CombineForks,SaveForks};

    public class AppViewModel: PropertyChangedBase
    {
        public SaveMode saveMode= SaveMode.SaveForks;

        //deferred load step
        public delegate void deferredLoadStep();

        private List<deferredLoadStep> m_deferredLoadSteps= new List<deferredLoadStep>();
        public void registerDeferredLoadStep(deferredLoadStep func) { m_deferredLoadSteps.Add(func); }
        public void doDeferredLoadSteps()
        {
            foreach (deferredLoadStep deferredStep in m_deferredLoadSteps)
                deferredStep();
            m_deferredLoadSteps.Clear();
        }

        //app properties: prerrequisites, exe files, definitions...
        private List<string> m_preFiles= new List<string>();
        public List<string> getPrerrequisites() { return m_preFiles; }
        private string m_exeFile;
        public string getExeFilename() { return m_exeFile; }
        private Dictionary<string,XmlNode> m_classDefinitions = new Dictionary<string, XmlNode>();
        private Dictionary<string,List<string>> m_enumDefinitions 
            = new Dictionary<string, List<string>>();
        private XmlDocument m_appDefinitionDoc = new XmlDocument();

        public XmlNode getClassDefinition(string className)
        {
            return m_classDefinitions[className];
        }

        public void addEnumeratedType(XmlNode definition)
        {
            List<string> enumeratedValues = new List<string>();
            foreach (XmlNode child in definition)
            {
                enumeratedValues.Add(child.InnerText);
            }

            m_enumDefinitions.Add(definition.Attributes[XMLConfig.nameAttribute].Value
                ,enumeratedValues);
        }
        public List<string> getEnumeratedType(string enumName)
        {
            return m_enumDefinitions[enumName];
        }

        //the app node's name: RLSimion, ...
        private string m_appName;
        public string appName { get { return m_appName; }set { m_appName = value; NotifyOfPropertyChange(() => appName); } }
        //experiment's name
        private string m_name;
        public string name
        {
            get { return m_name; }
            set { m_name = WindowViewModel.getValidAppName(value); NotifyOfPropertyChange(() => name); }
        }
        //file name (not null if it has been saved)
        private string m_fileName= null;
        public string fileName
        {
            get { return m_fileName; }
            set { m_fileName = value; NotifyOfPropertyChange(() => fileName); }
        }

        private string m_version;

        private BindableCollection<ConfigNodeViewModel> m_children= new BindableCollection<ConfigNodeViewModel>();
        public BindableCollection<ConfigNodeViewModel> children { get { return m_children; }
            set { m_children = value; NotifyOfPropertyChange(() => children); } }

        //Auxiliary XML definition files: Worlds (states and actions)
        private Dictionary<string, List<string>> m_auxDefinitions = new Dictionary<string, List<string>>();

        public void loadAuxDefinitions(string fileName)
        {
            XmlDocument doc = new XmlDocument();

            doc.Load(fileName);
            XmlNode rootNode= doc.LastChild; //we take here the last node to skip the <?xml ...> initial tag
            //HARD-CODED: the list is filled with the contents of the nodes from: ../<hangingFrom>/Variable/Name
            foreach (XmlNode child in rootNode.ChildNodes)
            {
                List<string> definedValues = new List<string>();
                string hangingFrom = child.Name;
                foreach (XmlNode child2 in child.ChildNodes)
                {
                    if (child2.Name == "Variable")
                    {
                        foreach (XmlNode child3 in child2.ChildNodes)
                        {
                            if (child3.Name == "Name")
                                definedValues.Add(child3.InnerText);
                        }
                    }
                }
                m_auxDefinitions[hangingFrom] = definedValues;
            }
            updateXMLDefRefs();
        }
        public List<string> getAuxDefinition(string hangingFrom)
        {
            return m_auxDefinitions[hangingFrom];
        }

        //XMLDefRefs
        private List<deferredLoadStep> m_XMLDefRefListeners = new List<deferredLoadStep>();
        public void registerXMLDefRef(deferredLoadStep func)
        { m_XMLDefRefListeners.Add(func); }
        public void updateXMLDefRefs()
        {
            foreach (deferredLoadStep func in m_XMLDefRefListeners)
                func();
        }

        //This constructor builds the whole tree of ConfigNodes either
        // -with default values ("New")
        // -with a configuration file ("Load")
        public AppViewModel(string fileName,string configFilename= null)
        {
            //Load the configFile if a configFilename is provided
            XmlDocument configDoc = new XmlDocument();
            if (configFilename != null) configDoc.Load(configFilename);

            m_appDefinitionDoc.Load(fileName);

            foreach (XmlNode rootChild in m_appDefinitionDoc.ChildNodes)
            {
                if (rootChild.Name == XMLConfig.appNodeTag)
                {
                    //APP node
                    m_preFiles.Clear();
                    m_appName = rootChild.Attributes[XMLConfig.nameAttribute].Value;

                    if (configFilename != null)
                        //if a config file is provided, the name of the experiment is the filename without extension
                        name = Utility.getFileName(configFilename, true);
                    else
                        name = "New";
                    if (rootChild.Attributes.GetNamedItem(XMLConfig.versionAttribute) != null)
                        m_version = rootChild.Attributes[XMLConfig.versionAttribute].Value;
                    else m_version = "0";

                    foreach (XmlNode child in rootChild.ChildNodes)
                    {
                        //Only EXE, PRE, INCLUDE and BRANCH children nodes
                        if (child.Name == XMLConfig.exeNodeTag) m_exeFile= child.InnerText;
                        else if (child.Name == XMLConfig.preNodeTag) m_preFiles.Add(child.InnerText);
                        else if (child.Name == XMLConfig.includeNodeTag)
                            loadIncludedDefinitionFile(child.InnerText);
                        else
                        {
                            XmlNode configRootNode = null;
                            if (configFilename != null)
                            {
                                configRootNode = configDoc.LastChild;
                            }
                            children.Add(ConfigNodeViewModel.getInstance(this, null,child, m_appName, configRootNode));
                            //here we assume definitions are before the children
                        }
                    }
                }
            }
            //deferred load step: enumerated types
            doDeferredLoadSteps();
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
                            m_classDefinitions.Add(name, definition);
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

        public void save(string filename,SaveMode mode)
        {
            saveMode = mode;
            using (FileStream stream = File.Create(filename))
            {
                using (StreamWriter writer = new StreamWriter(stream))
                {
                    writer.Write("<" + appName + " Version=\"" + m_version + "\">\n");

                    foreach (ConfigNodeViewModel node in m_children)
                    {
                        node.outputXML(writer,"  ");
                    }
                    if (saveMode == SaveMode.SaveForks)
                    {
                        foreach (ForkViewModel fork in m_forks)
                        {
                            fork.outputXML(writer, "  ");
                        }
                    }
                    writer.Write( "</" + appName + ">");
                }
            }
        }


        public string getLogDescriptorsFilePath()
        {
            if (m_fileName != "")
            {
                //the hard way because the elegant way didn't seem to work
                int lastPos1, lastPos2, lastPos;
                lastPos1 = m_fileName.LastIndexOf("/");
                lastPos2 = m_fileName.LastIndexOf("\\");

                lastPos = Math.Max(lastPos1, lastPos2);
                if (lastPos > 0)
                {
                    string directory = m_fileName.Substring(0, lastPos + 1);

                    return directory + "experiment-log.xml";
                }
            }
            return "";
        }
        public string getLogFilePath()
        {
            if (m_fileName != "")
            {
                //the hard way because the elegant way didn't seem to work
                int lastPos1, lastPos2, lastPos;
                lastPos1 = m_fileName.LastIndexOf("/");
                lastPos2 = m_fileName.LastIndexOf("\\");

                lastPos = Math.Max(lastPos1, lastPos2);
                if (lastPos > 0)
                {
                    string directory = m_fileName.Substring(0, lastPos + 1);

                    return directory + "experiment-log.bin";
                }
            }
            return "";
        }
        private bool m_bLogDataAvailable = false;
        public bool bLogDataAvailable
        {
            get { return m_bLogDataAvailable; }
            set { m_bLogDataAvailable = value; NotifyOfPropertyChange(() => bLogDataAvailable); }
        }
        public bool checkLogFilesAlreadyExist()
        {
            //for now, i'd rather hardcode the log filenames than go through the dll... doesn't seem worth the effort
            if (m_fileName != "")
            {
                if (File.Exists(getLogDescriptorsFilePath()) && File.Exists(getLogFilePath()))
                {
                    bLogDataAvailable = true;
                    return true;
                }
            }
            return false;
        }

        //FORKS
        private BindableCollection<ForkViewModel> m_forks = new BindableCollection<ForkViewModel>();
        public BindableCollection<ForkViewModel> forks
        {
            get { return m_forks; }
            set { m_forks = value; NotifyOfPropertyChange(() => forks); }
        }

        public ForkViewModel addFork(ConfigNodeViewModel forkedNode,ForkedNodeViewModel forkNode)
        {
            ForkViewModel newFork;

            newFork= new ForkViewModel(this,forkedNode, forkNode);
            forks.Add(newFork);
            NotifyOfPropertyChange(() => forks);
            return newFork;
        }
    }


}
