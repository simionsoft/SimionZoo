
using System.Collections.Generic;
using Caliburn.Micro;

namespace Badger.ViewModels
{
    public class LogQuery: PropertyChangedBase
    {
        public const string FunctionNone = "";
        public const string FunctionMax = "Max";
        public const string FunctionMin = "Min";
        //public const string FunctionAvg = "Avg";

        public const string FromAll = "*";
        public const string FromSelection = "Selection";

        private string m_havingFunction = "";
        public string havingFunction
        {
            get { return m_havingFunction; }
            set { m_havingFunction = value;}
        }
        private string m_havingVariable = "";
        public string havingVariable
        {
            get { return m_havingVariable; }
            set { m_havingVariable = value;}
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

        //public override string ToString()
        //{
        //    string query = "";
        //    query = "SELECT ";
        //    if (m_havingFunction != "") query += m_havingFunction + "(" + m_variable + ")";
        //    else query += m_variable;
        //    query+= " FROM " + m_from;
        //    if (m_groupBy.Count != 0) query += " GROUP BY " + groupByAsString;
            
        //    return query;
        //}

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

        public void execute(BindableCollection<LoggedExperimentViewModel> experiments
            ,BindableCollection<LoggedVariableViewModel> loggedVariables)
        {
            LogQueryResultTrackViewModel resultTrack= null;

            //add all selected variables to the list of variables
            foreach (LoggedVariableViewModel variable in loggedVariables)
                if (from == FromSelection || variable.bIsSelected)
                    variables.Add(variable.name);


            //traverse the experimental units within each experiment
            foreach (LoggedExperimentViewModel exp in experiments)
            {
                foreach (LoggedExperimentalUnitViewModel expUnit in exp.expUnits)
                {
                    //do take selection into account? is this exp. unit selected?
                    if (from == FromAll || (from == FromSelection && expUnit.bIsSelected))
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

                            //if the having selection function requires a variable not selected for the report
                            //we add it too to the list of variables read from the log
                            if (havingVariable != "" && !variables.Contains(havingVariable))
                                variables.Add(m_havingVariable);
                            //load data from the log file
                            newResultTrack.addTrackData(expUnit.loadTrackData(variables));

                            resultTracks.Add(newResultTrack);
                        }
                    }
                }
            }
            //if groups are used, we have to select one of them using the havingFunction
            //-max(avg(havingVariable)) or min(avg(havingVariable))
            if (groupBy.Count>0 && havingFunction!= FunctionNone)
            {
                foreach (LogQueryResultTrackViewModel track in resultTracks)
                    track.applyHavingSelection(havingFunction, havingVariable);
            }
        }
    }
}
