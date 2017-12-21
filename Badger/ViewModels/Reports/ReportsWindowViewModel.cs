using System.Xml;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.IO;
using Badger.Data;
using Caliburn.Micro;
using Badger.Simion;

namespace Badger.ViewModels
{
    public class ReportsWindowViewModel : Conductor<Screen>.Collection.OneActive
    {
        string m_loadedBatch = "No batch has been loaded";
        public string LoadedBatch { get { return m_loadedBatch; } set { m_loadedBatch = value; NotifyOfPropertyChange(() => LoadedBatch); } }
                
        public ObservableCollection<ReportViewModel> Reports { get; } = new ObservableCollection<ReportViewModel>();

        public ObservableCollection<LoggedExperimentalUnitViewModel> ExperimentalUnits { get; } = new ObservableCollection<LoggedExperimentalUnitViewModel>();

        private bool m_bCanGenerateReports;

        private bool m_bResampleData = true;
        public bool ResampleData { get { return m_bResampleData; } set { m_bResampleData = value;NotifyOfPropertyChange(() => ResampleData); } }
        private int m_ResamplingNumPoints = 100;
        public string ResamplingNumPoints
        {
            get { return m_ResamplingNumPoints.ToString(); }
            set
            {
                if (int.TryParse(value, out m_ResamplingNumPoints))
                    NotifyOfPropertyChange(() => ResamplingNumPoints);
                else CaliburnUtility.ShowWarningDialog("Error", "Wrong input: needs to be an integer number");
            }
        }
        public bool CanGenerateReports
        {
            get { return m_bCanGenerateReports; }
            set { m_bCanGenerateReports = value; NotifyOfPropertyChange(() => CanGenerateReports); }
        }

        //plot selection in tab control
        private ReportViewModel m_selectedReport;

        public ReportViewModel SelectedReport
        {
            get { return m_selectedReport; }
            set
            {
                m_selectedReport = value;
                NotifyOfPropertyChange(() => SelectedReport);
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
                    var.IsSelected = value;
                    ValidateQuery();
                    NotifyOfPropertyChange(() => var.IsSelected);
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

        /// <summary>
        ///     Add a fork to the "GroupByFork" list when a property of a LoggedForkValues changes.
        ///     The item added to the list is the one with the changes in one of its properties.
        /// </summary>
        /// <param name="sender">The object with the change in a property</param>
        /// <param name="e">The launched event</param>
        void OnChildPropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            //not all properties sending changes are due to "Group by this fork", so we need to check it
            if (e.PropertyName == "IsGroupedByThisFork")
            {
                if (!m_groupByForks.Contains(((LoggedForkViewModel)sender).Name))
                {
                    m_groupByForks.Add(((LoggedForkViewModel)sender).Name);
                    NotifyOfPropertyChange(() => groupBy);
                }
                GroupsEnabled = true;
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
            GroupsEnabled = false;
            m_groupByForks.Clear();
            NotifyOfPropertyChange(() => groupBy);
        }


        private bool m_bGroupsEnabled; // no groups by default

        public bool GroupsEnabled
        {
            get { return m_bGroupsEnabled; }
            set { m_bGroupsEnabled = value; NotifyOfPropertyChange(() => GroupsEnabled); }
        }

        // Limit to
        private BindableCollection<string> m_limitToOptions = new BindableCollection<string>();

        public BindableCollection<string> limitToOptions
        {
            get { return m_limitToOptions; }
            set { m_limitToOptions = value; NotifyOfPropertyChange(() => limitToOptions); }
        }


        /// <summary>
        /// The selected number of maximum tracks
        /// </summary>
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

        /// <summary>
        /// With this property we control wether to use fork selection. By default, all forks (all experimental units)
        /// are considered to create the report
        /// </summary>
        private bool m_bUseForkSelection = false;
        public bool UseForkSelection
        {
            get { return m_bUseForkSelection; }
            set
            {
                m_bUseForkSelection = value; NotifyOfPropertyChange(() => UseForkSelection);
                ValidateQuery();

                foreach (LoggedExperimentViewModel exp in LoggedExperiments)
                    exp.TraverseAction(true, (child) =>
                    {
                        child.IsCheckVisible = UseForkSelection;
                    });
            }
        }

        /// <summary>
        /// This property tells us whether some batch has been loaded already or not
        /// </summary>
        private bool m_bLogsLoaded;
        public bool LogsLoaded
        {
            get { return m_bLogsLoaded; }
            set { m_bLogsLoaded = value; NotifyOfPropertyChange(() => LogsLoaded); }
        }

        /// <summary>
        /// This function validates the current query and sets CanGenerateReports accordingly
        /// </summary>
        public void ValidateQuery()
        {
            // Validate the current query
            int numSelectedVars = 0;

            foreach (LoggedVariableViewModel variable in Variables)
                if (variable.IsSelected) ++numSelectedVars;

            if (numSelectedVars == 0 || selectedInGroupSelectionVariable == "")
                CanGenerateReports = false;
            else
                CanGenerateReports = true;

            // Update the "enabled" property of the variable used to select a group
            GroupsEnabled = GroupByForks.Count > 0;
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
            // Add the limit to options
            limitToOptions.Add(LogQuery.noLimitOnResults);
            for (int i = 1; i <= 10; i++) limitToOptions.Add(i.ToString());
            selectedLimitToOption = LogQuery.noLimitOnResults;
            // Add order by functions
            orderByFunctions.Add(LogQuery.functionMax);
            orderByFunctions.Add(LogQuery.functionMin);
            selectedOrderByFunction = LogQuery.functionMax;
        }

        /// <summary>
        ///     Method called from the view. Generate a report from a set of selected configurations once
        ///     all conditions are fulfilled.
        /// </summary>
        public void MakeReport()
        {
            // Fill the LogQuery data
            LogQuery query = new LogQuery();
            //use fork selection?
            query.UseForkSelection = UseForkSelection;
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

            //set resampling parametres
            query.ResampleData = ResampleData;
            if (ResampleData)
                query.ResamplingNumPoints = int.Parse(ResamplingNumPoints);

            // Execute the query
            query.Execute(LoggedExperiments, Variables);
            // Display the report
            foreach (Report report in query.Reports)
            {
                ReportViewModel newReport = new ReportViewModel(query, report);
                Reports.Add(newReport);
            }
            if (Reports.Count > 0)
            {
                SelectedReport = Reports[0];
                CanSaveReports = true;
            }
        }

        public BindableCollection<LoggedVariableViewModel> Variables { get; }
            = new BindableCollection<LoggedVariableViewModel>();

        private bool m_bCanSaveReports;

        public bool CanSaveReports
        {
            get { return m_bCanSaveReports; }
            set
            {
                m_bCanSaveReports = value;
                NotifyOfPropertyChange(() => CanSaveReports);
            }
        }

        /// <summary>
        /// Are there any variables in the logs we loaded? This property is used to enable/disable variable-related 
        /// options
        /// </summary>

        public bool VariablesLoaded
        {
            get { return Variables.Count > 0; }
        }


        private int m_numTotalForks = 0;
        /// <summary>
        /// Are there any forks in the logs we loaded? This property is used to enable/disable fork-related options
        /// </summary>
        public bool ForksLoaded
        {
            get { return m_numTotalForks > 0; }
        }

        /// <summary>
        ///     Method called from the view. When the report is generated it can be saved in a folder 
        ///     as a set of separated files containing the report data for further analysis.
        /// </summary>
        public void SaveReports()
        {
            if (Reports.Count == 0) return;

            string outputBaseFolder =
                CaliburnUtility.SelectFolder(SimionFileData.imageRelativeDir);

            if (outputBaseFolder != "")
            {
                foreach (ReportViewModel report in Reports)
                {
                    // If there is more than one report, we store each one in a subfolder
                    string outputFolder;
                    if (Reports.Count > 1)
                    {
                        outputFolder = outputBaseFolder + "\\" + Utility.RemoveSpecialCharacters(report.name);
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

            //add all experimental units to the collection
           
            foreach (LoggedExperimentViewModel experiment in LoggedExperiments)
            {
                experiment.TraverseAction(false, (n) =>
                 {
                     //get all the experimental units in a list
                     if (n is LoggedExperimentalUnitViewModel expUnit)
                         ExperimentalUnits.Add(expUnit);

                     //count the number of forks in this batch
                     if (n is LoggedForkViewModel fork)
                         m_numTotalForks++;
                 });
            }

            //Update flags use to enable/disable parts of the report generation menu
            NotifyOfPropertyChange(() => ForksLoaded);
            NotifyOfPropertyChange(() => VariablesLoaded);
            if (Variables.Count > 0)
            {
                LogsLoaded = true;
                selectedInGroupSelectionVariable = Variables[0].name;
                selectedOrderByVariable = Variables[0].name;
            }

            // Add a property change listener to each node in the tree
            newExperiment.TraverseAction(true, (n) => {n.PropertyChanged += OnChildPropertyChanged; });
        }

        /// <summary>
        ///     Load an experiment from a batch file. The batch should be from an already finished
        ///     experiment, this is in order to make reports correctly but is not mandatory.
        ///     We clear the previously loaded data to avoid mixing data from two different batches
        /// </summary>
        /// <param name="batchFileName">The name of the file to load</param>
        public void LoadExperimentBatch(string batchFileName)
        {
            ClearReportViewer();
            LoadedBatch = batchFileName;
            SimionFileData.LoadExperimentBatchFile(LoadLoggedExperiment, batchFileName);
        }

        /// <summary>
        ///     Close a tab (report view) and remove it from the list of reports.
        /// </summary>
        /// <param name="report">The report to be removed</param>
        public void Close(ReportViewModel report)
        {
            Reports.Remove(report);
            if (Reports.Count > 0)
                SelectedReport = Reports[0];
            else SelectedReport = null;
        }

        /// <summary>
        ///     Method called from the view. This clear every list and field. Should be called when
        ///     we load a new experiment if one is already loaded or when we hit the delete button
        ///     from the view.
        /// </summary>
        public void ClearReportViewer()
        {
            ExperimentalUnits.Clear();
            LoggedExperiments.Clear();
            Reports.Clear();
            ResetGroupBy();
            inGroupSelectionVariables.Clear();
            Variables.Clear();
            GroupByForks.Clear();
            m_numTotalForks = 0;

            NotifyOfPropertyChange(() => VariablesLoaded);
            NotifyOfPropertyChange(() => ForksLoaded);

            selectedLimitToOption = "-";
            UseForkSelection = false;
            CanGenerateReports = false;
            CanSaveReports = false;
            LogsLoaded = false;
            GroupsEnabled = false;
            bVariableSelection = true;
            Refresh();
        }
    }
}
