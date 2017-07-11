using System.Xml;
using System.Collections.Generic;
using Badger.Simion;
using Badger.Data;
using System;

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

        public LoggedExperimentalUnitViewModel(XmlNode configNode, string baseDirectory)
        {
            if (configNode.Attributes != null)
            {
                if (configNode.Attributes.GetNamedItem(XMLConfig.nameAttribute) != null)
                    name = configNode.Attributes[XMLConfig.nameAttribute].Value;

                if (configNode.Attributes.GetNamedItem(XMLConfig.pathAttribute) != null)
                    experimentFilePath = baseDirectory + configNode.Attributes[XMLConfig.pathAttribute].Value;
            }

            logDescriptorFilePath = SimionFileData.GetLogFilePath(baseDirectory + experimentFilePath);
            logFilePath = SimionFileData.GetLogFilePath(baseDirectory + experimentFilePath, false);

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

        public List<string> processDescriptor()
        {
            List<string> variablesNames = new List<string>();
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
                        // add the variable to the local list
                        m_variablesInLog.Add(varName);
                        variablesNames.Add(varName);
                    }
                }
            }

            return variablesNames;
        }

        //loads the log file and then returns the data of the requested variable as an array of TrackData
        //not the most efficient way, but the easiest right now
        public TrackData loadTrackData(List<string> varNames)
        {
            ExperimentData experimentData = new ExperimentData();
            experimentData.load(m_logFilePath);

            EpisodeData lastEvalEpisode = experimentData.episodes[experimentData.episodes.Count - 1];

            int numSteps = lastEvalEpisode.steps.Count;
            TrackData trackData = new TrackData(numSteps, experimentData.numEpisodes, varNames);
            trackData.bSuccesful = experimentData.bSuccesful;
            trackData.forkValues = forkValues;

            generateExperimentLongAvgValues(trackData, experimentData.episodes, varNames);

            fillTrackDataRealAndSimTime(trackData, lastEvalEpisode.steps);
            generateLastEpisodeData(trackData, lastEvalEpisode.steps, varNames);

            generateAllEvaluationEpisodesValues(trackData, experimentData.episodes, varNames);

            calculateEachVariableStats(trackData, varNames);

            return trackData;
        }

        private void fillTrackDataRealAndSimTime(TrackData trackData, List<StepData> stepList)
        {
            int i = 0;
            foreach (StepData step in stepList)
            {
                trackData.realTime[i] = step.episodeRealTime;
                trackData.simTime[i] = step.episodeSimTime;
                i++;
            }
        }

        private void generateLastEpisodeData(TrackData trackData, List<StepData> stepList, List<string> variablesNames)
        {
            int i = 0;
            int variableIndex;
            foreach (StepData step in stepList)
            {
                foreach (string variable in variablesNames)
                {
                    variableIndex = m_variablesInLog.IndexOf(variable);
                    TrackVariableData variableData = trackData.getVariableData(variable);
                    if (variableData != null)
                        variableData.lastEpisodeData.values[i] = step.data[variableIndex];
                }
                ++i;
            }
        }

        private void calculateEachVariableStats(TrackData trackData, List<string> variablesNames)
        {
            foreach (string variable in variablesNames)
            {
                TrackVariableData variableData = trackData.getVariableData(variable);
                if (variableData != null)
                {
                    variableData.calculateStats();
                }
                else
                {
                    throw new Exception("Variable " + variable + " not loaded correctly");
                }
            }
        }

        private void generateExperimentLongAvgValues(TrackData trackData, List<EpisodeData> episodes, List<string> variablesNames)
        {
            double avg;
            int variableIndex;
            foreach (EpisodeData episode in episodes)
            {
                if (episode.steps.Count > 0)
                {
                    foreach (string variable in variablesNames)
                    {
                        variableIndex = m_variablesInLog.IndexOf(variable);
                        TrackVariableData trackVariableData = trackData.getVariableData(variable);
                        if (trackVariableData != null)
                        {
                            avg = getAverageValueFromEpisodeStepsData(episode.steps, variableIndex);
                            trackVariableData.experimentData.values[episode.index - 1] = avg;
                        }
                    }
                }
            }
        }

        private double getAverageValueFromEpisodeStepsData(List<StepData> stepList, int variableIndex)
        {
            if (stepList != null)
            {
                double sum = 0;
                foreach (StepData step in stepList)
                {
                    sum += step.data[variableIndex];
                }
                double average = sum / stepList.Count;
                return average;
            }
            else
            {
                throw new ArgumentNullException();
            }
        }

        private void generateAllEvaluationEpisodesValues(TrackData trackData, List<EpisodeData> episodes, List<string> variablesNames)
        {
            int episodeIndex = 0;
            foreach (EpisodeData episode in episodes)
            {
                generateEpisodeData(trackData, episode.steps, variablesNames, episodeIndex);
                episodeIndex++;
            }
        }

        private void generateEpisodeData(TrackData trackData, List<StepData> stepList, List<string> variablesNames, int episodeIndex)
        {
            int i = 0;
            int variableIndex;
            foreach (StepData step in stepList)
            {
                foreach (string variable in variablesNames)
                {
                    variableIndex = m_variablesInLog.IndexOf(variable);
                    TrackVariableData variableData = trackData.getVariableData(variable);
                    if (variableData != null)
                        variableData.evaluationEpisodesData[episodeIndex].values[i] = step.data[variableIndex];
                }
                ++i;
            }
        }
    }
}
