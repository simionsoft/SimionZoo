using System.Xml;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.IO;
using Badger.Data;
using Caliburn.Micro;
using Badger.Simion;
using System.Threading.Tasks;

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

        string m_loadedBatch = "No batch loaded";
        public string LoadedBatch
        {
            get { return m_loadedBatch; }
            set { m_loadedBatch = value; NotifyOfPropertyChange(() => LoadedBatch); }
        }
                
        public BindableCollection<ReportViewModel> Reports
        { get; } = new BindableCollection<ReportViewModel>();

        public BindableCollection<LoggedExperimentalUnitViewModel> ExperimentalUnits
        { get; } = new BindableCollection<LoggedExperimentalUnitViewModel>();


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
                    exp.IsCheckVisible = Query.UseForkSelection;

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
            Task.Run(() =>
            {
                StartLongOperation();
                // Execute the Query
                string batchFilename = LoadedBatch;

                LoadedBatch = "Running query";
                Query.Execute(LoggedExperiments, OnExperimentalUnitProcessed);

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
                LoadedBatch = batchFilename;
                EndLongOperation();
            });
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


        /// <summary>
        /// Are there any forks in the logs we loaded? This property is used to enable/disable fork-related options
        /// </summary>
        private bool m_bForksLoaded = false;
        public bool ForksLoaded
        { get { return m_bForksLoaded; } set { m_bForksLoaded = value; NotifyOfPropertyChange(() => ForksLoaded); } }

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

        
        private int LoadLoggedExperiment(XmlNode node, string baseDirectory
            , SimionFileData.LoadUpdateFunction loadUpdateFunction)
        {
            LoggedExperimentViewModel newExperiment
                = new LoggedExperimentViewModel(node, baseDirectory, true, false, loadUpdateFunction);

            LoggedExperiments.Add(newExperiment);
            ExperimentalUnits.AddRange(newExperiment.ExperimentalUnits);
            Query.AddLogVariables(newExperiment.VariablesInLog);
            ForksLoaded |= newExperiment.Forks.Count > 0;

            newExperiment.TraverseAction(true, (n)=>
            {
                if (n is LoggedForkViewModel fork)
                    fork.PropertyChanged += OnChildPropertyChanged;
            });

            return ExperimentalUnits.Count;
        }

        private double m_loadProgress = 0.0;
        public double LoadProgress
        {
            get { return m_loadProgress; }
            set { m_loadProgress = value; NotifyOfPropertyChange(() => LoadProgress); }
        }
        private bool m_bLoading = false;
        public bool Loading { get { return m_bLoading; } set { m_bLoading = value;NotifyOfPropertyChange(() => Loading); } }

        int m_numExperimentalUnits = 0;
        public int NumExperimentalUnits
        {
            get { return m_numExperimentalUnits; }
        }

        int m_numProcessedExperimentalUnits = 0;
        public void OnExperimentalUnitProcessed()
        {
            m_numProcessedExperimentalUnits++;
            if (m_numExperimentalUnits != 0)
                LoadProgress = (double)m_numProcessedExperimentalUnits / (double)m_numExperimentalUnits;

        }
        void StartLongOperation()
        {
            LoadProgress = 0;
            m_numProcessedExperimentalUnits = 0;
            Loading = true;
        }
        void EndLongOperation()
        {
            LoadProgress = 1;
            Loading = false;
        }
        /// <summary>
        ///     Load an experiment from a batch file. If <paramref name="batchFileName"/> is either
        ///     null or empty, a dialog window will be opened to let the user select a batch file.
        /// </summary>
        /// <param name="batchFileName">The name of the file to load</param>
        public void LoadExperimentBatch(string batchFileName)
        {
            //Ask the user for the name of the batch
            if (string.IsNullOrEmpty(batchFileName))
            {
                bool bSuccess = SimionFileData.OpenFileDialog(ref batchFileName
                    , SimionFileData.ExperimentBatchFilter, XMLConfig.experimentBatchExtension);
                if (!bSuccess)
                    return;
            }

            //reset the view and the query if a batch was succesfully selected
            ClearReportViewer();
            Query.OnExperimentBatchLoaded();

            //Inefficient but not so much as to care
            //First we load the batch file to cout how many experimental units we have
            StartLongOperation();
            LoadedBatch = "Reading batch file";
            m_numExperimentalUnits = SimionFileData.LoadExperimentBatchFile(batchFileName
                , SimionFileData.CountExperimentalUnitsInBatch);

            Task.Run(() =>
            {
                //load the batch
                LoadedBatch = "Reading experiment files";
                SimionFileData.LoadExperimentBatchFile(batchFileName, LoadLoggedExperiment, OnExperimentalUnitProcessed);

                //Update flags use to enable/disable parts of the report generation menu
                NotifyOfPropertyChange(() => ForksLoaded);
                NotifyOfPropertyChange(() => VariablesLoaded);

                LoadedBatch = batchFileName;
                LogsLoaded = true;

                EndLongOperation();
            });
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

            NotifyOfPropertyChange(() => VariablesLoaded);
            NotifyOfPropertyChange(() => ForksLoaded);

            Query.Reset();

            CanSaveReports = false;
            LogsLoaded = false;
            ForksLoaded = false;
        }
    }
}
