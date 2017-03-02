using Caliburn.Micro;
using System.Collections.Generic;
using System;

namespace Badger.ViewModels
{
    public class TrackVariableData
    {
        public TrackVariableData(int numSteps)
        {
            data = new double[numSteps];
        }
        public double[] data= null;
        public double min= 0.0, max= 0.0, avg= 0.0, stdDev= 0.0;
        public void calculateStats()
        {
            if (data == null || data.Length==0) return;
            //calculate avg, min and max
            double sum = 0.0;
            min = data[0]; max = data[0];
            foreach (double val in data)
            {
                sum += val;
                if (val > max) max = val;
                if (val < min) min = val;
            }
            avg = sum / data.Length;
            //calculate std. deviation
            double diff;
            sum = 0.0;
            foreach(double val in data)
            {
                diff = val - avg;
                sum += diff*diff;
            }
            stdDev = Math.Sqrt(sum / data.Length);
        }
    }
    public class TrackData
    {
        public double []simTime;
        public double []realTime;
        public Dictionary<string,TrackVariableData>variables= new Dictionary<string,TrackVariableData>();

        public TrackData(int numSteps, List<string> variables)
        {
            simTime = new double[numSteps];
            realTime = new double[numSteps];
            foreach (string variable in variables)
            {
                this.variables[variable] = new TrackVariableData(numSteps);
            }
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

        public void applyHavingSelection(string function,string variable)
        {
            if (m_trackData.Count>1)
            {
                int selectedTrack = -1;
                double min= double.MaxValue, max= double.MinValue;
                for (int i= 0; i< m_trackData.Count; i++)
                {
                    if (function==LogQuery.functionMax && m_trackData[i].variables[variable].avg> max)
                    {
                        max = m_trackData[i].variables[variable].avg;
                        selectedTrack = i;
                    }
                    if (function == LogQuery.functionMin && m_trackData[i].variables[variable].avg < min)
                    {
                        min = m_trackData[i].variables[variable].avg;
                        selectedTrack = i;
                    }
                }
                TrackData selectedTrackData = m_trackData[selectedTrack];
                m_trackData.Clear();
                m_trackData.Add(selectedTrackData);
            }
        }
    }
}
