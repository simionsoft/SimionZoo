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
        LogQueryViewModel m_query = new LogQueryViewModel();
        public LogQueryViewModel Query
        {
            get { return m_query; }
            set { m_query = value; NotifyOfPropertyChange(() => Query); }
        }

        string m_loadedBatch = "No batch has been loaded";
        public string LoadedBatch { get { return m_loadedBatch; } set { m_loadedBatch = value; NotifyOfPropertyChange(() => LoadedBatch); } }
                
        public ObservableCollection<ReportViewModel> Reports { get; } = new ObservableCollection<ReportViewModel>();

        public ObservableCollection<LoggedExperimentalUnitViewModel> ExperimentalUnits { get; } = new ObservableCollection<LoggedExperimentalUnitViewModel>();


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
                Query.AddGroupByFork(((LoggedForkViewModel)sender).Name);
            }
            else if (e.PropertyName=="UseForkSelection")
            {
                foreach (LoggedExperimentViewModel exp in LoggedExperiments)
                    exp.TraverseAction(true, (child) =>
                    {
                        child.IsCheckVisible = Query.UseForkSelection;
                    });
                Query.ValidateQuery();
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
        ///     Class default constructor.
        /// </summary>
        public ReportsWindowViewModel()
        {
            //Add the listening function to the LogQuery object with all the parameters
            Query.PropertyChanged += OnChildPropertyChanged;
        }

        /// <summary>
        ///     Method called from the view. Generate a report from a set of selected configurations once
        ///     all conditions are fulfilled.
        /// </summary>
        public void MakeReport()
        {
            // Execute the Query
            Query.Execute(LoggedExperiments);

            // Display the reports
            foreach (Report report in Query.Reports)
            {
                ReportViewModel newReport = new ReportViewModel(Query, report);
                Reports.Add(newReport);
            }
            if (Reports.Count > 0)
            {
                SelectedReport = Reports[0];
                CanSaveReports = true;
            }
        }


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
            get { return Query.VariablesVM.Count > 0; }
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

                    report.Export(outputFolder);
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

            Query.AddLogVariables(newExperiment.VariablesInLog);

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

            UpdateMenuState();

            // Add a property change listener to each node in the tree
            newExperiment.TraverseAction(true, (n) => {n.PropertyChanged += OnChildPropertyChanged; });
        }

        void UpdateMenuState()
        {
            //Update flags use to enable/disable parts of the report generation menu
            NotifyOfPropertyChange(() => ForksLoaded);
            NotifyOfPropertyChange(() => VariablesLoaded);
            Query.Init();
            LogsLoaded = true;
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
            Query = new LogQueryViewModel();
            Query.PropertyChanged += OnChildPropertyChanged;

            m_numTotalForks = 0;

            NotifyOfPropertyChange(() => VariablesLoaded);
            NotifyOfPropertyChange(() => ForksLoaded);

            CanSaveReports = false;
            LogsLoaded = false;
            Refresh();
        }
    }
}
