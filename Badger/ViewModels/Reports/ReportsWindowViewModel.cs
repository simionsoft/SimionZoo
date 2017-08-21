using System;
using System.Xml;
using System.Collections.ObjectModel;
using System.Collections.Specialized;
using System.ComponentModel;
using System.IO;
using Badger.Data;
using Caliburn.Micro;
using OxyPlot;

namespace Badger.ViewModels
{
    public class ReportsWindowViewModel : Conductor<Screen>.Collection.OneActive
    {
        private ObservableCollection<ReportViewModel> m_reports = new ObservableCollection<ReportViewModel>();

        public ObservableCollection<ReportViewModel> Reports => m_reports;

        public ObservableCollection<LoggedForkViewModel> Forks { get; } = new ObservableCollection<LoggedForkViewModel>();

        private bool m_bCanGenerateReports;


        public bool bCanGenerateReports
        {
            get { return m_bCanGenerateReports; }
            set { m_bCanGenerateReports = value; NotifyOfPropertyChange(() => bCanGenerateReports); }
        }

        //plot selection in tab control
        private ReportViewModel m_selectedReport;


        public ReportViewModel selectedReport
        {
            get { return m_selectedReport; }
            set
            {
                m_selectedReport = value;
                NotifyOfPropertyChange(() => selectedReport);
            }
        }


        private bool m_bVariableSelection = true;
        public bool bVariableSelection
        {
            get { return m_bVariableSelection; }
            set
            {
                m_bVariableSelection = value;
                foreach (LoggedVariableViewModel var in Variables)
                {
                    var.bIsSelected = value;
                    ValidateQuery();
                    NotifyOfPropertyChange(() => var.bIsSelected);
                }
            }
        }

        private BindableCollection<string> m_inGroupSelectionVariables = new BindableCollection<string>();

        public BindableCollection<string> inGroupSelectionVariables
        {
            get { return m_inGroupSelectionVariables; }
            set { m_inGroupSelectionVariables = value; NotifyOfPropertyChange(() => inGroupSelectionVariables); }
        }

        // In-Group selection
        private string m_selectedInGroupSelectionFunction = "";
        public string selectedInGroupSelectionFunction
        {
            get { return m_selectedInGroupSelectionFunction; }
            set
            {
                m_selectedInGroupSelectionFunction = value;
                ValidateQuery();
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
                ValidateQuery();
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
                ValidateQuery();
                NotifyOfPropertyChange(() => inGroupSelectionFunctions);
            }
        }

        // From
        private string m_selectedFrom = "";

        public string selectedFrom
        {
            get { return m_selectedFrom; }
            set
            {
                m_selectedFrom = value;
                ValidateQuery();
                NotifyOfPropertyChange(() => selectedFrom);

                foreach (LoggedExperimentViewModel exp in LoggedExperiments)
                    exp.TraverseAction(true, (child) =>
                    {
                        child.bCheckIsVisible = (selectedFrom == LogQuery.fromSelection);
                    });
            }
        }

        private BindableCollection<string> m_fromOptions = new BindableCollection<string>();

        public BindableCollection<string> fromOptions
        {
            get { return m_fromOptions; }
            set { m_fromOptions = value; NotifyOfPropertyChange(() => fromOptions); }
        }

        // Order by
        private bool m_bIsOrderByEnabled;

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


        void item_PropertyChanged(object sender, PropertyChangedEventArgs e)
        {

            if (e.PropertyName == "IsGroupedByThisFork")
            {
                m_groupByForks.Add(((LoggedForkViewModel)sender).name);
                NotifyOfPropertyChange(() => groupBy);
                bGroupsEnabled = true;
            }
            ValidateQuery();

        }

        // Group By
        private BindableCollection<string> m_groupByForks = new BindableCollection<string>();

        public BindableCollection<string> GroupByForks
        {
            get { return m_groupByForks; }
            set
            {
                m_groupByForks = value;
                NotifyOfPropertyChange(() => GroupByForks);
            }
        }

        public string groupBy
        {
            get
            {
                string s = "";
                for (int i = 0; i < m_groupByForks.Count - 1; i++)
                    s += m_groupByForks[i] + ",";

                if (m_groupByForks.Count > 0)
                    s += m_groupByForks[m_groupByForks.Count - 1];

                return s;
            }
        }

        public void ResetGroupBy()
        {
            m_groupByForks.Clear();
            NotifyOfPropertyChange(() => groupBy);
        }


        private bool m_bGroupsEnabled; // no groups by default

        public bool bGroupsEnabled
        {
            get { return m_bGroupsEnabled; }
            set { m_bGroupsEnabled = value; NotifyOfPropertyChange(() => bGroupsEnabled); }
        }

        // Limit to
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
                // Ordering results only makes sense if results are limited
                bIsOrderByEnabled = (value != LogQuery.noLimitOnResults);
                NotifyOfPropertyChange(() => selectedLimitToOption);
            }
        }

        private bool m_bLogsLoaded;

        public bool bLogsLoaded
        {
            get { return m_bLogsLoaded; }
            set { m_bLogsLoaded = value; NotifyOfPropertyChange(() => bLogsLoaded); }
        }

        /// <summary>
        /// 
        /// </summary>
        public void ValidateQuery()
        {
            // Validate the current query
            int numSelectedVars = 0;

            foreach (LoggedVariableViewModel variable in Variables)
                if (variable.bIsSelected) ++numSelectedVars;

            if (numSelectedVars == 0 || selectedInGroupSelectionVariable == "")
                bCanGenerateReports = false;
            else
                bCanGenerateReports = true;

            // Update the "enabled" property of the variable used to select a group
            bGroupsEnabled = GroupByForks.Count > 0;
        }

        /// <summary>
        ///     Class default constructor.
        /// </summary>
        public ReportsWindowViewModel()
        {
            // Add the function options
            inGroupSelectionFunctions.Add(LogQuery.functionMax);
            inGroupSelectionFunctions.Add(LogQuery.functionMin);
            selectedInGroupSelectionFunction = LogQuery.functionMax;
            // Add the from options
            fromOptions.Add(LogQuery.fromAll);
            fromOptions.Add(LogQuery.fromSelection);
            selectedFrom = LogQuery.fromAll;
            // Add the limit to options
            limitToOptions.Add(LogQuery.noLimitOnResults);
            for (int i = 1; i <= 10; i++) limitToOptions.Add(i.ToString());
            selectedLimitToOption = LogQuery.noLimitOnResults;
            // Add order by functions
            orderByFunctions.Add(LogQuery.orderAsc);
            orderByFunctions.Add(LogQuery.orderDesc);
            selectedOrderByFunction = LogQuery.orderDesc;
        }

        /// <summary>
        ///     Method called from the view. Generate a report from a set of selected configurations once
        ///     all conditions are fulfilled.
        /// </summary>
        public void MakeReport()
        {
            // Fill the LogQuery data
            LogQuery query = new LogQuery { @from = selectedFrom };
            // Group by
            foreach (string fork in m_groupByForks) query.groupBy.Add(fork);

            if (query.groupBy.Count > 0)
            {
                query.inGroupSelectionFunction = selectedInGroupSelectionFunction;
                query.inGroupSelectionVariable = selectedInGroupSelectionVariable;
            }
            // Order by
            query.limitToOption = selectedLimitToOption;
            if (selectedLimitToOption != LogQuery.noLimitOnResults)
            {
                query.orderByFunction = selectedOrderByFunction;
                query.orderByVariable = selectedOrderByVariable;
            }

            // Execute the query
            query.execute(LoggedExperiments, Variables);
            // Display the report
            ReportViewModel newReport = new ReportViewModel(query);
            Reports.Add(newReport);
            selectedReport = newReport;
            bCanSaveReports = true;
        }

        public BindableCollection<LoggedVariableViewModel> Variables { get; }
            = new BindableCollection<LoggedVariableViewModel>();

        private bool m_bCanSaveReports;

        public bool bCanSaveReports
        {
            get { return m_bCanSaveReports; }
            set
            {
                m_bCanSaveReports = value;
                NotifyOfPropertyChange(() => bCanSaveReports);
            }
        }

        /// <summary>
        ///     Method called from the view. When the report is generated it can be saved in a folder 
        ///     as a set of separated files containing the report data for further analysis.
        /// </summary>
        public void SaveReports()
        {
            if (m_reports.Count == 0) return;

            string outputBaseFolder =
                CaliburnUtility.SelectFolder(SimionFileData.imageRelativeDir);

            if (outputBaseFolder != "")
            {
                foreach (ReportViewModel report in m_reports)
                {
                    // If there is more than one report, we store each one in a subfolder
                    string outputFolder;
                    if (m_reports.Count > 1)
                    {
                        outputFolder = outputBaseFolder + "\\" + Utility.removeSpecialCharacters(report.name);
                        Directory.CreateDirectory(outputFolder);
                    }
                    else
                        outputFolder = outputBaseFolder;

                    report.export(outputFolder);
                }
            }
        }


        private BindableCollection<LoggedExperimentViewModel> m_loggedExperiments
            = new BindableCollection<LoggedExperimentViewModel>();

        public BindableCollection<LoggedExperimentViewModel> LoggedExperiments
        {
            get { return m_loggedExperiments; }
            set { m_loggedExperiments = value; NotifyOfPropertyChange(() => LoggedExperiments); }
        }


        private void LoadLoggedExperiment(XmlNode node, string baseDirectory)
        {
            LoggedExperimentViewModel newExperiment = new LoggedExperimentViewModel(node, baseDirectory, true);
            LoggedExperiments.Add(newExperiment);

            //the harsh way, because Observable collection doesn't implement Exists
            //and Contains will look for the same object, not just an object with the same name
            foreach (LoggedVariableViewModel var in newExperiment.variables)
            {
                bool bFound = false;
                foreach (LoggedVariableViewModel existingVar in Variables)
                    if (var.name == existingVar.name) bFound = true;
                if (!bFound)
                    Variables.Add(var);
            }

            foreach (LoggedVariableViewModel variable in Variables)
                variable.setParent(this);

            bLogsLoaded = true;

            if (Variables.Count > 0)
            {
                selectedInGroupSelectionVariable = Variables[0].name;
                selectedOrderByVariable = Variables[0].name;
            }

            foreach (var fork in newExperiment.Forks)
            {
                fork.PropertyChanged += item_PropertyChanged;
                Forks.Add(fork);
            }
        }


        public void LoadExperimentBatch(string batchFileName)
        {
            SimionFileData.LoadExperimentBatchFile(LoadLoggedExperiment, batchFileName);
        }

        /// <summary>
        ///     Close a tab (report view) and remove it from the list of reports.
        /// </summary>
        /// <param name="report">The report to be removed</param>
        public void Close(ReportViewModel report)
        {
            Reports.Remove(report);
            selectedReport = null;
        }

        /// <summary>
        ///     Method called from the view.
        /// </summary>
        public void ClearReportViewer()
        {
            LoggedExperiments.Clear();
            Reports.Clear();
            ResetGroupBy();
            inGroupSelectionVariables.Clear();
            Variables.Clear();
            GroupByForks.Clear();
            Forks.Clear();

            selectedLimitToOption = "-";
            selectedFrom = "*";
            bCanGenerateReports = false;
            bCanSaveReports = false;
            bLogsLoaded = false;
            bVariableSelection = true;
            Refresh();
        }
    }
}
