using System;
using System.Collections.Generic;
using Caliburn.Micro;

namespace Badger.ViewModels
{
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
    public class LogQuery: PropertyChangedBase
    {
        public const string functionMax = "Max";
        public const string functionMin = "Min";

        public const string noLimitOnResults = "-";

        public const string orderAsc = "Asc";
        public const string orderDesc = "Desc";

        public const string fromAll = "*";
        public const string fromSelection = "Selection";

        private string m_inGroupSelectionFunction = "";
        public string inGroupSelectionFunction
        {
            get { return m_inGroupSelectionFunction; }
            set { m_inGroupSelectionFunction = value;}
        }
        private string m_inGroupSelectionVariable = "";
        public string inGroupSelectionVariable
        {
            get { return m_inGroupSelectionVariable; }
            set { m_inGroupSelectionVariable = value;}
        }
        private string m_orderByFunction = "";
        public string orderByFunction
        {
            get { return m_orderByFunction; }
            set { m_orderByFunction = value;  NotifyOfPropertyChange(() => orderByFunction); }
        }
        private string m_orderByVariable = "";
        public string orderByVariable
        {
            get { return m_orderByVariable; }
            set { m_orderByVariable = value;  NotifyOfPropertyChange(() => orderByVariable); }
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
        private string m_from = "";
        public string from
        {
            get { return m_from; }
            set { m_from = value; }
        }
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

        private BindableCollection<LogQueryResultTrackViewModel> m_resultTracks
            = new BindableCollection<LogQueryResultTrackViewModel>();
        public BindableCollection<LogQueryResultTrackViewModel> resultTracks
        {
            get { return m_resultTracks; }
            set { m_resultTracks = value;  NotifyOfPropertyChange(() => resultTracks); }
        }

        private LogQueryResultTrackViewModel getTrack(Dictionary<string,string> forkValues)
        {
            foreach (LogQueryResultTrackViewModel resultTrack in resultTracks)
            {
                foreach (string forkName in forkValues.Keys)
                {
                    if (!resultTrack.forkValues.ContainsKey(forkName)
                        || forkValues[forkName] != resultTrack.forkValues[forkName])
                    {
                        break;
                    }
                    return resultTrack;
                }

            }
            return null;
        }

        private BindableCollection<LoggedVariableViewModel> m_loggedVariables;
        public BindableCollection<LoggedVariableViewModel> loggedVariables
        {
            get { return m_loggedVariables; }
            set{ m_loggedVariables = value; }
        }

        public void execute(BindableCollection<LoggedExperimentViewModel> experiments
            ,BindableCollection<LoggedVariableViewModel> loggedVariablesVM)
        {
            LogQueryResultTrackViewModel resultTrack= null;
            loggedVariables = loggedVariablesVM;

            //add all selected variables to the list of variables
            foreach (LoggedVariableViewModel variable in loggedVariablesVM)
                if (from == fromSelection || variable.bIsSelected)
                    variables.Add(variable.name);

            //traverse the experimental units within each experiment
            foreach (LoggedExperimentViewModel exp in experiments)
            {
                foreach (LoggedExperimentalUnitViewModel expUnit in exp.expUnits)
                {
                    //take selection into account? is this exp. unit selected?
                    if (from == fromAll || (from == fromSelection && expUnit.bIsSelected))
                    {
                        if (groupBy.Count != 0)
                        {
                            resultTrack = getTrack(expUnit.forkValues);
                            if (resultTrack != null)
                            {
                                //the track exists and we are using forks to group results
                                resultTrack.addTrackData(expUnit.loadTrackData(variables));
                            }
                        }
                        if (resultTrack==null) //New track
                        {
                            //No groups (each experimental unit is a track) or the track doesn't exist
                            //Either way, we create a new track
                            LogQueryResultTrackViewModel newResultTrack = new LogQueryResultTrackViewModel(exp.name);
                            newResultTrack.forkValues = expUnit.forkValues;

                            //if the in-group selection function requires a variable not selected for the report
                            //we add it too to the list of variables read from the log
                            if (inGroupSelectionVariable != "" && !variables.Contains(inGroupSelectionVariable))
                                variables.Add(inGroupSelectionVariable);

                            //if we use some sorting function to select only some tracks, we need to add the variable
                            // to the list too
                            if (limitToOption!= noLimitOnResults && !variables.Contains(orderByVariable))
                                variables.Add(orderByVariable);

                            //load data from the log file
                            newResultTrack.addTrackData(expUnit.loadTrackData(variables));

                            resultTracks.Add(newResultTrack);
                        }
                    }
                }
            }
            //if groups are used, we have to select a single track in each group using the in-group selection function
            //-max(avg(inGroupSelectionVariable)) or min(avg(inGroupSelectionVariable))
            if (groupBy.Count>0)
            {
                foreach (LogQueryResultTrackViewModel track in resultTracks)
                    track.consolidateGroups(inGroupSelectionFunction, inGroupSelectionVariable);
            }

            //if we are using limitTo/orderBy, we have to select the best tracks/groups according to the given criteria
            if (limitToOption!=LogQuery.noLimitOnResults)
            {
                int numMaxTracks = int.Parse(limitToOption);

                if (resultTracks.Count > numMaxTracks)
                {
                    SortedList<double, LogQueryResultTrackViewModel> sortedList;

                    if (orderByFunction == orderAsc)
                        sortedList = new SortedList<double, LogQueryResultTrackViewModel>(resultTracks.Count
                            ,new AscComparer());
                    else
                        sortedList = new SortedList<double, LogQueryResultTrackViewModel>(resultTracks.Count
                            ,new DescComparer());

                    foreach (LogQueryResultTrackViewModel track in resultTracks)
                    {
                        double sortValue= 0.0;
                        TrackVariableData variableData = track.trackData.getVariableData(orderByVariable);
                        if (variableData != null)
                            sortValue = variableData.lastEpisodeData.stats.avg;
                        sortedList.Add(sortValue, track);
                    }

                    //set the sorted list as resultTracks
                    resultTracks.Clear();
                    for (int i= 0; i<numMaxTracks; i++)
                        resultTracks.Add(sortedList.Values[i]);
                }
            }
        }
    }
}
