using System.Xml;
using System.Collections.Generic;
using System;

using Caliburn.Micro;

using Badger.Data;

using Herd.Files;

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
            get { return Herd.Utils.DictionaryAsString(forkValues); }
        }

        public bool ContainsForks(BindableCollection<string> forks)
        {
            foreach (string fork in forks)
            {
                if (fork!=ReportsWindowViewModel.GroupByExperimentId && !forkValues.ContainsKey(fork))
                    return false;
            }
            return true;
        }

        public List<string> VariablesInLog { get; set; }


        /// <summary>
        /// Fake constructor for testing purposes
        /// </summary>
        /// <param name="filename"></param>
        public LoggedExperimentalUnitViewModel(string filename)
        {
            ExperimentFileName = filename;
            Name = Herd.Utils.GetFilename(filename);
        }

        /// <summary>
        /// Main constructor
        /// </summary>
        /// <param name="configNode"></param>
        /// <param name="baseDirectory"></param>
        /// <param name="updateFunction"></param>

        public LoggedExperimentalUnitViewModel(XmlNode configNode, string baseDirectory
            , Files.LoadUpdateFunction updateFunction = null)
        {
            //Experiment Name
            if (configNode.Attributes != null)
            {
                if (configNode.Attributes.GetNamedItem(XMLTags.nameAttribute) != null)
                    Name = configNode.Attributes[XMLTags.nameAttribute].Value;
            }

            //Initalize the paths to the log files
            if (configNode.Attributes.GetNamedItem(XMLTags.pathAttribute) == null)
                throw new Exception("Malformed experiment batch file: cannot get the path to an experimental unit");

            ExperimentFileName= baseDirectory + configNode.Attributes[XMLTags.pathAttribute].Value;
            LogDescriptorFileName = Herd.Utils.GetLogFilePath(ExperimentFileName, true);
            LogFileName = Herd.Utils.GetLogFilePath(ExperimentFileName, false);

            //FORKS
            //load the value of each fork used in this experimental unit
            foreach (XmlNode fork in configNode.ChildNodes)
            {
                string forkName = fork.Attributes[XMLTags.aliasAttribute].Value;
                foreach (XmlNode value in fork.ChildNodes)
                {
                    string forkValue = value.Attributes.GetNamedItem("Value").InnerText; // The value is in the attribute named "Value"
                    forkValues[forkName] = forkValue;
                }
            }
            //update progress
            updateFunction?.Invoke();
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
        /// <summary>
        /// Reads the log file and returns in a track the data for each of the reports.
        /// </summary>
        /// <param name="reports">Parameters of each of the reporters: variable, type, ...</param>
        /// <returns></returns>
        public Track LoadTrackData(List<Report> reports)
        {
            Log.SimionLog Log = new Log.SimionLog();
            Log.LoadBinaryLog(LogFileName);

            if (!Log.SuccessfulLoad || Log.TotalNumEpisodes == 0) return null;

            Track track = new Track(forkValues,LogFileName,LogDescriptorFileName,ExperimentFileName);
            SeriesGroup dataSeries;
            int variableIndex;
            foreach (Report report in reports)
            {
                variableIndex = GetVariableIndex(report.Variable);
                switch(report.Type)
                {
                    case ReportType.LastEvaluation:
                        Log.EpisodesData lastEpisode = Log.EvaluationEpisodes[Log.EvaluationEpisodes.Count - 1];
                        dataSeries = new SeriesGroup(report);
                        Series series = LogFileUtils.GetVariableData(lastEpisode, report, variableIndex);
                        if (series != null)
                        {
                            dataSeries.AddSeries(series);
                            track.AddVariableData(report, dataSeries);
                        }
                        break;
                    case ReportType.EvaluationAverages:
                        track.AddVariableData(report
                            , LogFileUtils.GetAveragedData(Log.EvaluationEpisodes, report,variableIndex));
                        break;
                    case ReportType.AllEvaluationEpisodes:
                    case ReportType.AllTrainingEpisodes:
                        dataSeries = new SeriesGroup(report);
                        List<Log.EpisodesData> episodes;
                        if (report.Type == ReportType.AllEvaluationEpisodes)
                            episodes = Log.EvaluationEpisodes;
                        else episodes = Log.TrainingEpisodes;
                        foreach(Log.EpisodesData episode in episodes)
                        {
                            Series subSeries = LogFileUtils.GetVariableData(episode, report, variableIndex);
                            if (subSeries != null)
                            {
                                subSeries.Id = episode.index.ToString();
                                dataSeries.AddSeries(subSeries);
                            }
                        }
                        track.AddVariableData(report, dataSeries);
                        break;
                }
            }
            return track;
        }
    }
}
