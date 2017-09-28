using System;
using System.Collections.Generic;
using Caliburn.Micro;
using Badger.Simion;
using Badger.Data;

namespace Badger.ViewModels
{
    public class AscAbsComparer : IComparer<double>
    {
        public int Compare(double x, double y)
        {
            if (Math.Abs(x) >= Math.Abs(y)) return 1;
            else return -1;
        }
    }
    public class DescAbsComparer : IComparer<double>
    {
        public int Compare(double x, double y)
        {
            if (Math.Abs(x) <= Math.Abs(y)) return 1;
            else return -1;
        }
    }
    public class AscComparer : IComparer<double>
    {
        public int Compare(double x, double y)
        {
            if (x >= y) return 1;
            else return -1;
        }
    }
    public class DescComparer : IComparer<double>
    {
        public int Compare(double x, double y)
        {
            if (x <= y) return 1;
            else return -1;
        }
    }
    public class LogQuery : PropertyChangedBase
    {
        public const string functionMax = "Max";
        public const string functionMin = "Min";

        public const string noLimitOnResults = "-";

        private string m_inGroupSelectionFunction = "";
        public string inGroupSelectionFunction
        {
            get { return m_inGroupSelectionFunction; }
            set { m_inGroupSelectionFunction = value; }
        }
        private string m_inGroupSelectionVariable = "";
        public string inGroupSelectionVariable
        {
            get { return m_inGroupSelectionVariable; }
            set { m_inGroupSelectionVariable = value; }
        }
        private string m_orderByFunction = "";
        public string orderByFunction
        {
            get { return m_orderByFunction; }
            set { m_orderByFunction = value; NotifyOfPropertyChange(() => orderByFunction); }
        }
        private string m_orderByVariable = "";
        public string orderByVariable
        {
            get { return m_orderByVariable; }
            set { m_orderByVariable = value; NotifyOfPropertyChange(() => orderByVariable); }
        }
        private string m_limitToOption = "";
        public string limitToOption
        {
            get { return m_limitToOption; }
            set { m_limitToOption = value; NotifyOfPropertyChange(() => limitToOption); }
        }

        private List<string> m_variables
            = new List<string>();
        public List<string> variables
        {
            get { return m_variables; }
            set { m_variables = value; }
        }

        public bool UseForkSelection { get; set; } = false;
        private List<string> m_groupBy = new List<string>();
        public List<string> groupBy { get { return m_groupBy; } }

        public LogQuery()
        {
        }

        public void addGroupBy(string name)
        {
            if (!m_groupBy.Contains(name)) m_groupBy.Add(name);
        }

        public string groupByAsString
        {
            get
            {
                string s = "";
                for (int i = 0; i < m_groupBy.Count - 1; i++) s += m_groupBy[i];
                if (m_groupBy.Count > 0) s += m_groupBy[m_groupBy.Count - 1];
                return s;
            }
        }

        private List<TrackGroup> m_resultTracks
            = new List<TrackGroup>();
        public List<TrackGroup> ResultTracks
        {
            get { return m_resultTracks; }
            set { m_resultTracks = value; NotifyOfPropertyChange(() => ResultTracks); }
        }

        public List<Report> GetTracksParameters()
        {
            List<Report> list = new List<Report>();
            foreach(TrackGroup group in ResultTracks)
            {
                //we iterate over the TrackParameters of the result track
                foreach (Report parameters in group.ConsolidatedTrack.SeriesGroups.Keys)
                    if (!list.Contains(parameters))
                        list.Add(parameters);
            }
            return list;
        }

        private TrackGroup GetTrack(Dictionary<string, string> forkValues)
        {
            uint numMatchedForks;
            foreach (TrackGroup resultTrack in ResultTracks)
            {
                numMatchedForks = 0;
                foreach (string forkName in forkValues.Keys)
                {
                    if (resultTrack.ForkValues.ContainsKey(forkName)
                        && forkValues[forkName] == resultTrack.ForkValues[forkName])
                        numMatchedForks++;
                }
                if (numMatchedForks == resultTrack.ForkValues.Count)
                    return resultTrack;

            }
            return null;
        }

        private BindableCollection<LoggedVariableViewModel> m_loggedVariables;
        public BindableCollection<LoggedVariableViewModel> loggedVariables
        {
            get { return m_loggedVariables; }
            set { m_loggedVariables = value; }
        }

        public List<Report> Reports = new List<Report>();

        public void Execute(BindableCollection<LoggedExperimentViewModel> experiments
            , BindableCollection<LoggedVariableViewModel> loggedVariablesVM)
        {
            TrackGroup resultTrackGroup = null;
            loggedVariables = loggedVariablesVM;

            //add all selected variables to the list of variables
            foreach (LoggedVariableViewModel variable in loggedVariablesVM)
                if ( variable.bIsSelected )
                    variables.Add(variable.name);

            Reports = DataTrackUtilities.FromLoggedVariables(loggedVariablesVM);

            //if the in-group selection function requires a variable not selected for the report
            //we add it too to the list of variables read from the log
            if (inGroupSelectionVariable != "" && !variables.Contains(inGroupSelectionVariable))
                Reports.Add(new Report(inGroupSelectionVariable, PlotType.LastEvaluation, ProcessFunc.None));

            //if we use some sorting function to select only some tracks, we need to add the variable
            // to the list too
            if (limitToOption != noLimitOnResults && !variables.Contains(orderByVariable))
                Reports.Add(new Report(orderByVariable, PlotType.LastEvaluation, ProcessFunc.None));

            //traverse the experimental units within each experiment
            foreach (LoggedExperimentViewModel exp in experiments)
            {
                foreach (LoggedExperimentalUnitViewModel expUnit in exp.ExperimentalUnits)
                {
                    //take selection into account? is this exp. unit selected?
                    if (!UseForkSelection || (UseForkSelection && expUnit.bIsSelected))
                    {
                        if (groupBy.Count != 0)
                        {
                            resultTrackGroup = GetTrack(expUnit.forkValues);
                            if (resultTrackGroup != null)
                            {
                                //the track exists and we are using forks to group results
                                Track trackData = expUnit.LoadTrackData(Reports);
                                if (trackData!=null)
                                    resultTrackGroup.AddTrackData(trackData);
                            }
                        }
                        if (resultTrackGroup == null) //New track
                        {
                            //No groups (each experimental unit is a track) or the track doesn't exist
                            //Either way, we create a new track
                            TrackGroup newResultTrackGroup = new TrackGroup(exp.Name);

                            if (groupBy.Count == 0)
                                newResultTrackGroup.ForkValues = expUnit.forkValues;
                            else
                                foreach (string group in groupBy)
                                {
                                    //an experimental unit may not have a fork used to group
                                    if (expUnit.forkValues.ContainsKey(group))
                                        newResultTrackGroup.ForkValues[group] = expUnit.forkValues[group];
                                }

                            //load data from the log file
                            Track trackData = expUnit.LoadTrackData(Reports);

                            if (trackData != null)
                            {
                                //for now, we just ignore failed experiments. Maybe we could do something more sophisticated
                                //for example, allow to choose only those parameter variations that lead to failed experiments
                                if (trackData.HasData)
                                    newResultTrackGroup.AddTrackData(trackData);

                                //we only consider those tracks with data loaded
                                if (newResultTrackGroup.HasData)
                                    ResultTracks.Add(newResultTrackGroup);
                            }
                        }
                    }
                }
            }

            //ConsolidateGroups selects a single track in each group using the in-group selection function
            //-max(avg(inGroupSelectionVariable)) or min(avg(inGroupSelectionVariable))
            //and also names groups depending on the number of tracks in the group
            foreach (TrackGroup trackGroup in ResultTracks)
                trackGroup.ConsolidateGroups(inGroupSelectionFunction, inGroupSelectionVariable, groupBy);

            //if we are using limitTo/orderBy, we have to select the best tracks/groups according to the given criteria
            if (limitToOption != LogQuery.noLimitOnResults)
            {
                int numMaxTracks = int.Parse(limitToOption);

                if (ResultTracks.Count > numMaxTracks)
                {
                    SortedList<double, TrackGroup> sortedList;

                    if (orderByFunction == functionMin)
                        sortedList = new SortedList<double, TrackGroup>(ResultTracks.Count
                            , new AscComparer());
                    else // (orderByFunction == orderDesc)
                        sortedList = new SortedList<double, TrackGroup>(ResultTracks.Count
                            , new DescComparer());

                    foreach (TrackGroup group in ResultTracks)
                    {
                        double sortValue = 0.0;
                        SeriesGroup variableData = group.ConsolidatedTrack.GetDataSeriesForOrdering(orderByVariable);
                        if (variableData != null)
                            sortValue = variableData.MainSeries.Stats.avg;
                        sortedList.Add(sortValue, group);
                    }

                    //set the sorted list as resultTracks
                    ResultTracks.Clear();
                    for (int i = 0; i < numMaxTracks; i++)
                        ResultTracks.Add(sortedList.Values[i]);
                }
            }
        }
    }
}
