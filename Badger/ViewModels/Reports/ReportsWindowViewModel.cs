using System.Xml;
using System.Collections.ObjectModel;
using System.IO;
using Badger.Data;
using Caliburn.Micro;


namespace Badger.ViewModels
{
    public class ReportsWindowViewModel : Conductor<Screen>.Collection.OneActive
    {
        private ObservableCollection<ReportViewModel> m_reports = new ObservableCollection<ReportViewModel>();
        public ObservableCollection<ReportViewModel> reports { get { return m_reports; } }

        private bool m_bCanGenerateReports = false;
        public bool bCanGenerateReports
        {
            get { return m_bCanGenerateReports; }
            set { m_bCanGenerateReports = value; NotifyOfPropertyChange(() => bCanGenerateReports); }
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

        //In-Group selection
        private string m_selectedInGroupSelectionFunction = "";
        public string selectedInGroupSelectionFunction
        {
            get { return m_selectedInGroupSelectionFunction; }
            set
            {
                m_selectedInGroupSelectionFunction = value;
                validateQuery();
                NotifyOfPropertyChange(() => selectedInGroupSelectionFunction);
            }
        }

        private string m_selectedInGroupSelectionVariable = "";
        public string selectedInGroupSelectionVariable
        {
            get { return m_selectedInGroupSelectionVariable; }
            set
            {
                m_selectedInGroupSelectionVariable = value;
                validateQuery();
                NotifyOfPropertyChange(() => selectedInGroupSelectionVariable);
            }
        }

        private BindableCollection<string> m_inGroupSelectionFunctions = new BindableCollection<string>();
        public BindableCollection<string> inGroupSelectionFunctions
        {
            get { return m_inGroupSelectionFunctions; }
            set
            {
                m_inGroupSelectionFunctions = value;
                validateQuery();
                NotifyOfPropertyChange(() => inGroupSelectionFunctions);
            }
        }

        //From
        private string m_selectedFrom = "";
        public string selectedFrom
        {
            get { return m_selectedFrom; }
            set
            {
                m_selectedFrom = value;
                validateQuery();
                NotifyOfPropertyChange(() => selectedFrom);
                foreach (LoggedExperimentViewModel exp in loggedExperiments)
                    exp.TraverseAction(true, (child) =>
                     { child.bCheckIsVisible = (selectedFrom == LogQuery.fromSelection); });
            }
        }
        private BindableCollection<string> m_fromOptions = new BindableCollection<string>();
        public BindableCollection<string> fromOptions
        {
            get { return m_fromOptions; }
            set { m_fromOptions = value; NotifyOfPropertyChange(() => fromOptions); }
        }


        //Order by
        private bool m_bIsOrderByEnabled = false;
        public bool bIsOrderByEnabled
        {
            get { return m_bIsOrderByEnabled; }
            set { m_bIsOrderByEnabled = value; NotifyOfPropertyChange(() => bIsOrderByEnabled); }
        }
        private BindableCollection<string> m_orderByFunctions = new BindableCollection<string>();
        public BindableCollection<string> orderByFunctions
        {
            get { return m_orderByFunctions; }
            set { m_orderByFunctions = value; NotifyOfPropertyChange(() => orderByFunctions); }
        }
        private string m_selectedOrderByFunction = "";
        public string selectedOrderByFunction
        {
            get { return m_selectedOrderByFunction; }
            set { m_selectedOrderByFunction = value; NotifyOfPropertyChange(() => selectedOrderByFunction); }
        }


        private string m_selectedOrderByVariable = "";
        public string selectedOrderByVariable
        {
            get { return m_selectedOrderByVariable; }
            set { m_selectedOrderByVariable = value; NotifyOfPropertyChange(() => selectedOrderByVariable); }
        }

        //Limit to
        private BindableCollection<string> m_limitToOptions = new BindableCollection<string>();
        public BindableCollection<string> limitToOptions
        {
            get { return m_limitToOptions; }
            set { m_limitToOptions = value; NotifyOfPropertyChange(() => limitToOptions); }
        }
        private string m_selectedLimitToOption;
        public string selectedLimitToOption
        {
            get { return m_selectedLimitToOption; }
            set
            {
                m_selectedLimitToOption = value;
                //ordering results only makes sense if results are limited
                bIsOrderByEnabled = (value != LogQuery.noLimitOnResults);
                NotifyOfPropertyChange(() => selectedLimitToOption);
            }
        }

        private bool m_bLogsLoaded = false;
        public bool bLogsLoaded
        {
            get { return m_bLogsLoaded; }
            set { m_bLogsLoaded = value; NotifyOfPropertyChange(() => bLogsLoaded); }
        }

        public void validateQuery()
        {
            //validate the current query
            int numSelectedVars = 0;

            foreach (LoggedVariableViewModel variable in Variables)
                if (variable.bIsSelected) ++numSelectedVars;

            if (numSelectedVars == 0 || selectedInGroupSelectionVariable == "")
                bCanGenerateReports = false;
            else bCanGenerateReports = true;

            //update the "enabled" property of the variable used to select a group
            foreach (var experiment in loggedExperiments)
                foreach (var fork in experiment.Forks)
                    fork.bGroupsEnabled = fork.GroupByForks.Count > 0;
        }

        public ReportsWindowViewModel()
        {
            //add the function options
            inGroupSelectionFunctions.Add(LogQuery.functionMax);
            inGroupSelectionFunctions.Add(LogQuery.functionMin);
            selectedInGroupSelectionFunction = LogQuery.functionMax;

            //add the from options
            fromOptions.Add(LogQuery.fromAll);
            fromOptions.Add(LogQuery.fromSelection);
            selectedFrom = LogQuery.fromAll;

            //add the limit to options
            limitToOptions.Add(LogQuery.noLimitOnResults);
            for (int i = 1; i <= 10; i++) limitToOptions.Add(i.ToString());
            selectedLimitToOption = LogQuery.noLimitOnResults;

            //order by functions
            orderByFunctions.Add(LogQuery.orderAsc);
            orderByFunctions.Add(LogQuery.orderDesc);
            selectedOrderByFunction = LogQuery.orderDesc;
        }

        public void makeReport()
        {
            //FILL the LogQuery data
            LogQuery query = new LogQuery();
            query.from = selectedFrom;
            //group by
            foreach (var experiment in loggedExperiments)
                foreach (var fork in experiment.Forks)
                    foreach (var group in fork.GroupByForks)
                        query.groupBy.Add(group);
            //having
            if (query.groupBy.Count > 0)
            {
                query.inGroupSelectionFunction = selectedInGroupSelectionFunction;
                query.inGroupSelectionVariable = selectedInGroupSelectionVariable;
            }
            //orderBy
            query.limitToOption = selectedLimitToOption;
            if (selectedLimitToOption != LogQuery.noLimitOnResults)
            {
                query.orderByFunction = selectedOrderByFunction;
                query.orderByVariable = selectedOrderByVariable;
            }

            //EXECUTE the query
            query.execute(loggedExperiments, Variables);

            //DISPLAY the report
            ReportViewModel newReport = new ReportViewModel(query, this);
            reports.Add(newReport);
            selectedReport = newReport;
            bCanSaveReports = true;
        }

        public BindableCollection<LoggedVariableViewModel> Variables { get; }
            = new BindableCollection<LoggedVariableViewModel>();

        private bool m_bCanSaveReports = false;

        public bool bCanSaveReports
        {
            get { return m_bCanSaveReports; }
            set
            {
                m_bCanSaveReports = value;
                NotifyOfPropertyChange(() => bCanSaveReports);
            }
        }

        public void saveReports()
        {
            if (m_reports.Count == 0) return;

            string outputBaseFolder =
                CaliburnUtility.SelectFolder(SimionFileData.imageRelativeDir);

            if (outputBaseFolder != "")
            {
                foreach (ReportViewModel report in m_reports)
                {
                    //if there is more than one report, we store each one in a subfolder
                    string outputFolder;
                    if (m_reports.Count > 1)
                    {
                        outputFolder = outputBaseFolder + "\\" + Utility.removeSpecialCharacters(report.name);
                        Directory.CreateDirectory(outputFolder);
                    }
                    else outputFolder = outputBaseFolder;

                    report.export(outputFolder);
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
            Variables.AddRange(newExperiment.variables);
            bLogsLoaded = true;

            if (Variables.Count > 0)
            {
                selectedInGroupSelectionVariable = Variables[0].name;
                selectedOrderByVariable = Variables[0].name;
            }
        }

        public void loadExperimentBatch(string batchFileName)
        {
            SimionFileData.LoadExperimentBatchFile(loadLoggedExperiment, batchFileName);
        }

        public void close(ReportViewModel report)
        {
            reports.Remove(report);
        }

        public void clearReportViewer()
        {
            loggedExperiments.Clear();
            reports.Clear();
            // resetGroupBy();
            Variables.Clear();

            foreach (var experiment in loggedExperiments)
            {
                experiment.variables.Clear();
                experiment.inGroupSelectionVariables.Clear();
                experiment.orderByVariables.Clear();
                experiment.bVariableSelection = true;
            }

            selectedLimitToOption = "-";
            selectedFrom = "*";
            bCanGenerateReports = false;
            bCanSaveReports = false;
            bLogsLoaded = false;
            Refresh();
        }
    }
}
