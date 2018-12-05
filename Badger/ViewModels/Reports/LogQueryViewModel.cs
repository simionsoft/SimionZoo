using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Windows.Data;
using System.Globalization;
using System.Runtime.Serialization;

using Caliburn.Micro;

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
    public class TrackGroupComparer : IComparer<TrackGroup>
    {
        string m_varName;
        bool m_bAsc;
        bool m_bUseBeauty;
        ReportType m_orderByReportType;
        public TrackGroupComparer(bool asc, bool bUseBeauty, string varName, string orderByReportType)
        {
            m_bAsc = asc;
            m_varName = varName;
            m_bUseBeauty = bUseBeauty;

            EnumDescriptionConverter conv = new EnumDescriptionConverter();
            m_orderByReportType = (ReportType)((IValueConverter)conv).ConvertBack(orderByReportType, typeof(ReportType), null, CultureInfo.CurrentCulture);
        }
        public int Compare(TrackGroup x, TrackGroup y)
        {
            SeriesGroup variableDataX = x.ConsolidatedTrack.GetDataSeries(m_varName, m_orderByReportType);
            if (variableDataX == null) return -1;
            SeriesGroup variableDataY = y.ConsolidatedTrack.GetDataSeries(m_varName, m_orderByReportType);
            if (variableDataY == null) return 1;
            if (!m_bUseBeauty)
            {
                if ((m_bAsc && variableDataX.MainSeries.Stats.avg >= variableDataY.MainSeries.Stats.avg)
                    || (!m_bAsc && variableDataX.MainSeries.Stats.avg <= variableDataY.MainSeries.Stats.avg))
                    return 1;
                return -1;
            }
            else
            {
                if ((m_bAsc && variableDataX.MainSeries.Stats.ascBeauty <= variableDataY.MainSeries.Stats.ascBeauty)
                    || (!m_bAsc && variableDataX.MainSeries.Stats.dscBeauty <= variableDataY.MainSeries.Stats.dscBeauty))
                    return 1;
                return -1;
            }
        }
    }
    [DataContract]
    public class LogQueryViewModel : PropertyChangedBase
    {

        public const string FunctionMax = "Max";
        public const string FunctionMin = "Min";
        public const string FunctionAscBeauty = "AscBeauty";
        public const string FunctionDscBeauty = "DscBeauty";

        [DataMember]
        static int m_numInstances = 0;

        public int QueryId { get; set; }

        const int DefaultMaxNumTracks = 10;

        [DataMember]
        public bool ResampleData { get; set; } = false;
        [DataMember]
        public int ResamplingNumPoints { get; set; } = 100;

        [DataMember]
        public double TimeOffset { get; set; } = 0.0;
        [DataMember]
        public double MinEpisodeLength { get; set; } = 0.0;

        string m_inGroupSelectionFunction;
        [DataMember]
        public string InGroupSelectionFunction
        {
            get { return m_inGroupSelectionFunction; }
            set { m_inGroupSelectionFunction = value; NotifyOfPropertyChange(() => InGroupSelectionFunction); }
        }

        string m_inGroupSelectionVariable;
        [DataMember]
        public string InGroupSelectionVariable
        {
            get { return m_inGroupSelectionVariable; }
            set { m_inGroupSelectionVariable = value; NotifyOfPropertyChange(() => InGroupSelectionVariable); }
        }

        [DataMember]
        public BindableCollection<string> InGroupSelectionReportTypes { get; set; } = new BindableCollection<string>();

        private string m_inGroupSelectionReportType;
        [DataMember]
        public string InGroupSelectionReportType
        {
            get { return m_inGroupSelectionReportType; }
            set { m_inGroupSelectionReportType = value; NotifyOfPropertyChange(() => InGroupSelectionReportType); }
        }

        // Limit to
        bool m_bLimitTracks = false;
        [DataMember]
        public bool LimitTracks
        {
            get { return m_bLimitTracks; }
            set
            {
                m_bLimitTracks = value;
                NotifyOfPropertyChange(() => LimitTracks);
            }
        }
        private BindableCollection<int> m_maxNumTrackOptions = new BindableCollection<int>();
        [DataMember]
        public BindableCollection<int> MaxNumTracksOptions
        {
            get { return m_maxNumTrackOptions; }
            set { m_maxNumTrackOptions = value; NotifyOfPropertyChange(() => MaxNumTracksOptions); }
        }
        private int m_maxNumTracks;
        [DataMember]
        public int MaxNumTracks
        {
            get { return m_maxNumTracks; }
            set { m_maxNumTracks = value; NotifyOfPropertyChange(() => MaxNumTracks); }
        }


        //Order by
        [DataMember]
        public BindableCollection<string> OrderByReportTypes { get; set; } = new BindableCollection<string>();

        private string m_orderByReportType;
        [DataMember]
        public string OrderByReportType
        {
            get { return m_orderByReportType; }
            set { m_orderByReportType = value; NotifyOfPropertyChange(() => OrderByReportType); }
        }
        private string m_orderByFunction;
        [DataMember]
        public string OrderByFunction
        {
            get { return m_orderByFunction; }
            set { m_orderByFunction = value; NotifyOfPropertyChange(() => OrderByFunction); }
        }
        private string m_orderByVariable;
        [DataMember]
        public string OrderByVariable
        {
            get { return m_orderByVariable; }
            set { m_orderByVariable = value; NotifyOfPropertyChange(() => OrderByVariable); }
        }

        [DataMember]
        public BindableCollection<string> OrderByFunctions { get; set; }
            = new BindableCollection<string>() { FunctionMax, FunctionMin, FunctionAscBeauty, FunctionDscBeauty };
        [DataMember]
        public BindableCollection<string> OrderByVariables
        {
            get { return Variables; }
            set { Variables = value; NotifyOfPropertyChange(() => OrderByVariables); }
        }

        // Group By
        [DataMember]
        public BindableCollection<string> GroupByForks { get; set; } = new BindableCollection<string>();

        public void AddGroupByFork(string forkName)
        {
            if (!GroupByForks.Contains(forkName))
            {
                GroupByForks.Add(forkName);
            }
            GroupsEnabled = true;
        }

        public bool IsForkUsedToGroup(string forkName)
        {
            return GroupByForks.Contains(forkName);
        }

        public void ResetGroupBy()
        {
            //If several experiments have been loaded we do not remove the experiment-id from the list
            bool bGroupByExperiment = GroupByForks.Contains(ReportsWindowViewModel.GroupByExperimentId);
            GroupsEnabled = false;
            GroupByForks.Clear();
            if (bGroupByExperiment)
                GroupByForks.Add(ReportsWindowViewModel.GroupByExperimentId);
            else
                GroupsEnabled = false;
        }

           
        [DataMember]
        public BindableCollection<string> InGroupSelectionFunctions { get; set; }
            = new BindableCollection<string>() { FunctionMax, FunctionMin, FunctionAscBeauty, FunctionDscBeauty };

        private bool m_bGroupsEnabled; // no groups by default
        [DataMember]
        public bool GroupsEnabled
        {
            get { return m_bGroupsEnabled; }
            set { m_bGroupsEnabled = value; NotifyOfPropertyChange(() => GroupsEnabled); }
        }

        bool m_useForkSelection = false;
        [DataMember]
        public bool UseForkSelection
        {
            get { return m_useForkSelection; }
            set { m_useForkSelection = value; NotifyOfPropertyChange(() => UseForkSelection); }
        }

        private bool m_bGroupByExperiment = false;
        public bool GroupByExperiment
        {
            get { return m_bGroupByExperiment; }
            set
            {
                //If there is more than one experiment, use it to group tracks
                if (value)
                {
                    AddGroupByFork(ReportsWindowViewModel.GroupByExperimentId);
                }
                else
                {
                    if (GroupByForks.Count == 1)
                        GroupsEnabled = false;
                    GroupByForks.Remove(ReportsWindowViewModel.GroupByExperimentId);
                }
                m_bGroupByExperiment = value;
            }
        }

        public LogQueryViewModel()
        {
            m_numInstances++;
            QueryId = m_numInstances;

            EnumDescriptionConverter conv = new EnumDescriptionConverter();
            InGroupSelectionReportTypes.Add((string)((IValueConverter)conv).Convert(ReportType.LastEvaluation, typeof(ReportType), null, CultureInfo.CurrentCulture));
            InGroupSelectionReportTypes.Add((string)((IValueConverter)conv).Convert(ReportType.EvaluationAverages, typeof(ReportType), null, CultureInfo.CurrentCulture));
            InGroupSelectionReportType = (string)((IValueConverter)conv).Convert(ReportType.LastEvaluation, typeof(ReportType), null, CultureInfo.CurrentCulture);
            OrderByReportTypes.Add((string)((IValueConverter)conv).Convert(ReportType.LastEvaluation, typeof(ReportType), null, CultureInfo.CurrentCulture));
            OrderByReportTypes.Add((string)((IValueConverter)conv).Convert(ReportType.EvaluationAverages, typeof(ReportType), null, CultureInfo.CurrentCulture));
            OrderByReportType = (string)((IValueConverter)conv).Convert(ReportType.LastEvaluation, typeof(ReportType), null, CultureInfo.CurrentCulture);

            // Add the limit to options
            MaxNumTracksOptions.Clear();
            for (int i = 0; i <= 10; i++) MaxNumTracksOptions.Add(i);
            MaxNumTracks = DefaultMaxNumTracks;

            OrderByFunction = FunctionMax;
            NotifyOfPropertyChange(() => OrderByFunctions);

            if (InGroupSelectionVariables.Count > 0) InGroupSelectionVariable = InGroupSelectionVariables[0];
            NotifyOfPropertyChange(() => InGroupSelectionVariables);

            if (OrderByVariables.Count > 0) OrderByVariable = OrderByVariables[0];
            NotifyOfPropertyChange(() => OrderByVariables);

            //Add the listening function to the LogQuery object with all the parameters
            PropertyChanged += OnChildPropertyChanged;
        }

        public List<Report> GetTracksParameters(List<TrackGroup> resultTracks)
        {
            List<Report> list = new List<Report>();
            foreach(TrackGroup group in resultTracks)
            {
                //we iterate over the TrackParameters of the result track
                foreach (Report parameters in group.ConsolidatedTrack.SeriesGroups.Keys)
                    if (!list.Contains(parameters))
                        list.Add(parameters);
            }
            return list;
        }

        private TrackGroup GetTrackGroup(List<TrackGroup> resultTracks, Dictionary<string, string> forkValues
            , string experimentId)
        {
            uint numMatchedForks;
            foreach (TrackGroup resultTrack in resultTracks)
            {
                numMatchedForks = 0;

                //same experiment id??
                if (resultTrack.ForkValues.ContainsKey(ReportsWindowViewModel.GroupByExperimentId)
                    && resultTrack.ExperimentId == experimentId)
                        numMatchedForks++;
                
                //match regular forks
                foreach (string forkName in forkValues.Keys)
                {
                    if (resultTrack.ForkValues.ContainsKey(forkName) 
                        && forkValues[forkName] == resultTrack.ForkValues[forkName])
                    { 
                        numMatchedForks++;
                    }
                }

                //all forks have been matched?
                if (numMatchedForks == GroupByForks.Count)
                    return resultTrack;

            }
            return null;
        }
        [DataMember]
        private BindableCollection<string> Variables = new BindableCollection<string>();
        [DataMember]
        public BindableCollection<string> InGroupSelectionVariables
        {
            get { return Variables; }
            set { Variables = value; NotifyOfPropertyChange(() => InGroupSelectionVariables); }
        }

        [DataMember]
        public BindableCollection<LoggedVariableViewModel> VariablesVM { get; set; }
            = new BindableCollection<LoggedVariableViewModel>();

        string GetVariableProcessFunc(string variable)
        {
            foreach (LoggedVariableViewModel variableVM in VariablesVM)
            {
                if (variableVM.Name == variable)
                {
                    return variableVM.SelectedProcessFunc;
                }
            }
            return null;
        }
        bool IsVariableSelected(string variable, string reportType)
        {
            foreach (LoggedVariableViewModel variableVM in VariablesVM)
            {
                if (variableVM.Name == variable)
                {
                    if (variableVM.IsSelected)
                    {
                        foreach (string selectedType in variableVM.SelectedPlotTypes)
                        {
                            if (selectedType == reportType)
                                return true;
                        }
                    }
                    //either the variable was not selected or the requested report type wasn't
                    return false;
                }
            }
            return false;
        }

        void OnChildPropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            if (e.PropertyName == "IsSelected")
            {
                Validate();
            }
        }

        public void AddLogVariables(List<string> variables)
        {
            foreach (string variable in variables)
            {
                bool bFound = LogVariableExists(variable);
                if (!bFound)
                {
                    LoggedVariableViewModel newVariableVM= new LoggedVariableViewModel(variable);
                    VariablesVM.Add(newVariableVM);
                    newVariableVM.PropertyChanged += OnChildPropertyChanged;
                    InGroupSelectionVariables.Add(variable);
                }
            }
        }

        public bool LogVariableExists(string variable)
        {
            foreach (LoggedVariableViewModel variableVM in VariablesVM)
            {
                if (variableVM.Name == variable)
                    return true;
            }
            return false;
        }

        private bool m_bCanGenerateReports = false;
        [DataMember]
        public bool CanGenerateReports
        {
            get { return m_bCanGenerateReports; }
            set { m_bCanGenerateReports = value; NotifyOfPropertyChange(() => CanGenerateReports); }
        }

        /// <summary>
        /// This function validates the current query and sets CanGenerateReports accordingly
        /// </summary>
        public void Validate()
        {
            // Make sure at least one variable is selected
            int numSelectedVars = 0;

            foreach (LoggedVariableViewModel variable in VariablesVM)
                if (variable.IsSelected) ++numSelectedVars;

            //check selected items are valid
            if (!LogVariableExists(InGroupSelectionVariable))
            {
                if (InGroupSelectionVariables.Count > 0)
                    InGroupSelectionVariable = InGroupSelectionVariables[0];
                else
                    InGroupSelectionVariable = null;
            }

            if (!LogVariableExists(OrderByVariable))
            {
                if (OrderByVariables.Count > 0)
                    OrderByVariable = OrderByVariables[0];
                else
                    OrderByVariable = null;
            }

            if (numSelectedVars == 0 || (GroupsEnabled && (InGroupSelectionVariable == null || InGroupSelectionVariable == "")))
                CanGenerateReports = false;
            else
                CanGenerateReports = true;

            // Update the "enabled" property of the variable used to select a group
            GroupsEnabled = GroupByForks.Count > 0;

        }

        public void Execute(BindableCollection<LoggedExperimentViewModel> experiments
            ,Files.LoadUpdateFunction loadUpdateFunction, out List<TrackGroup> resultTracks, out List<Report> reports)
        {
            TrackGroup resultTrackGroup = null;

            //initialize the output lists
            resultTracks = new List<TrackGroup>();
            reports = DataTrackUtilities.FromLoggedVariables(VariablesVM);

            //if the in-group selection function requires a variable not selected for the report
            //we add it too to the list of variables read from the log
            if (GroupsEnabled && InGroupSelectionVariable != null && InGroupSelectionVariable != ""
                && !IsVariableSelected(InGroupSelectionVariable,InGroupSelectionReportType))
            {
                EnumDescriptionConverter conv = new EnumDescriptionConverter();
                ReportType reportType = (ReportType)((IValueConverter)conv).ConvertBack(InGroupSelectionReportType, typeof(ReportType), null, CultureInfo.CurrentCulture);

                reports.Add(new Report(InGroupSelectionVariable, reportType, GetVariableProcessFunc(InGroupSelectionVariable)));
            }

            //if we use some sorting function to select only some tracks, we need to add the variable
            // to the list too
            if (LimitTracks && !IsVariableSelected(OrderByVariable, OrderByReportType))
            {
                EnumDescriptionConverter conv = new EnumDescriptionConverter();
                ReportType reportType = (ReportType)((IValueConverter)conv).ConvertBack(OrderByReportType, typeof(ReportType), null, CultureInfo.CurrentCulture);

                reports.Add(new Report(OrderByVariable, reportType, GetVariableProcessFunc(OrderByVariable)));
            }

            //set the data resampling options
            foreach (Report report in reports)
            {
                report.Resample = ResampleData;
                report.NumSamples = ResamplingNumPoints;
                report.TimeOffset = TimeOffset;
                report.MinEpisodeLength = MinEpisodeLength;
            }

            //traverse the experimental units within each experiment
            foreach (LoggedExperimentViewModel exp in experiments)
            {
                foreach (LoggedExperimentalUnitViewModel expUnit in exp.ExperimentalUnits)
                {
                    //take selection into account? is this exp. unit selected?
                    bool expUnitContainsGroupByForks = expUnit.ContainsForks(GroupByForks);
                    if ((!UseForkSelection || (UseForkSelection && expUnit.IsSelected))
                        && expUnitContainsGroupByForks)
                    {
                        resultTrackGroup = null;
                        if (GroupByForks.Count != 0)
                        {
                            resultTrackGroup = GetTrackGroup(resultTracks, expUnit.forkValues, exp.Name);
                            if (resultTrackGroup != null)
                            {
                                //the track exists and we are using forks to group results
                                Track trackData = expUnit.LoadTrackData(reports);
                                if (trackData!=null)
                                    resultTrackGroup.AddTrackData(trackData);

                                //It is not the first track in the track group, so we consolidate it asap
                                //to avoid using unnecessary amounts of memory
                                //Consolidate selects a single track in each group using the in-group selection function
                                //-max(avg(inGroupSelectionVariable)) or min(avg(inGroupSelectionVariable))
                                //and also names groups depending on the number of tracks in the group
                                resultTrackGroup.Consolidate(InGroupSelectionFunction, InGroupSelectionVariable
                                    , InGroupSelectionReportType,GroupByForks);
                            }
                        }
                        if (resultTrackGroup == null && expUnitContainsGroupByForks) //New track group
                        {
                            //No groups (each experimental unit is a track) or the track doesn't exist
                            //Either way, we create a new track
                            TrackGroup newResultTrackGroup = new TrackGroup(exp.Name);

                            if (GroupByForks.Count == 0)
                                newResultTrackGroup.ForkValues= expUnit.forkValues;
                            else
                                foreach (string forkName in GroupByForks)
                            {
                                    //an experimental unit may not have a fork used to group
                                    if (expUnit.forkValues.ContainsKey(forkName))
                                        newResultTrackGroup.ForkValues[forkName] = expUnit.forkValues[forkName];
                                    else if (forkName == ReportsWindowViewModel.GroupByExperimentId)
                                        newResultTrackGroup.ForkValues[forkName] = exp.Name;
                            }

                            //load data from the log file
                            Track trackData = expUnit.LoadTrackData(reports);

                            if (trackData != null)
                            {
                                //for now, we just ignore failed experiments. Maybe we could do something more sophisticated
                                //for example, allow to choose only those parameter variations that lead to failed experiments
                                if (trackData.HasData)
                                    newResultTrackGroup.AddTrackData(trackData);

                                //we only consider those tracks with data loaded
                                if (newResultTrackGroup.HasData)
                                    resultTracks.Add(newResultTrackGroup);
                            }
                        }
                        //Limit the number of tracks asap
                        //if we are using limitTo/orderBy, we have to select the best tracks/groups according to the given criteria
                        if (LimitTracks)
                        {
                            if (resultTracks.Count > MaxNumTracks)
                            {
                                bool asc = (OrderByFunction == FunctionMin) || (OrderByFunction == FunctionAscBeauty);
                                bool useBeauty = (OrderByFunction == FunctionDscBeauty) || (OrderByFunction == FunctionAscBeauty);
                                resultTracks.Sort(new TrackGroupComparer(asc, useBeauty, OrderByVariable, OrderByReportType));
                                resultTracks.RemoveRange(MaxNumTracks, resultTracks.Count - MaxNumTracks);
                            }
                        }
                    }
                    loadUpdateFunction?.Invoke();
                }
            }
        }
        public void SetNotifying(bool notifying)
        {
            IsNotifying = notifying;
            foreach (LoggedVariableViewModel variable in VariablesVM)
                variable.IsNotifying = notifying;
        }
    }
}
