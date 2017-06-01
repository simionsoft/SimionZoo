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
            stats.min = getMinValueOfArray(values);
            stats.max = getMaxValueOfArray(values);
            stats.avg = getAverageOfArrayValues(values);
            stats.stdDev = getStdDeviationOfArrayValues(values, stats.avg);
        }

        private double getMinValueOfArray(double []values)
        {
            if (values != null)
            {
                double minimumValue = values[0];
                foreach(double valueToCheck in values)
                {
                    if (valueToCheck < minimumValue) minimumValue = valueToCheck;
                }
                return minimumValue;
            }
            else
            {
                throw new ArgumentNullException();
            }
        }

        private double getMaxValueOfArray(double[] values)
        {
            if (values != null)
            {
                double maximumValue = values[0];
                foreach (double valueToCheck in values)
                {
                    if (valueToCheck > maximumValue) maximumValue = valueToCheck;
                }
                return maximumValue;
            }
            else
            {
                throw new ArgumentNullException();
            }
        }

        private double getAverageOfArrayValues(double[] values)
        {
            if (values != null)
            {
                double sum = 0;
                foreach (double valueToSum in values)
                {
                    sum += valueToSum;
                }
                return sum / values.Length;
            }
            else
            {
                throw new ArgumentNullException();
            }
        }

        private double getStdDeviationOfArrayValues(double[] values, double averageValue)
        {
            if (values != null)
            {
                double valueDifferenceWithAvg = 0;
                double totalSum = 0;
                foreach (double valueToCheck in values)
                {
                    valueDifferenceWithAvg = valueToCheck - averageValue;
                    totalSum += Math.Pow(valueDifferenceWithAvg, 2);
                }
                return Math.Sqrt(totalSum / values.Length);
            }
            else
            {
                throw new ArgumentNullException();
            }
        }
    }
    public class TrackVariableData
    {
        public TrackVariableData(int numSteps, int numEpisodes)
        {
            if (numSteps > 0) lastEpisodeData = new DataSeries(numSteps);
            if (numEpisodes > 0)
            {
                experimentData = new DataSeries(numEpisodes);
                evaluationEpisodesData = new DataSeries[numEpisodes];
                for (int i = 0; i < numEpisodes; i++)
                {
                    evaluationEpisodesData[i] = new DataSeries(numSteps);
                }
            }

        }
        public DataSeries lastEpisodeData;
        public DataSeries experimentData;
        public DataSeries[] evaluationEpisodesData;

        public void calculateStats()
        {
            if (lastEpisodeData != null) lastEpisodeData.calculateStats();
            if (experimentData != null) experimentData.calculateStats();
            foreach (DataSeries evaluationEpisode in evaluationEpisodesData)
            {
                if (evaluationEpisode != null)
                    evaluationEpisode.calculateStats();
            }
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
                    foreach (string group in groupBy)
                    {
                        m_groupId += group + "=" + forkValues[group] + ",";
                        forkValues.Remove(group);
                    }
                    groupId = m_groupId.TrimEnd(',');
                }
            }
        }
    }
}
