using Caliburn.Micro;
using System.Collections.Generic;
using System;
using Badger.Data;
using System.Linq;

namespace Badger.ViewModels
{
    public class DataSeries
    {
        private double[] _values = null;
        public double[] Values
        {
            get
            {
                if (_values is null)
                    throw new InvalidOperationException("Values has not been initiliazed; you must call SetLength() before accessing the Values property.");
                return _values;
            }
            set { _values = value; }
        }
        public StatData Stats = new StatData();

        public bool Initialized
        {
            get { return (_values != null); }
        }

        public void SetLength(int numValues)
        {
            Values = new double[numValues];
        }

        public void CalculateStats()
        {
            if (!Initialized) return;

            //calculate avg, min and max
            double sum = 0.0;
            Stats.min = Values[0]; Stats.max = Values[0];
            foreach (double val in Values)
            {
                sum += val;
                if (val > Stats.max) Stats.max = val;
                if (val < Stats.min) Stats.min = val;
            }

            Stats.avg = sum / Values.Length;

            //calculate std. deviation
            double diff;
            sum = 0.0;
            foreach (double val in Values)
            {
                diff = val - Stats.avg;
                sum += diff * diff;
            }

            Stats.stdDev = Math.Sqrt(sum / Values.Length);
        }

    }
    public class TrackVariableData
    {
        public TrackVariableData(int numSteps, int evaluationEpisodes, int trainingEpisodes)
        {
            lastEvaluationEpisodeData = new DataSeries();
            lastEvaluationEpisodeData.SetLength(numSteps);
            if (numSteps > 0)
            {
                experimentAverageData = new DataSeries();
                //averages are only interesting in evaluation episodes
                experimentAverageData.SetLength(evaluationEpisodes);
            }
            
            experimentEvaluationData = new List<DataSeries>(evaluationEpisodes);
            for (int i = 0; i < evaluationEpisodes; i++)
            {
                experimentEvaluationData.Add(new DataSeries());
            }
            
            experimentTrainingData = new List<DataSeries>(trainingEpisodes);
            for (int i = 0; i < trainingEpisodes; i++)
            {
                experimentTrainingData.Add(new DataSeries());
            }
        }

        public DataSeries lastEvaluationEpisodeData;
        public DataSeries experimentAverageData;
        public List<DataSeries> experimentEvaluationData;
        public List<DataSeries> experimentTrainingData;

        public void CalculateStats()
        {
            if (lastEvaluationEpisodeData != null) lastEvaluationEpisodeData.CalculateStats();
            if (experimentAverageData != null) experimentAverageData.CalculateStats();

            foreach (var item in experimentEvaluationData)
                item.CalculateStats();

            foreach (var item in experimentTrainingData)
                item.CalculateStats();
        }
    }
    public class TrackData
    {
        public bool bSuccesful;
        public double[] simTime;
        public double[] realTime;
        public Dictionary<string, string> forkValues;
        private Dictionary<string, TrackVariableData> variablesData = new Dictionary<string, TrackVariableData>();

        public TrackData(int maxNumSteps, int numEpisodes, int evaluationEpisodes, int trainingEpisodes, List<string> variables)
        {
            simTime = new double[maxNumSteps];
            realTime = new double[maxNumSteps];
            foreach (string variable in variables)
            {
                this.variablesData[variable] = new TrackVariableData(maxNumSteps, evaluationEpisodes, trainingEpisodes);
            }
        }

        private void addVariableData(string variable, TrackVariableData variableData)
        {
            this.variablesData.Add(variable, variableData);
        }

        public TrackVariableData getVariableData(string variable)
        {
            if (variablesData.ContainsKey(variable)) return variablesData[variable];
            else return null;
        }
    }
    public class LogQueryResultTrackViewModel : PropertyChangedBase
    {
        //data read fromm the log files: might be more than one track before applying a group function
        private List<TrackData> m_trackData = new List<TrackData>();
        //public merged track data: cannot be accessed before calling consolidateGroups()
        public TrackData trackData
        {
            get { if (m_trackData.Count == 1) return m_trackData[0]; return null; }
            set { }
        }
        public bool HasData
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
        public string TrackId
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
                    id += Utility.limitLength(entry.Value,20,valueDelimiters) + ",";
                }
                id = id.Trim(',');
                return id;
            }
        }
        private string m_groupId = null;
        public string GroupId
        {
            get { if (m_groupId != null) return m_groupId; return TrackId; }
            set { m_groupId = value; NotifyOfPropertyChange(() => GroupId); }
        }

        public void AddTrackData(TrackData newTrackData)
        {
            m_trackData.Add(newTrackData);
        }

        //this function selects a unique track fromm each group (if there's more than one track)
        public void ConsolidateGroups(string function, string variable, List<string> groupBy)
        {
            if (m_trackData.Count > 1)
            {
                TrackData selectedTrack = null;
                double min = double.MaxValue, max = double.MinValue;
                foreach (TrackData track in m_trackData)
                {
                    TrackVariableData variableData = track.getVariableData(variable);
                    if (variableData != null)
                    {
                        if (function == LogQuery.functionMax && Math.Abs(variableData.lastEvaluationEpisodeData.Stats.avg) > max)
                        {
                            max = Math.Abs(variableData.lastEvaluationEpisodeData.Stats.avg);
                            selectedTrack = track;
                        }
                        if (function == LogQuery.functionMin && Math.Abs(variableData.lastEvaluationEpisodeData.Stats.avg) < min)
                        {
                            min = Math.Abs(variableData.lastEvaluationEpisodeData.Stats.avg);
                            selectedTrack = track;
                        }
                    }
                }
                m_trackData.Clear();
                m_trackData.Add(selectedTrack);

                //create a copy of the dictionary to solve the following issue:
                //after generating the first report (with groups) the forkValues of the experiments are cleared
                //and therefore, no more report can be generated afterwards
                forkValues = new Dictionary<string, string>(selectedTrack.forkValues);

                if (groupBy.Count > 0)
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
                    GroupId = m_groupId.TrimEnd(',');
                }
            }
        }
    }
}
