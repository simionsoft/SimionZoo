using System;
using System.Collections.Generic;
using Caliburn.Micro;
using Badger.Simion;
using Badger.Data;
using System.ComponentModel;
using System.Windows.Data;
using System.Globalization;

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
    public class LogQueryViewModel : PropertyChangedBase
    {
        const int DefaultMaxNumTracks = 10;
        public bool ResampleData { get; set; } = false;
        public int ResamplingNumPoints { get; set; } = 100;
        public const string FunctionMax = "Max";
        public const string FunctionMin = "Min";
        public const string FunctionAscBeauty = "AscBeauty";
        public const string FunctionDscBeauty = "DscBeauty";

        public double TimeOffset { get; set; } = 0.0;

        string m_inGroupSelectionFunction;
        public string InGroupSelectionFunction
        {
            get { return m_inGroupSelectionFunction; }
            set { m_inGroupSelectionFunction = value; NotifyOfPropertyChange(() => InGroupSelectionFunction); }
        }
        string m_inGroupSelectionVariable;
        public string InGroupSelectionVariable
        {
            get { return m_inGroupSelectionVariable; }
            set { m_inGroupSelectionVariable = value; NotifyOfPropertyChange(() => InGroupSelectionVariable); }
        }
        private BindableCollection<string> m_inGroupSelectionReportTypes = new BindableCollection<string>();
        public BindableCollection<string> InGroupSelectionReportTypes
        {
            get { return m_inGroupSelectionReportTypes; }
        }
        private string m_inGroupSelectionReportType;
        public string InGroupSelectionReportType
        {
            get { return m_inGroupSelectionReportType; }
            set { m_inGroupSelectionReportType = value; NotifyOfPropertyChange(() => InGroupSelectionReportType); }
        }

        // Limit to
        private BindableCollection<int> m_maxNumTrackOptions = new BindableCollection<int>();
        public BindableCollection<int> MaxNumTracksOptions
        {
            get { return m_maxNumTrackOptions; }
            set { m_maxNumTrackOptions = value; NotifyOfPropertyChange(() => MaxNumTracksOptions); }
        }
        private int m_maxNumTracks;
        public int MaxNumTracks
        {
            get { return m_maxNumTracks; }
            set { m_maxNumTracks = value;NotifyOfPropertyChange(() => MaxNumTracks); }
        }


        //Order by
        private BindableCollection<string> m_orderByReportTypes = new BindableCollection<string>();
        public BindableCollection<string> OrderByReportTypes
        {
            get { return m_orderByReportTypes; }
        }
        private string m_orderByReportType;
        public string OrderByReportType
        {
            get { return m_orderByReportType; }
            set { m_orderByReportType = value; NotifyOfPropertyChange(() => OrderByReportType); }
        }
        private string m_orderByFunction;
        public string OrderByFunction
        {
            get { return m_orderByFunction; }
            set { m_orderByFunction = value;NotifyOfPropertyChange(() => OrderByFunction); }
        }
        private string m_orderByVariable;
        public string OrderByVariable
        {
            get { return m_orderByVariable; }
            set { m_orderByVariable = value; NotifyOfPropertyChange(() => OrderByVariable); }
        }
        private BindableCollection<string> m_orderByFunctions 
            = new BindableCollection<string>() { FunctionMax, FunctionMin, FunctionAscBeauty, FunctionDscBeauty};
        public BindableCollection<string> OrderByFunctions
        {
            get { return m_orderByFunctions; }
            set { m_orderByFunctions = value; NotifyOfPropertyChange(() => OrderByFunctions); }
        }
        public BindableCollection<string> OrderByVariables
        {
            get { return m_variables; }
            set { m_variables = value; NotifyOfPropertyChange(() => OrderByVariables); }
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
                NotifyOfPropertyChange(() => GroupByForks);
            }
            GroupsEnabled = true;
        }

        public void Reset()
        {
            this.CanGenerateReports = false;
            this.GroupByForks.Clear();
            this.GroupByForksList.Clear();
            this.GroupsEnabled = false;
            this.InGroupSelectionFunction = FunctionMax;
            this.InGroupSelectionVariable = null;
            
            this.MaxNumTracks = DefaultMaxNumTracks;
        }

        public void ResetGroupBy()
        {
            GroupsEnabled = false;
            GroupByForks.Clear();
        }

        private BindableCollection<string> m_inGroupSelectionFunctions 
            = new BindableCollection<string>() { FunctionMax, FunctionMin, FunctionAscBeauty, FunctionDscBeauty };
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
            EnumDescriptionConverter conv = new EnumDescriptionConverter();
            InGroupSelectionReportTypes.Add((string)((IValueConverter)conv).Convert(ReportType.LastEvaluation, typeof(ReportType), null, CultureInfo.CurrentCulture));
            InGroupSelectionReportTypes.Add((string)((IValueConverter)conv).Convert(ReportType.EvaluationAverages, typeof(ReportType), null, CultureInfo.CurrentCulture));
            InGroupSelectionReportType = (string)((IValueConverter)conv).Convert(ReportType.LastEvaluation, typeof(ReportType), null, CultureInfo.CurrentCulture);
            OrderByReportTypes.Add((string)((IValueConverter)conv).Convert(ReportType.LastEvaluation, typeof(ReportType), null, CultureInfo.CurrentCulture));
            OrderByReportTypes.Add((string)((IValueConverter)conv).Convert(ReportType.EvaluationAverages, typeof(ReportType), null, CultureInfo.CurrentCulture));
            OrderByReportType = (string)((IValueConverter)conv).Convert(ReportType.LastEvaluation, typeof(ReportType), null, CultureInfo.CurrentCulture);
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
                if (numMatchedForks == GroupByForks.Count)
                    return resultTrack;

            }
            return null;
        }

        private BindableCollection<string> m_variables = new BindableCollection<string>();
        public BindableCollection<string> InGroupSelectionVariables
        {
            get { return m_variables; }
            set { m_variables = value; NotifyOfPropertyChange(() => InGroupSelectionVariables); }
        }
            
        public BindableCollection<LoggedVariableViewModel> VariablesVM { get; }
            = new BindableCollection<LoggedVariableViewModel>();

        string GetVariableProcessFunc(string variable)
        {
            foreach (LoggedVariableViewModel variableVM in VariablesVM)
            {
                if (variableVM.name == variable)
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
                if (variableVM.name == variable)
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
                    InGroupSelectionVariables.Add(variable);
                }
            }
        }

        public void OnExperimentBatchLoaded()
        {
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

            if (numSelectedVars == 0 || (GroupsEnabled && (InGroupSelectionVariable == null || InGroupSelectionVariable == "")))
                CanGenerateReports = false;
            else
                CanGenerateReports = true;

            // Update the "enabled" property of the variable used to select a group
            GroupsEnabled = GroupByForks.Count > 0;
        }

        public void Execute(BindableCollection<LoggedExperimentViewModel> experiments
            ,SimionFileData.LoadUpdateFunction loadUpdateFunction)
        {
            TrackGroup resultTrackGroup = null;
            ResultTracks.Clear();

            Reports = DataTrackUtilities.FromLoggedVariables(VariablesVM);

            //if the in-group selection function requires a variable not selected for the report
            //we add it too to the list of variables read from the log
            if (InGroupSelectionVariable != null && InGroupSelectionVariable != ""
                && !IsVariableSelected(InGroupSelectionVariable,InGroupSelectionReportType))
            {
                EnumDescriptionConverter conv = new EnumDescriptionConverter();
                ReportType reportType = (ReportType)((IValueConverter)conv).ConvertBack(InGroupSelectionReportType, typeof(ReportType), null, CultureInfo.CurrentCulture);

                Reports.Add(new Report(InGroupSelectionVariable, reportType, GetVariableProcessFunc(InGroupSelectionVariable)));
            }

            //if we use some sorting function to select only some tracks, we need to add the variable
            // to the list too
            if (MaxNumTracks != 0 && !IsVariableSelected(OrderByVariable, OrderByReportType))
            {
                EnumDescriptionConverter conv = new EnumDescriptionConverter();
                ReportType reportType = (ReportType)((IValueConverter)conv).ConvertBack(OrderByReportType, typeof(ReportType), null, CultureInfo.CurrentCulture);

                Reports.Add(new Report(OrderByVariable, reportType, GetVariableProcessFunc(OrderByVariable)));
            }

            //set the data resampling options
            foreach (Report report in Reports)
            {
                report.Resample = ResampleData;
                report.NumSamples = ResamplingNumPoints;
                report.TimeOffset = TimeOffset;
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
                                bool asc = (OrderByFunction == FunctionMin) || (OrderByFunction == FunctionAscBeauty);
                                bool useBeauty = (OrderByFunction == FunctionDscBeauty) || (OrderByFunction == FunctionAscBeauty);
                                m_resultTracks.Sort(new TrackGroupComparer(asc, useBeauty, OrderByVariable, OrderByReportType));
                                ResultTracks.RemoveRange(MaxNumTracks, m_resultTracks.Count - MaxNumTracks);
                            }
                        }
                    }
                    loadUpdateFunction?.Invoke();
                }
            }
        }
    }
}
