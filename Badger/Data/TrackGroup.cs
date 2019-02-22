/*
	SimionZoo: A framework for online model-free Reinforcement Learning on continuous
	control problems

	Copyright (c) 2016 SimionSoft. https://github.com/simionsoft

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in all
	copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
	SOFTWARE.
*/

using Caliburn.Micro;
using System.Collections.Generic;
using System.Windows.Data;
using System.Globalization;

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
        //If GroupBy is used, then we will only include those used to group because they are common to all
        //the tracks in the group
        //Else, all the forks from the tracks
        public Dictionary<string, string> ForkValues { get; set; } = new Dictionary<string, string>();

        public string ExperimentId { get; set; } = "";

        private static char[] valueDelimiters = new char[] { '=', '/', '\\' };
        
        public TrackGroup(string experimentId)
        {
            ExperimentId = experimentId;
        }

        public void AddTrackData(Track newTrackData)
        {
            m_tracks.Add(newTrackData);
        }

        /// <summary>
        /// When grouping tracks by a fork, this function must be called to select a track inside the group.
        /// We call this "consolidating" the track group.
        /// </summary>
        /// <param name="inGroupSelectionFunction">The function used to compare tracks inside the group</param>
        /// <param name="inGroupSelectionVariable">The variable used to evaluate tracks</param>
        /// <param name="groupBy">The list of forks used to group tracks</param>
        public void Consolidate(string inGroupSelectionFunction, string inGroupSelectionVariable, string inGroupSelectionReportType
            , BindableCollection<string> groupBy)
        {
            //Consolidation makes only sense if were are using groups
            if (groupBy.Count == 0) return;

            EnumDescriptionConverter conv = new EnumDescriptionConverter();
            ReportType orderByReportType = (ReportType)((IValueConverter)conv).ConvertBack(inGroupSelectionReportType, typeof(ReportType), null, CultureInfo.CurrentCulture);

            if (m_tracks.Count > 1)
            {
                Track selectedTrack = m_tracks[0];
                double min = double.MaxValue, max = double.MinValue;
                double maxAscBeauty = double.MinValue, maxDscBeauty = double.MaxValue;
                foreach (Track track in m_tracks)
                {
                    SeriesGroup variableData = track.GetDataSeries(inGroupSelectionVariable, orderByReportType);
                    if (variableData != null)
                    {
                        double sortValue = variableData.MainSeries.Stats.avg;
                        if (inGroupSelectionFunction == LogQueryViewModel.FunctionMax && sortValue > max)
                        {
                            max = sortValue;
                            selectedTrack = track;
                        }
                        else if (inGroupSelectionFunction == LogQueryViewModel.FunctionMin && sortValue < min)
                        {
                            min = sortValue;
                            selectedTrack = track;
                        }
                        else if (inGroupSelectionFunction == LogQueryViewModel.FunctionAscBeauty 
                            && variableData.MainSeries.Stats.ascBeauty > maxAscBeauty)
                        {
                            maxAscBeauty= variableData.MainSeries.Stats.ascBeauty;
                            selectedTrack = track;
                        }
                        else if (inGroupSelectionFunction == LogQueryViewModel.FunctionDscBeauty
                             && variableData.MainSeries.Stats.dscBeauty > maxDscBeauty)
                        {
                            maxDscBeauty = variableData.MainSeries.Stats.dscBeauty;
                            selectedTrack = track;
                        }
                    }
                }
                m_tracks.Clear();
                if (selectedTrack != null)
                    m_tracks.Add(selectedTrack);
            }
        }


    }
}
