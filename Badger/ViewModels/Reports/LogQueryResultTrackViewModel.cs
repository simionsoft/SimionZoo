using Caliburn.Micro;
using System.Collections.Generic;
using System;
using Badger.Data;

namespace Badger.ViewModels
{
    public class DataSeries
    {
        public double[] values = null;
        public StatData stats= new StatData();

        public DataSeries(int numValues)
        {
            values = new double[numValues];
        }
        public void calculateStats()
        {
            if (values == null) return;
            //calculate avg, min and max
            double sum = 0.0;
            stats.min = values[0]; stats.max = values[0];
            foreach (double val in values)
            {
                sum += val;
                if (val > stats.max) stats.max = val;
                if (val < stats.min) stats.min = val;
            }
            stats.avg = sum / values.Length;
            //calculate std. deviation
            double diff;
            sum = 0.0;
            foreach (double val in values)
            {
                diff = val - stats.avg;
                sum += diff * diff;
            }
            stats.stdDev = Math.Sqrt(sum / values.Length);
        }
    }
    public class TrackVariableData
    {
        public TrackVariableData(int numSteps,int numEpisodes)
        {
            if (numSteps>0) lastEpisodeData = new DataSeries(numSteps);
            if (numEpisodes>0) experimentData = new DataSeries(numEpisodes);
        }
        public DataSeries lastEpisodeData;
        public DataSeries experimentData;

        public void calculateStats()
        {
            if (lastEpisodeData != null) lastEpisodeData.calculateStats();
            if (experimentData != null) experimentData.calculateStats();
        }
    }
    public class TrackData
    {
        public bool bSuccesful;
        public double []simTime;
        public double []realTime;
        public Dictionary<string, string> forkValues;
        private Dictionary<string,TrackVariableData>variablesData= new Dictionary<string,TrackVariableData>();

        public TrackData(int numSteps,int numEpisodes, List<string> variables)
        {
            simTime = new double[numSteps];
            realTime = new double[numSteps];
            foreach (string variable in variables)
            {
                this.variablesData[variable] = new TrackVariableData(numSteps,numEpisodes);
            }
        }
        private void addVariableData(string variable,TrackVariableData variableData)
        {
            this.variablesData.Add(variable, variableData);
        }
        public TrackVariableData getVariableData(string variable)
        {
            if (variablesData.ContainsKey(variable)) return variablesData[variable];
            else return null;
        }
    }
    public class LogQueryResultTrackViewModel: PropertyChangedBase
    {
        //data read fromm the log files: might be more than one track before applying a group function
        private List<TrackData> m_trackData = new List<TrackData>();
        //public merged track data: cannot be accessed before calling consolidateGroups()
        public TrackData trackData
        {
            get { if (m_trackData.Count == 1) return m_trackData[0]; return null; }
            set { }
        }
        public bool bHasData
        {
            get { return m_trackData.Count > 0; }
        }

        //fork values given to this group
        private Dictionary<string, string> m_forkValues = new Dictionary<string, string>();
        public Dictionary<string, string> forkValues
        {
            get { return m_forkValues; }
            set { m_forkValues = value; NotifyOfPropertyChange(() => forkValues); }
        }
        private string m_parentExperimentName = "";

        private static char[] valueDelimiters = new char[] { '=', '/', '\\' };
        
        public LogQueryResultTrackViewModel(string experimentName)
        {
            m_parentExperimentName = experimentName;
        }
        public string trackId
        {
            get
            {
                if (m_forkValues.Count == 0) return m_parentExperimentName;
                string id = "", shortId;
                foreach (KeyValuePair<string, string> entry in m_forkValues)
                {
                    //we limit the length of the values
                    shortId= Utility.limitLength(entry.Key, 10);
                    if (shortId.Length > 0)
                        id += shortId + "=";
                    id += Utility.limitLength(entry.Value,10,valueDelimiters) + ",";
                }
                id= id.Trim(',');
                return id;
            }
        }
        private string m_groupId = null;
        public string groupId
        {
            get { if (m_groupId!=null) return m_groupId; return trackId; }
            set { m_groupId = value;  NotifyOfPropertyChange(()=>groupId); }
        }

        public void addTrackData(TrackData newTrackData)
        {
            m_trackData.Add(newTrackData);
        }

        //this function selects a unique track fromm each group (if there's more than one track)
        public void consolidateGroups(string function,string variable,List<string> groupBy)
        {
            if (m_trackData.Count>1)
            {
                TrackData selectedTrack= null;
                double min= double.MaxValue, max= double.MinValue;
                foreach (TrackData track in m_trackData)
                {
                    TrackVariableData variableData = track.getVariableData(variable);
                    if (variableData != null)
                    {
                        if (function == LogQuery.functionMax && Math.Abs(variableData.lastEpisodeData.stats.avg) > max)
                        {
                            max = Math.Abs(variableData.lastEpisodeData.stats.avg);
                            selectedTrack = track;
                        }
                        if (function == LogQuery.functionMin && Math.Abs(variableData.lastEpisodeData.stats.avg) < min)
                        {
                            min = Math.Abs(variableData.lastEpisodeData.stats.avg);
                            selectedTrack = track;
                        }
                    }
                }
                m_trackData.Clear();
                m_trackData.Add(selectedTrack);
                forkValues = selectedTrack.forkValues;

                if (groupBy.Count>0)
                {
                    //we remove those forks used to group from the forkValues
                    //because *hopefully* we only use them to name the track
                    m_groupId = "";
                    string shortId;
                    foreach (string group in groupBy)
                    {
                        shortId = Utility.limitLength(group, 10);
                        if (shortId.Length > 0)
                            m_groupId += shortId + "=";
                        m_groupId += Utility.limitLength(forkValues[group],10,valueDelimiters) + ",";
                        forkValues.Remove(group);
                    }
                    groupId = m_groupId.TrimEnd(',');
                }
            }
        }
    }
}
