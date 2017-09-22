using System.Xml;
using System.Collections.Generic;
using Badger.Simion;
using Badger.Data;
using System.IO;
using System.Linq;
using System;
using Badger.Utils;

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
        public string ForkValuesAsString => Utility.DictionaryAsString(forkValues);

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

            logDescriptorFilePath = SimionFileData.GetLogFilePath(experimentFilePath, true);
            if (!File.Exists(logDescriptorFilePath))
            {
                //for back-compatibility: if the approapriate log file is not found, check whether one exists
                //with the legacy naming convention: experiment-log.xml
                logDescriptorFilePath = SimionFileData.GetLogFilePath(experimentFilePath, true, true);
                logFilePath = SimionFileData.GetLogFilePath(experimentFilePath, false, true);
            }
            else
                logFilePath = SimionFileData.GetLogFilePath(experimentFilePath, false);

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

            if (!experimentData.bSuccesful || experimentData.episodes.Count == 0) return null;

            int longestEpisodeIndex = experimentData.episodes.Select(e => e.steps.Count).MaxIndex();
            int maxNumSteps = experimentData.episodes[longestEpisodeIndex].steps.Count;

            EpisodeData lastEvaluationEpisode = experimentData.episodes.Where(e => e.type == 0).Last();

            TrackData data = new TrackData(maxNumSteps, experimentData.numEpisodes, experimentData.numEvaluationEpisodes, experimentData.numTrainingEpisodes, varNames);
            data.bSuccesful = experimentData.bSuccesful;
            data.forkValues = forkValues;

            //set the realTime and simTime
            foreach (var item in experimentData.episodes[longestEpisodeIndex].steps.Select((step, index) => new { index, step }))
            {
                data.realTime[item.index] = item.step.episodeRealTime;
                data.simTime[item.index] = item.step.episodeSimTime;
            }

            //set the experiment data now
            int evaluationIndex = 0;
            int trainingIndex = 0;
            foreach (EpisodeData episode in experimentData.episodes)
            {
                if (episode.type == EpisodeData.episodeTypeEvaluation)
                {
                    //experiment-long average values
                    foreach (string variable in varNames)
                    {
                        double avg = 0.0;

                        int variableIndex = m_variablesInLog.IndexOf(variable);
                        if (variableIndex >= 0)
                        {
                            TrackVariableData variableData = null;
                            variableData = data.getVariableData(variable);

                            if (variableData != null && episode.steps.Count > 0)
                            {
                                foreach (StepData step in episode.steps)
                                {
                                    avg += step.data[variableIndex];
                                }
                                avg /= episode.steps.Count;
                                variableData.experimentAverageData.Values[episode.index - 1] = avg;
                            }
                        }
                    }
                }

                //gather data for all training/evaluation episodes
                foreach (string variable in varNames)
                {
                    int variableIndex = m_variablesInLog.IndexOf(variable);
                    if (variableIndex >= 0)
                    {
                        TrackVariableData variableData = data.getVariableData(variable);

                        if (episode.type == 0)
                            variableData.experimentEvaluationData[evaluationIndex].SetLength(episode.steps.Count);
                        else
                            variableData.experimentTrainingData[trainingIndex].SetLength(episode.steps.Count);

                        foreach (var item in episode.steps.Select((step, index) => new { index, step }))
                        {
                            if (variableData != null)
                            {
                                if (episode.type == 0)
                                    variableData.experimentEvaluationData[evaluationIndex].Values[item.index] = item.step.data[variableIndex];
                                else
                                    variableData.experimentTrainingData[trainingIndex].Values[item.index] = item.step.data[variableIndex];
                            }
                        }
                    }
                }

                if (episode.type == 0)
                    evaluationIndex++;
                else
                    trainingIndex++;
            }


            //last evaluation Episode
            foreach (string variable in varNames)
            {
                int variableIndex = m_variablesInLog.IndexOf(variable);
                if (variableIndex >= 0)
                {
                    TrackVariableData variableData = data.getVariableData(variable);

                    variableData.lastEvaluationEpisodeData.SetLength(lastEvaluationEpisode.steps.Count);

                    foreach (var item in lastEvaluationEpisode.steps.Select((step, index) => new { index, step }))
                    {
                        if (variableData != null)
                            variableData.lastEvaluationEpisodeData.Values[item.index] = item.step.data[variableIndex];
                    }
                }
            }

            //calculate each variable's last episode stats
            foreach (string variable in varNames)
            {
                TrackVariableData variableData = data.getVariableData(variable);
                if (variableData != null)
                    variableData.CalculateStats();
            }

            return data;
        }
    }
}
