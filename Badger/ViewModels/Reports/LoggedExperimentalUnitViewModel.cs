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
        public TrackData LoadTrackData(List<ReportParams> trackParameters)
        {
            Log.LoadBinaryLog();

            if (!Log.BinFileLoadSuccess || Log.TotalNumEpisodes == 0) return null;

            TrackData trackData = new TrackData(forkValues);
            DataSeries dataSeries;
            foreach (ReportParams track in trackParameters)
            {
                switch(track.Type)
                {
                    case PlotType.LastEvaluation:
                        EpisodesData lastEpisode = Log.EvaluationEpisodes[Log.EvaluationEpisodes.Count - 1];
                        dataSeries = new DataSeries(track);
                        dataSeries.AddSeries(Log.GetEpisodeData(lastEpisode, track));
                        trackData.AddVariableData(track, dataSeries);
                        break;
                    case PlotType.EvaluationAverages:
                        trackData.AddVariableData(track, Log.GetAveragedData(Log.EvaluationEpisodes, track));
                        break;
                    case PlotType.AllEvaluationEpisodes:
                    case PlotType.AllTrainingEpisodes:
                        dataSeries = new DataSeries(track);
                        List<EpisodesData> episodes;
                        if (track.Type == PlotType.AllEvaluationEpisodes)
                            episodes = Log.EvaluationEpisodes;
                        else episodes = Log.TrainingEpisodes;
                        foreach(EpisodesData episode in episodes)
                        {
                            XYSeries subSeries = Log.GetEpisodeData(episode, track);
                            subSeries.Id = episode.index.ToString();
                            dataSeries.AddSeries(subSeries);
                        }
                        trackData.AddVariableData(track, dataSeries);
                        break;
                }
            }
            return trackData;
        }
    }
}
