using Caliburn.Micro;
using System.Collections.Generic;
using System;

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
        public double []simTime;
        public double []realTime;
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
        //public merged track data: cannot be accessed before calling merge()
        public TrackData trackData
        {
            get { if (m_trackData.Count == 1) return m_trackData[0]; return null; }
            set { }
        }

        //fork values given to this group
        private Dictionary<string, string> m_forkValues = new Dictionary<string, string>();
        public Dictionary<string, string> forkValues
        {
            get { return m_forkValues; }
            set { m_forkValues = value; NotifyOfPropertyChange(() => forkValues); }
        }
        private string m_parentExperimentName = "";

        public LogQueryResultTrackViewModel(string experimentName)
        {
            m_parentExperimentName = experimentName;
        }
        public string trackId
        {
            get
            {
                if (m_forkValues.Count == 0) return m_parentExperimentName;
                string id = "";
                foreach (KeyValuePair<string, string> entry in m_forkValues)
                {
                    id += entry.Key + "=" + entry.Value + ",";
                }
                id.TrimEnd(',');
                return id;
            }
        }

        public void addTrackData(TrackData newTrackData)
        {
            m_trackData.Add(newTrackData);
        }

        //this function selects a unique track fromm each group (if there's more than one track)
        public void consolidateGroups(string function,string variable)
        {
            if (m_trackData.Count>1)
            {
                int selectedTrack = -1;
                double min= double.MaxValue, max= double.MinValue;
                for (int i= 0; i< m_trackData.Count; i++)
                {
                    TrackVariableData variableData = m_trackData[i].getVariableData(variable);
                    if (variableData != null)
                    {
                        if (function == LogQuery.functionMax && variableData.lastEpisodeData.stats.avg > max)
                        {
                            max = variableData.lastEpisodeData.stats.avg;
                            selectedTrack = i;
                        }
                        if (function == LogQuery.functionMin && variableData.lastEpisodeData.stats.avg < min)
                        {
                            min = variableData.lastEpisodeData.stats.avg;
                            selectedTrack = i;
                        }
                    }
                }
                TrackData selectedTrackData = m_trackData[selectedTrack];
                m_trackData.Clear();
                m_trackData.Add(selectedTrackData);
            }
        }
    }
}
