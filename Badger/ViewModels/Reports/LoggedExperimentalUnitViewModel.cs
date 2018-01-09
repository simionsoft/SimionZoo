using System.Xml;
using System.Collections.Generic;
using Badger.Simion;
using Badger.Data;
using System;
using System.IO;

namespace Badger.ViewModels
{
    public class LoggedExperimentalUnitViewModel : SelectableTreeItem
    {
        private string m_name;
        public string Name
        {
            get { return m_name; }
            set { m_name = value; NotifyOfPropertyChange(() => Name); }
        }

        public string ExperimentFileName;
        public string LogDescriptorFileName;
        public string LogFileName;

        private Dictionary<string, string> m_forkValues = new Dictionary<string, string>();
        public Dictionary<string, string> forkValues
        {
            get { return m_forkValues; }
            set { m_forkValues = value; NotifyOfPropertyChange(() => forkValues); }
        }
        public string ForkValuesAsString
        {
            get { return Utility.DictionaryAsString(forkValues); }
        }

        public List<string> VariablesInLog { get; set; }

        public LoggedExperimentalUnitViewModel(XmlNode configNode, string baseDirectory)
        {
            //Experiment Name
            if (configNode.Attributes != null)
            {
                if (configNode.Attributes.GetNamedItem(XMLConfig.nameAttribute) != null)
                    Name = configNode.Attributes[XMLConfig.nameAttribute].Value;
            }

            //Initalize the paths to the log files
            if (configNode.Attributes.GetNamedItem(XMLConfig.pathAttribute) == null)
                throw new Exception("Malformed experiment batch file: cannot get the path to an experimental unit");

            ExperimentFileName= baseDirectory + configNode.Attributes[XMLConfig.pathAttribute].Value;
            LogDescriptorFileName = SimionFileData.GetLogFilePath(ExperimentFileName, true);
            if (!File.Exists(LogDescriptorFileName))
            {
                //for back-compatibility: if the appropriate log file is not found, check whether one exists
                //with the legacy naming convention: experiment-log.xml
                LogDescriptorFileName = SimionFileData.GetLogFilePath(ExperimentFileName, true, true);
                LogFileName = SimionFileData.GetLogFilePath(ExperimentFileName, false, true);
            }
            else
                LogFileName = SimionFileData.GetLogFilePath(ExperimentFileName, false);

            //FORKS
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
       
        public void LoadLogDescriptor()
        {
            VariablesInLog = SimionLogDescriptor.LoadLogDescriptor(LogDescriptorFileName);
        }
        public int GetVariableIndex(string variableName)
        {
            int index = 0;
            foreach (string variable in VariablesInLog)
            {
                if (variable == variableName)
                    return index;
                index++;
            }
            return -1;
        }
        //loads the log file and then returns the data of the requested variable as a TrackData object
        //Not the most efficient way (we may load way more than we need), but good enough for now
        public Track LoadTrackData(List<Report> reports)
        {
            SimionLog Log = new SimionLog();
            Log.LoadBinaryLog(LogFileName);

            if (!Log.BinFileLoadSuccess || Log.TotalNumEpisodes == 0) return null;

            Track track = new Track(forkValues);
            SeriesGroup dataSeries;
            int variableIndex;
            foreach (Report report in reports)
            {
                variableIndex = GetVariableIndex(report.Variable);
                switch(report.Type)
                {
                    case ReportType.LastEvaluation:
                        EpisodesData lastEpisode = Log.EvaluationEpisodes[Log.EvaluationEpisodes.Count - 1];
                        dataSeries = new SeriesGroup(report);
                        dataSeries.AddSeries(Log.GetEpisodeData(lastEpisode, report,variableIndex));
                        track.AddVariableData(report, dataSeries);
                        break;
                    case ReportType.EvaluationAverages:
                        track.AddVariableData(report
                            , Log.GetAveragedData(Log.EvaluationEpisodes, report,variableIndex));
                        break;
                    case ReportType.AllEvaluationEpisodes:
                    case ReportType.AllTrainingEpisodes:
                        dataSeries = new SeriesGroup(report);
                        List<EpisodesData> episodes;
                        if (report.Type == ReportType.AllEvaluationEpisodes)
                            episodes = Log.EvaluationEpisodes;
                        else episodes = Log.TrainingEpisodes;
                        foreach(EpisodesData episode in episodes)
                        {
                            Series subSeries = Log.GetEpisodeData(episode, report, variableIndex);
                            subSeries.Id = episode.index.ToString();
                            dataSeries.AddSeries(subSeries);
                        }
                        track.AddVariableData(report, dataSeries);
                        break;
                }
            }
            return track;
        }
    }
}
