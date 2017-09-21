using System.Xml;
using System.Collections.Generic;
using Badger.Simion;
using Badger.Data;
using System;

namespace Badger.ViewModels
{
    public class LoggedExperimentalUnitViewModel : SelectableTreeItem
    {
        public SimionLog Log { get; }

        private string m_name;
        public string Name
        {
            get { return m_name; }
            set { m_name = value; NotifyOfPropertyChange(() => Name); }
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

        public List<string> VariablesInLog => Log.VariablesLogged;

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
            Log= new SimionLog( baseDirectory + configNode.Attributes[XMLConfig.pathAttribute].Value);


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
       

        //loads the log file and then returns the data of the requested variable as a TrackData object
        //Not the most efficient way (we may load way more than we need), but good enough for now
        public Track LoadTrackData(List<Report> reports)
        {
            Log.LoadBinaryLog();

            if (!Log.BinFileLoadSuccess || Log.TotalNumEpisodes == 0) return null;

            Track track = new Track(forkValues);
            SeriesGroup dataSeries;
            foreach (Report report in reports)
            {
                switch(report.Type)
                {
                    case PlotType.LastEvaluation:
                        EpisodesData lastEpisode = Log.EvaluationEpisodes[Log.EvaluationEpisodes.Count - 1];
                        dataSeries = new SeriesGroup(report);
                        dataSeries.AddSeries(Log.GetEpisodeData(lastEpisode, report));
                        track.AddVariableData(report, dataSeries);
                        break;
                    case PlotType.EvaluationAverages:
                        track.AddVariableData(report, Log.GetAveragedData(Log.EvaluationEpisodes, report));
                        break;
                    case PlotType.AllEvaluationEpisodes:
                    case PlotType.AllTrainingEpisodes:
                        dataSeries = new SeriesGroup(report);
                        List<EpisodesData> episodes;
                        if (report.Type == PlotType.AllEvaluationEpisodes)
                            episodes = Log.EvaluationEpisodes;
                        else episodes = Log.TrainingEpisodes;
                        foreach(EpisodesData episode in episodes)
                        {
                            Series subSeries = Log.GetEpisodeData(episode, report);
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
