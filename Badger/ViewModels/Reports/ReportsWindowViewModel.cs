using System.Xml;
using System.Collections.ObjectModel;
using System.Collections.Generic;
using Badger.Data;
using Caliburn.Micro;


namespace Badger.ViewModels
{
    public class ReportsWindowViewModel : Conductor<ReportViewModel>.Collection.OneActive
    {

        private ObservableCollection<ReportViewModel> m_reports = new ObservableCollection<ReportViewModel>();
        public ObservableCollection<ReportViewModel> reports { get { return m_reports; } set { } }

        private bool m_bCanGenerateReports = false;
        public bool bCanGenerateReports { get { return m_bCanGenerateReports; }
            set { m_bCanGenerateReports = value; NotifyOfPropertyChange(() => bCanGenerateReports); } }


        //Having
        private string m_selectedHavingFunction = "";
        public string selectedHavingFunction
        {
            get { return m_selectedHavingFunction; }
            set { m_selectedHavingFunction = value;
                validateQuery();
                NotifyOfPropertyChange(() => selectedHavingFunction); }
        }
        private string m_selectedHavingVariable = "";
        public string selectedHavingVariable
        {
            get { return m_selectedHavingVariable; }
            set { m_selectedHavingVariable = value; validateQuery(); NotifyOfPropertyChange(() => selectedHavingVariable); }
        }
        private BindableCollection<string> m_havingFunctions = new BindableCollection<string>();
        public BindableCollection<string> havingFunctions
        {
            get { return m_havingFunctions; }
            set { m_havingFunctions = value; validateQuery(); NotifyOfPropertyChange(() => havingFunctions); }
        }
        private BindableCollection<string> m_havingVariables = new BindableCollection<string>();
        public BindableCollection<string> havingVariables
        {
            get { return m_havingVariables; }
            set { m_havingVariables = value;  NotifyOfPropertyChange(() => havingVariables); }
        }
        private bool m_bGroupsEnabled = false; //no groups by default
        public bool bGroupsEnabled
        {
            get { return m_bGroupsEnabled; }
            set { m_bGroupsEnabled = value; NotifyOfPropertyChange(() => bGroupsEnabled); }
        }

        //Group By
        private BindableCollection<string> m_groupByForks = new BindableCollection<string>();
        public string groupBy
        {
            get
            {
                string s = "";
                for (int i = 0; i < m_groupByForks.Count - 1; i++) s += m_groupByForks[i] + ",";
                if (m_groupByForks.Count > 0) s += m_groupByForks[m_groupByForks.Count - 1];
                return s;
            }
        }
        public void addGroupBy(string forkName)
        {
            m_groupByForks.Add(forkName);
            validateQuery();
            NotifyOfPropertyChange(() => groupBy);
            bGroupsEnabled = true;
        }
        public void resetGroupBy()
        {
            m_groupByForks.Clear();
            NotifyOfPropertyChange(() => groupBy);
        }

        //From
        private string m_selectedFrom = "";
        public string selectedFrom
        {
            get { return m_selectedFrom; }
            set
            {
                m_selectedFrom = value; validateQuery(); NotifyOfPropertyChange(() => selectedFrom);
                foreach (LoggedExperimentViewModel exp in loggedExperiments)
                    exp.TraverseAction(true,(child) => 
                    { child.bCheckIsVisible = (selectedFrom == LogQuery.FromSelection); });
            }
        }
        private BindableCollection<string> m_fromOptions = new BindableCollection<string>();
        public BindableCollection<string> fromOptions
        {
            get { return m_fromOptions; }
            set { m_fromOptions = value; NotifyOfPropertyChange(() => fromOptions); }
        }

        //Variables
        private BindableCollection<LoggedVariableViewModel> m_variables
            = new BindableCollection<LoggedVariableViewModel>();
        public BindableCollection<LoggedVariableViewModel> variables
        {
            get { return m_variables; }
            set { m_variables = value; NotifyOfPropertyChange(() => variables); }
        }
        //to be called by children experimental units after reading their log file descriptor
        public void addVariable(string variable)
        {
            bool bVarExists = false;
            foreach (LoggedVariableViewModel var in variables) if (var.name == variable) bVarExists= true;
            if (!bVarExists)
            {
                variables.Add(new LoggedVariableViewModel(variable, this));
                havingVariables.Add(variable);
            }
        }

        private bool m_bLogsLoaded= false;
        public bool bLogsLoaded
        {
            get { return m_bLogsLoaded; }
            set { m_bLogsLoaded = value; NotifyOfPropertyChange(() => bLogsLoaded); }
        }

        public void validateQuery()
        {
            //validate the current query
            int numSelectedVars = 0;
            foreach (LoggedVariableViewModel variable in variables) if (variable.bIsSelected) ++numSelectedVars;
            if (numSelectedVars==0 || selectedHavingVariable=="")
                bCanGenerateReports = false;
            else bCanGenerateReports = true;

            //update the "enabled" property of the variable used to select a group
            bGroupsEnabled = m_groupByForks.Count>0;
        }

        public ReportsWindowViewModel()
        {
            //add the function options
            havingFunctions.Add(LogQuery.FunctionMax);
            havingFunctions.Add(LogQuery.FunctionMin);
            selectedHavingFunction = LogQuery.FunctionMax;

            //add the from options
            fromOptions.Add(LogQuery.FromAll);
            fromOptions.Add(LogQuery.FromSelection);
            selectedFrom = LogQuery.FromAll;
        }

        public void makeReport()
        {
            LogQuery query= new LogQuery();
            query.from = selectedFrom;
            //group by
            foreach (string fork in m_groupByForks) query.groupBy.Add(fork);
            //having
            if (query.groupBy.Count > 0)
            {
                query.havingFunction = selectedHavingFunction;
                query.havingVariable = selectedHavingVariable;
            }
            //EXECUTE the query
            query.execute(loggedExperiments,variables);

            //Show the report
            ReportViewModel newReport = new ReportViewModel(query);
            reports.Add(newReport);
            selectedReport = newReport;
            bCanSaveReports = true;
        }

  
        public void generateStats()
        {
            //StatsViewModel statsViewModel = new StatsViewModel("Stats");
            //statsViewModel.parent = this;

            //foreach (LoggedExperimentViewModel log in m_selectedLogs)
            //{
            //    //List<Stat> stats = log.getVariableStats(m_selectedVariables);
            //    //foreach (Stat stat in stats)
            //    //{
            //    //    statsViewModel.addStat(stat);
            //    //}
            //}

            //bCanSaveReports = true;

            //reports.Add(statsViewModel);
            //selectedReport = statsViewModel;
        }

        //plot selection in tab control
        private ReportViewModel m_selectedReport = null;
        public ReportViewModel selectedReport
        {
            get { return m_selectedReport; }
            set
            {
                m_selectedReport = value;
                if (m_selectedReport != null) m_selectedReport.updateView();
                NotifyOfPropertyChange(() => selectedReport);
            }
        }

        private bool m_bCanSaveReports = false;
        public bool bCanSaveReports { get { return m_bCanSaveReports; } set { m_bCanSaveReports = value; NotifyOfPropertyChange(() => bCanSaveReports); } }

        public void saveReports()
        {
            string outputFolder= CaliburnUtility.selectFolder(SimionFileData.imageRelativeDir);
            if (outputFolder!="")
            { 
                foreach(ReportViewModel plot in m_reports)
                {
                    plot.export(outputFolder);
                }
            }
        }

        private BindableCollection<LoggedExperimentViewModel> m_loggedExperiments
            = new BindableCollection<LoggedExperimentViewModel>();
        public BindableCollection<LoggedExperimentViewModel> loggedExperiments
        {
            get { return m_loggedExperiments; }
            set { m_loggedExperiments = value; NotifyOfPropertyChange(() => loggedExperiments); }
        }

        private void loadLoggedExperiment(XmlNode node)
        {
            LoggedExperimentViewModel newExperiment = new LoggedExperimentViewModel(node, this);
            loggedExperiments.Add(newExperiment);
            bLogsLoaded = true;
            if (variables.Count>0) selectedHavingVariable = variables[0].name;
        }

        public void loadExperimentBatch()
        {
            SimionFileData.loadExperimentBatch(loadLoggedExperiment);
        }
        public void loadExperimentBatch(string batchFileName)
        {
            SimionFileData.loadExperimentBatch(loadLoggedExperiment, batchFileName);
        }

        public void close(ReportViewModel report)
        {
            reports.Remove(report);
        }
    }
}
