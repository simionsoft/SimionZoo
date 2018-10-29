using System.Xml;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.IO;
using Badger.Data;
using Caliburn.Micro;
using Badger.Simion;
using System.Threading.Tasks;
using System.Collections.Generic;

namespace Badger.ViewModels
{
    public class ReportsWindowViewModel : Conductor<Screen>.Collection.OneActive
    {
        LogQueryViewModel m_query = null;
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

        //experimental units
        public BindableCollection<LoggedExperimentalUnitViewModel> ExperimentalUnits
        { get; set; } = new BindableCollection<LoggedExperimentalUnitViewModel>();


        //log query results
        public BindableCollection<LogQueryResultViewModel> LogQueryResults
        { get; set; } = new BindableCollection<LogQueryResultViewModel>();
 

        //plot selection in tab control
        private LogQueryResultViewModel m_selectedLogQueryResult;

        public LogQueryResultViewModel SelectedLogQueryResult
        {
            get { return m_selectedLogQueryResult; }
            set
            {
                m_selectedLogQueryResult = value;
                //avoid cloning the first query when it is selected
                if (value != null && !ReferenceEquals(value.Query, Query))
                {
                    Query = Serialiazer.Clone(value.Query);

                    //IsNotifying property must be manually set because the constructor of PropertyChangedBase is not called when we clone the object
                    Query.SetNotifying(true);
                }
                NotifyOfPropertyChange(() => SelectedLogQueryResult);
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

                Query.Validate();
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

                Query.Execute(LoggedExperiments, OnExperimentalUnitProcessed, out List<TrackGroup> queryResultTracks, out List<Report> queryReports);

                //Clone the query
                LogQueryViewModel clonedQuery = Serialiazer.Clone(Query);

                //Create and add to list the result of the query
                LogQueryResultViewModel result = new LogQueryResultViewModel(queryResultTracks, queryReports, clonedQuery);
                LogQueryResults.Add(result);
                //set this last result as selected
                SelectedLogQueryResult = LogQueryResults[LogQueryResults.Count-1];

                LoadedBatch = batchFilename;
                EndLongOperation();
            });
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
        /// Show a dialog window to select the experiment batch or report to load. Depending on the type of input file selected
        /// , a different load function will be called: either LoadExperimentBatch or LoadReports
        /// </summary>
        public void LoadExperimentBatchOrReport()
        {
            string filter = SimionFileData.ExperimentBatchOrReportFilter;
            string filetype = SimionFileData.ExperimentBatchOrReportFileTypeDescription;

            List<string> filenames = SimionFileData.OpenFileDialogMultipleFiles(filetype, filter, false);

            if (filenames.Count>0)
            {
                string filename = filenames[0];
                string fileExtension = Utility.GetExtension(filename, 2);
                if (fileExtension == SimionFileData.ExperimentBatchExtension)
                    LoadExperimentBatch(filename);
                else if (fileExtension == SimionFileData.ReportExtension)
                    LoadReport(filename);
            }
        }

        /// <summary>
        /// Loads a report previously saved from the Reports window
        /// </summary>
        /// <param name="reportFileName">File where the report was saved using SaveReports()</param>
        public void LoadReport(string reportFileName)
        {
            string readBatchFilename;
            BindableCollection<LogQueryResultViewModel> readQueries;
            int numQueriesRead= SimionFileData.LoadReport(reportFileName, out readBatchFilename, out readQueries);

            if (numQueriesRead>0)
            {
                ClearReportViewer();

                LoadExperimentBatch(readBatchFilename);
                LoadedBatch = readBatchFilename;
                LogQueryResults.AddRange(readQueries);
                SelectedLogQueryResult = LogQueryResults[0];
            }
        }

        /// <summary>
        ///     Method called from the view. When the report is generated it can be saved in a folder.
        ///     Each LogQueryResults object is saved in a different subfolder
        /// </summary>
        public void SaveReport()
        {
            if (LogQueryResults.Count == 0) return;

            string outputBaseFolder =
                CaliburnUtility.SelectFolder(SimionFileData.imageRelativeDir);

            if (outputBaseFolder != "")
                SimionFileData.SaveReport(LoadedBatch, LogQueryResults, outputBaseFolder);
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
                    , SimionFileData.ExperimentBatchDescription, SimionFileData.ExperimentBatchFilter);
                if (!bSuccess)
                    return;
            }

            //reset the view if a batch was succesfully selected
            ClearReportViewer();

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

        public const string GroupByExperimentId = "Experiment-Id";

        /// <summary>
        ///     Close a tab (report view) and remove it from the list of reports.
        /// </summary>
        /// <param name="result">The report to be removed</param>
        public void Close(LogQueryResultViewModel result)
        {
            LogQueryResults.Remove(result);
            if (LogQueryResults.Count > 0)
                SelectedLogQueryResult = LogQueryResults[0];
            else SelectedLogQueryResult = null;
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
            LogQueryResults.Clear();

            NotifyOfPropertyChange(() => VariablesLoaded);
            NotifyOfPropertyChange(() => ForksLoaded);

            Query = new LogQueryViewModel();
            //Add the listening function to the LogQuery object with all the parameters
            Query.PropertyChanged += OnChildPropertyChanged;

            LogsLoaded = false;
            ForksLoaded = false;
        }
    }
}
