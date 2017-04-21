using System.Xml;
using System.Collections.Generic;
using Caliburn.Micro;
using Badger.Simion;
using Badger.Data;

namespace Badger.ViewModels
{
    public class LoggedExperimentalUnitViewModel : SelectableTreeItem
    {
        private string m_logFilePath = "";
        public string logFilePath { get { return m_logFilePath; } set { m_logFilePath = value; } }

        private string m_logDescriptorFilePath = "";
        public string logDescriptorFilePath { get { return m_logDescriptorFilePath; } set { m_logDescriptorFilePath = value; } }

        private string m_experimentFilePath = "";
        public string experimentFilePath { get { return m_experimentFilePath; } set { m_experimentFilePath = value; } }

        private string m_name;
        public string name
        {
            get { return m_name; }
            set { m_name = value; NotifyOfPropertyChange(() => name); }
        }

        private XmlDocument m_logDescriptor;
        public XmlDocument logDescriptor
        {
            get { return m_logDescriptor; }
            set { m_logDescriptor = value; }
        }

        private Dictionary<string, string> m_forkValues = new Dictionary<string, string>();
        public Dictionary<string, string> forkValues
        {
            get { return m_forkValues; }
            set { m_forkValues = value; NotifyOfPropertyChange(() => forkValues); }
        }

        private List<string> m_variablesInLog = new List<string>();

        public LoggedExperimentalUnitViewModel(XmlNode configNode, Window parent)
        {
            m_parentWindow = parent;

            if (configNode.Attributes.GetNamedItem(XMLConfig.nameAttribute) != null)
                name = configNode.Attributes[XMLConfig.nameAttribute].Value;
            if (configNode.Attributes.GetNamedItem(XMLConfig.pathAttribute) != null)
                experimentFilePath = configNode.Attributes[XMLConfig.pathAttribute].Value;
            logDescriptorFilePath = SimionFileData.getLogDescriptorsFilePath(experimentFilePath);
            logFilePath = SimionFileData.getLogFilePath(experimentFilePath);

            if (parent is ReportsWindowViewModel)
            {
                logDescriptor = new XmlDocument();
                logDescriptor.Load(logDescriptorFilePath);
                processDescriptor();
            }

            //load the value of each fork used in this experimental unit
            foreach (XmlNode fork in configNode.ChildNodes)
            {
                string forkName = fork.Attributes[XMLConfig.aliasAttribute].Value;
                foreach (XmlNode value in fork.ChildNodes)
                {
                    string forkValue = value.Attributes.GetNamedItem("Value").InnerText; // The value is in the attribute named "Value"
                    forkValues[forkName] = forkValue;
                }
            }
        }

        private void processDescriptor()
        {
            XmlNode node = m_logDescriptor.FirstChild;
            if (node.Name == XMLConfig.descriptorRootNodeName)
            {
                foreach (XmlNode child in node.ChildNodes)
                {
                    if (child.Name == XMLConfig.descriptorStateVarNodeName
                        || child.Name == XMLConfig.descriptorActionVarNodeName
                        || child.Name == XMLConfig.descriptorRewardVarNodeName
                        || child.Name == XMLConfig.descriptorStatVarNodeName)
                    {
                        string varName = child.InnerText;
                        //add the variable to the local list
                        m_variablesInLog.Add(varName);
                        //add the variable to the list managed by the parent window
                        m_parentWindow.addVariable(varName);
                    }
                }
            }
        }
        //loads the log file and then returns the data of the requested variable as an array of TrackData
        //not the most efficient way, but the easiest right now
        public TrackData loadTrackData(List<string> varNames)
        {
            ExperimentData experimentData = new ExperimentData();
            experimentData.load(m_logFilePath);

            EpisodeData lastEvalEpisode = experimentData.episodes[experimentData.episodes.Count - 1];

            int numSteps = lastEvalEpisode.steps.Count;
            TrackData data = new TrackData(numSteps, experimentData.numEpisodes, varNames);
            data.bSuccesful = experimentData.bSuccesful;
            data.forkValues = forkValues;

            //experiment-long average values
            foreach (EpisodeData episode in experimentData.episodes)
            {
                foreach (string variable in varNames)
                {
                    double avg = 0.0;
                    int variableIndex = m_variablesInLog.IndexOf(variable);
                    TrackVariableData variableData = data.getVariableData(variable);
                    if (variableData != null && episode.steps.Count > 0)
                    {
                        foreach (StepData step in episode.steps)
                        {
                            avg += step.data[variableIndex];
                        }
                        avg /= episode.steps.Count;
                        variableData.experimentData.values[episode.index - 1] = avg;
                    }
                }
            }

            //last evaluation Episode
            int i = 0;
            foreach (StepData step in lastEvalEpisode.steps)
            {
                data.realTime[i] = step.episodeRealTime;
                data.simTime[i] = step.episodeSimTime;
                foreach (string variable in varNames)
                {
                    int variableIndex = m_variablesInLog.IndexOf(variable);
                    TrackVariableData variableData = data.getVariableData(variable);
                    if (variableData != null)
                        variableData.lastEpisodeData.values[i] = step.data[variableIndex];
                }
                ++i;
            }
            //calculate each variable's last episode stats
            foreach (string variable in varNames)
            {
                TrackVariableData variableData = data.getVariableData(variable);
                if (variableData != null)
                    variableData.calculateStats();
            }

            return data;
        }
    }
}
