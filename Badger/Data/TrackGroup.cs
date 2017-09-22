using Caliburn.Micro;
using System.Collections.Generic;
using Badger.Simion;
using Badger.ViewModels;

namespace Badger.Data
{
    /// <summary>
    /// A set of tracks grouped by a set of forks. For example if "groupBy" is set to fork "controller"
    /// , then a different group will be created for each value of the fork "controller".
    /// Groups need to be consolidated according to some criteria (i.e. maximum average value of "r") before
    /// accessing the data.
    /// </summary>
    public class TrackGroup
    {
        //data read fromm the log files: might be more than one track before applying a group function
        private List<Track> m_tracks = new List<Track>();
        //public merged track data: cannot be accessed before calling consolidateGroups()
        public Track ConsolidatedTrack
        {
            get { if (m_tracks.Count == 1) return m_tracks[0]; return null; }
            set { }
        }
        public bool HasData
        {
            get { return m_tracks.Count > 0; }
        }

        //fork values given to this group
        public Dictionary<string, string> ForkValues { get; set; } = new Dictionary<string, string>();

        public string ExperimentId { get; set; } = "";

        private static char[] valueDelimiters = new char[] { '=', '/', '\\' };
        
        public TrackGroup(string experimentId)
        {
            ExperimentId = experimentId;
        }
        public string TrackId
        {
            get{ return SetNameFromForkValues(); }
        }
        public string GroupId { get; set; }


        public void AddTrackData(Track newTrackData)
        {
            m_tracks.Add(newTrackData);
        }

        //this function selects a unique track fromm each group (if there's more than one track)
        public void ConsolidateGroups(string inGroupSelectionFunction, string inGroupSelectionVariable, List<string> groupBy)
        {
            if (m_tracks.Count > 1)
            {
                Track selectedTrack = null;
                double min = double.MaxValue, max = double.MinValue;
                foreach (Track track in m_tracks)
                {
                    SeriesGroup variableData = track.GetDataSeriesForOrdering(inGroupSelectionVariable);
                    if (variableData != null)
                    {
                        double sortValue = variableData.MainSeries.Stats.avg;
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
                m_tracks.Clear();
                if (selectedTrack != null)
                {
                    m_tracks.Add(selectedTrack);

                    //create a copy of the dictionary to solve the following issue:
                    //after generating the first report (with groups) the forkValues of the experiments are cleared
                    //and therefore, no more report can be generated afterwards
                    ForkValues = new Dictionary<string, string>(selectedTrack.ForkValues);
                }
            }

            SetNameFromGroups(groupBy);
        }
        void SetNameFromGroups(List<string> groupBy)
        {
            if (groupBy.Count > 0)
            {
                //we remove those forks used to group from the forkValues
                //because *hopefully* we only use them to name the track
                GroupId = "";
                string shortId;
                foreach (string group in groupBy)
                {
                    if (ForkValues.ContainsKey(group))
                    {
                        shortId = Utility.LimitLength(group, 10);
                        if (shortId.Length > 0)
                            GroupId += shortId + "=";
                        GroupId += Utility.LimitLength(ForkValues[group], 10, valueDelimiters) + ",";
                        ForkValues.Remove(group);
                    }
                }
                GroupId = GroupId.TrimEnd(',');
            }
            else GroupId= SetNameFromForkValues();
        }
        string SetNameFromForkValues()
        {
            if (ForkValues.Count == 0) return "N/A";
            string id = "", shortId;
            foreach (KeyValuePair<string, string> entry in ForkValues)
            {
                //we limit the length of the values
                shortId = Utility.LimitLength(entry.Key, 10);
                if (shortId.Length > 0)
                    id += shortId + "=";
                id += Utility.LimitLength(entry.Value, 20, valueDelimiters) + ",";
            }
            id = id.Trim(',');
            return id;
        }
    }
}
