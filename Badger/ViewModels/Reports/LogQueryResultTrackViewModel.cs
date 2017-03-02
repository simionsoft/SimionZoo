using Caliburn.Micro;
using System.Collections.Generic;
using System;

namespace Badger.ViewModels
{
    public class TrackVariableData
    {
        public TrackVariableData(int numSteps,int numEpisodes)
        {
            lastEpisodeValues = new double[numSteps];
            avgEpisodeValues = new double[numEpisodes];
        }
        public double[] lastEpisodeValues= null;
        public double[] avgEpisodeValues = null;
        public double min= 0.0, max= 0.0, avg= 0.0, stdDev= 0.0;
        public void calculateStats()
        {
            if (lastEpisodeValues == null || lastEpisodeValues.Length==0) return;
            //calculate avg, min and max
            double sum = 0.0;
            min = lastEpisodeValues[0]; max = lastEpisodeValues[0];
            foreach (double val in lastEpisodeValues)
            {
                sum += val;
                if (val > max) max = val;
                if (val < min) min = val;
            }
            avg = sum / lastEpisodeValues.Length;
            //calculate std. deviation
            double diff;
            sum = 0.0;
            foreach(double val in lastEpisodeValues)
            {
                diff = val - avg;
                sum += diff*diff;
            }
            stdDev = Math.Sqrt(sum / lastEpisodeValues.Length);
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
                        if (function == LogQuery.functionMax && variableData.avg > max)
                        {
                            max = variableData.avg;
                            selectedTrack = i;
                        }
                        if (function == LogQuery.functionMin && variableData.avg < min)
                        {
                            min = variableData.avg;
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
