using Caliburn.Micro;
using System.Collections.Generic;
using Badger.Simion;
using Badger.Data;

namespace Badger.ViewModels
{
 
    public class LogQueryResultTrackViewModel : PropertyChangedBase
    {
        //data read fromm the log files: might be more than one track before applying a group function
        private List<TrackData> m_trackData = new List<TrackData>();
        //public merged track data: cannot be accessed before calling consolidateGroups()
        public TrackData ResultTrackData
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
        public string ExperimentId { get; set; } = "";

        private static char[] valueDelimiters = new char[] { '=', '/', '\\' };
        
        public LogQueryResultTrackViewModel(string experimentId)
        {
            ExperimentId = experimentId;
        }
        public string TrackId
        {
            get
            {
                if (m_forkValues.Count == 0) return "N/A";// m_experimentId;
                string id = "", shortId;
                foreach (KeyValuePair<string, string> entry in m_forkValues)
                {
                    //we limit the length of the values
                    shortId= Utility.LimitLength(entry.Key, 10);
                    if (shortId.Length > 0)
                        id += shortId + "=";
                    id += Utility.LimitLength(entry.Value,20,valueDelimiters) + ",";
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
        public void ConsolidateGroups(string inGroupSelectionFunction, string inGroupSelectionVariable, List<string> groupBy)
        {
            if (m_trackData.Count > 1)
            {
                TrackData selectedTrack = null;
                double min = double.MaxValue, max = double.MinValue;
                foreach (TrackData track in m_trackData)
                {
                    DataSeries variableData = track.GetDataSeriesForOrdering(inGroupSelectionVariable);
                    if (variableData != null)
                    {
                        double sortValue = variableData.Series.Stats.avg;
                        if (inGroupSelectionFunction == LogQuery.functionMax && sortValue > max)
                        {
                            max = sortValue;
                            selectedTrack = track;
                        }
                        if (inGroupSelectionFunction == LogQuery.functionMin && sortValue < min)
                        {
                            min = sortValue;
                            selectedTrack = track;
                        }
                    }
                }
                m_trackData.Clear();
                m_trackData.Add(selectedTrack);

                //create a copy of the dictionary to solve the following issue:
                //after generating the first report (with groups) the forkValues of the experiments are cleared
                //and therefore, no more report can be generated afterwards
                forkValues = new Dictionary<string, string>(selectedTrack.ForkValues);

                if (groupBy.Count > 0)
                {
                    //we remove those forks used to group from the forkValues
                    //because *hopefully* we only use them to name the track
                    m_groupId = "";
                    string shortId;
                    foreach (string group in groupBy)
                    {
                        shortId = Utility.LimitLength(group, 10);
                        if (shortId.Length > 0)
                            m_groupId += shortId + "=";
                        m_groupId += Utility.LimitLength(forkValues[group],10,valueDelimiters) + ",";
                        forkValues.Remove(group);
                    }
                    GroupId = m_groupId.TrimEnd(',');
                }
            }
        }
    }
}
