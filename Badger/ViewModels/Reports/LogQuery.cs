using System;
using System.Collections.Generic;
using Caliburn.Micro;
using Badger.Simion;
using Badger.Data;
using System.ComponentModel;

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
    public class TrackGroupComparer : IComparer<TrackGroup>
    {
        string m_varName;
        bool m_bAsc;
        public TrackGroupComparer(bool asc, string varName)
        {
            m_bAsc = asc;
            m_varName = varName;
        }
        public int Compare(TrackGroup x, TrackGroup y)
        {
            SeriesGroup variableDataX = x.ConsolidatedTrack.GetDataSeriesForOrdering(m_varName);
            if (variableDataX == null) return -1;
            SeriesGroup variableDataY = y.ConsolidatedTrack.GetDataSeriesForOrdering(m_varName);
            if (variableDataY == null) return 1;
            if ((m_bAsc && variableDataX.MainSeries.Stats.avg >= variableDataY.MainSeries.Stats.avg)
                || (!m_bAsc && variableDataX.MainSeries.Stats.avg <= variableDataY.MainSeries.Stats.avg))
                return 1;
            return -1;
        }
    }
    public class LogQueryViewModel : PropertyChangedBase
    {
        public bool ResampleData { get; set; } = false;
        public int ResamplingNumPoints { get; set; } = 100;
        public const string FunctionMax = "Max";
        public const string FunctionMin = "Min";

        public double TimeOffset { get; set; } = 0.0;

        public string InGroupSelectionFunction { get; set; } = FunctionMax;
        public string InGroupSelectionVariable { get; set; } = "";

        public int MaxNumTracks { get; set; } = 10;

        public string OrderByFunction { get; set; } = FunctionMax;
        public string OrderByVariable { get; set; } = "";
        private BindableCollection<string> m_orderByFunctions 
            = new BindableCollection<string>() { FunctionMax, FunctionMin};
        public BindableCollection<string> OrderByFunctions
        {
            get { return m_orderByFunctions; }
            set { m_orderByFunctions = value; NotifyOfPropertyChange(() => OrderByFunctions); }
        }

        // Group By
        private BindableCollection<string> m_groupByForks = new BindableCollection<string>();
        public BindableCollection<string> GroupByForks
        {
            get { return m_groupByForks; }
            set { m_groupByForks = value; NotifyOfPropertyChange(() => GroupByForks); }
        }

        public void AddGroupByFork(string forkName)
        {
            if (!GroupByForks.Contains(forkName))
            {
                m_groupByForks.Add(forkName);
                //NotifyOfPropertyChange(() => GroupBy);
            }
            GroupsEnabled = true;
        }


        public string GroupByAsString
        {
            get
            {
                string s = "";
                for (int i = 0; i < m_groupByForksList.Count; i++) s += m_groupByForksList[i] + ",";
                s= s.TrimEnd(',');
                return s;
            }
        }

        public void ResetGroupBy()
        {
            GroupsEnabled = false;
            m_groupByForks.Clear();
            NotifyOfPropertyChange(() => GroupByForksList);
        }

        private BindableCollection<string> m_inGroupSelectionFunctions 
            = new BindableCollection<string>() { FunctionMax, FunctionMin };
        public BindableCollection<string> InGroupSelectionFunctions
        {
            get { return m_inGroupSelectionFunctions; }
            set { m_inGroupSelectionFunctions = value; NotifyOfPropertyChange(() => InGroupSelectionFunctions); }
        }
        private bool m_bGroupsEnabled; // no groups by default
        public bool GroupsEnabled
        {
            get { return m_bGroupsEnabled; }
            set { m_bGroupsEnabled = value; NotifyOfPropertyChange(() => GroupsEnabled); }
        }

        // Limit to
        private BindableCollection<string> m_maxNumTrackOptions = new BindableCollection<string>();

        public BindableCollection<string> MaxNumTracksOptions
        {
            get { return m_maxNumTrackOptions; }
            set { m_maxNumTrackOptions = value; NotifyOfPropertyChange(() => MaxNumTracksOptions); }
        }

        bool m_useForkSelection = false;
        public bool UseForkSelection
        {
            get { return m_useForkSelection; }
            set { m_useForkSelection = value; NotifyOfPropertyChange(() => UseForkSelection); }
        }
        private List<string> m_groupByForksList = new List<string>();
        public List<string> GroupByForksList { get { return m_groupByForksList; } }

        public LogQueryViewModel()
        {
            // Add the limit to options
            for (int i = 0; i <= 10; i++) MaxNumTracksOptions.Add(i.ToString());
            NotifyOfPropertyChange(() => OrderByFunctions);
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

        private TrackGroup GetTrackGroup(Dictionary<string, string> forkValues)
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

        public BindableCollection<string> Variables { get; set; }
            = new BindableCollection<string>();
        public BindableCollection<LoggedVariableViewModel> VariablesVM { get; }
            = new BindableCollection<LoggedVariableViewModel>();

        bool IsVariableSelected(string variable)
        {
            foreach (LoggedVariableViewModel variableVM in VariablesVM)
            {
                if (variableVM.name == variable)
                {
                    return variableVM.IsSelected;
                }
            }
            return false;
        }

        void OnChildPropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            //not all properties sending changes are due to "Group by this fork", so we need to check it
            if (e.PropertyName == "IsSelected")
            {
                ValidateQuery();
            }

        }

        public void AddLogVariables(List<string> variables)
        {
            foreach (string variable in variables)
            {
                bool bFound = false;
                foreach (LoggedVariableViewModel variableVM in VariablesVM)
                {
                    if (variableVM.name == variable)
                    {
                        bFound = true;
                        break;
                    }
                }
                if (!bFound)
                {
                    LoggedVariableViewModel newVariableVM= new LoggedVariableViewModel(variable);
                    VariablesVM.Add(newVariableVM);
                    newVariableVM.PropertyChanged += OnChildPropertyChanged;
                    Variables.Add(variable);
                }
            }
        }

        public void Init()
        {
            if (VariablesVM.Count > 0)
            {
                InGroupSelectionVariable = VariablesVM[0].name;
                OrderByVariable = VariablesVM[0].name;
            }
        }

        public List<Report> Reports = new List<Report>();

        private bool m_bCanGenerateReports = false;

        public bool CanGenerateReports
        {
            get { return m_bCanGenerateReports; }
            set { m_bCanGenerateReports = value; NotifyOfPropertyChange(() => CanGenerateReports); }
        }

        /// <summary>
        /// This function validates the current query and sets CanGenerateReports accordingly
        /// </summary>
        public void ValidateQuery()
        {
            // Validate the current query
            int numSelectedVars = 0;

            foreach (LoggedVariableViewModel variable in VariablesVM)
                if (variable.IsSelected) ++numSelectedVars;

            if (numSelectedVars == 0 || InGroupSelectionVariable == "")
                CanGenerateReports = false;
            else
                CanGenerateReports = true;

            // Update the "enabled" property of the variable used to select a group
            GroupsEnabled = GroupByForks.Count > 0;
        }

        public void Execute(BindableCollection<LoggedExperimentViewModel> experiments)
        {
            TrackGroup resultTrackGroup = null;

            Reports = DataTrackUtilities.FromLoggedVariables(VariablesVM);

            //if the in-group selection function requires a variable not selected for the report
            //we add it too to the list of variables read from the log
            if (InGroupSelectionVariable != "" && !IsVariableSelected(InGroupSelectionVariable))
                Reports.Add(new Report(InGroupSelectionVariable, ReportType.LastEvaluation
                    , ProcessFunc.None));

            //if we use some sorting function to select only some tracks, we need to add the variable
            // to the list too
            if (MaxNumTracks != 0 && !IsVariableSelected(OrderByVariable))
                Reports.Add(new Report(OrderByVariable, ReportType.LastEvaluation, ProcessFunc.None));

            //set the data resampling options
            foreach (Report report in Reports)
            {
                report.Resample = ResampleData;
                report.NumSamples = ResamplingNumPoints;
            }

            //traverse the experimental units within each experiment
            foreach (LoggedExperimentViewModel exp in experiments)
            {
                foreach (LoggedExperimentalUnitViewModel expUnit in exp.ExperimentalUnits)
                {
                    //take selection into account? is this exp. unit selected?
                    if (!UseForkSelection || (UseForkSelection && expUnit.IsSelected))
                    {
                        if (GroupByForksList.Count != 0)
                        {
                            resultTrackGroup = GetTrackGroup(expUnit.forkValues);
                            if (resultTrackGroup != null)
                            {
                                //the track exists and we are using forks to group results
                                Track trackData = expUnit.LoadTrackData(Reports);
                                if (trackData!=null)
                                    resultTrackGroup.AddTrackData(trackData);

                                //It is not the first track in the track group, so we consolidate it asap
                                //to avoid using unnecessary amounts of memory
                                //Consolidate selects a single track in each group using the in-group selection function
                                //-max(avg(inGroupSelectionVariable)) or min(avg(inGroupSelectionVariable))
                                //and also names groups depending on the number of tracks in the group
                                resultTrackGroup.Consolidate(InGroupSelectionFunction, InGroupSelectionVariable, GroupByForksList);
                            }
                        }
                        if (resultTrackGroup == null) //New track group
                        {
                            //No groups (each experimental unit is a track) or the track doesn't exist
                            //Either way, we create a new track
                            TrackGroup newResultTrackGroup = new TrackGroup(exp.Name);

                            if (GroupByForksList.Count == 0)
                                newResultTrackGroup.SetForkValues(expUnit.forkValues);
                            else
                                foreach (string group in GroupByForksList)
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
                        //Limit the number of tracks asap
                        //if we are using limitTo/orderBy, we have to select the best tracks/groups according to the given criteria
                        if (MaxNumTracks != 0)
                        {
                            if (ResultTracks.Count > MaxNumTracks)
                            {
                                m_resultTracks.Sort(new TrackGroupComparer(OrderByFunction == FunctionMin, OrderByVariable));
                                ResultTracks.RemoveRange(MaxNumTracks, m_resultTracks.Count - MaxNumTracks);
                            }
                        }
                    }
                }
            }
        }
    }
}
